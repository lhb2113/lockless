#include "channel_pipeline_context.h"
#include "executor.h"
#include "channel_pipeline.h"
#include "channel_data.h"

#include <boost/algorithm/string.hpp>

OPEN_LOCKLESS_NAMESPACE

channel_pipeline_context::channel_pipeline_context()
    : m_main_executor(new executor(GEN_UUID, 1))
    , m_context(new runtime_context(m_main_executor->worker_service()))
    , m_root_domain(new channel_pipeline_domain(nullptr, "root"))
{
}

void channel_pipeline_context::synchronize()
{
    m_main_executor->syncronize();
}

void channel_pipeline_context::startup()
{
    m_main_executor->ps_started.subscribe(shared_this, m_context, [this](std::shared_ptr<lockless::object> _subscriber, int _pos){
        ps_started.publish();
    });
    m_main_executor->startup();
}

void channel_pipeline_context::shutdown()
{
    m_main_executor->shutdown();
}

std::string channel_pipeline_context::to_string()
{
    return m_root_domain->to_string(0);
}

bool channel_pipeline_context::add_pipeline(std::shared_ptr<channel_pipeline> _pipeline)
{
    AUTO_TRACE(m_context->uuid());
    std::vector<std::string> _domains;
    auto pipeline_name = _pipeline->get_name();
    boost::algorithm::split(_domains, pipeline_name, boost::algorithm::is_any_of("."));

    if (_domains.size() == 0 || _pipeline.get() == NULL) {
        return false;
    }
    
    bool suc = m_root_domain->add_pipeline(_domains, _pipeline);
    if (suc) {
        ps_pipeline_added.publish(_pipeline);
    }
    LOG_INFO_1(m_context->uuid(), "channel_pipeline_context::add_pipeline %s", pipeline_name.c_str());
    dump();
    return suc;
}

bool channel_pipeline_context::remove_pipeline(std::string _pipeline_name)
{
    AUTO_TRACE(m_context->uuid());
    std::vector<std::string> _domains;
    boost::algorithm::split(_domains, _pipeline_name, boost::algorithm::is_any_of("."));

    if (_domains.size() == 0) {
        return false;
    }
    bool suc = m_root_domain->remove_pipeline(_domains);
    if (suc) {
        ps_pipeline_removed.publish(_pipeline_name);
    }
    LOG_INFO_1(m_context->uuid(), "channel_pipeline_context::remove_pipeline %s", _pipeline_name.c_str());
    dump();
    return suc;
}

int channel_pipeline_context::route_read_data(std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info)
{
    AUTO_TRACE(m_context->uuid());
    LOG_INFO_3(m_context->uuid(),
               "channel_pipeline_context::route_read_data from [%s] to [%s]:%s",
               _info->from.c_str(),
               _info->to.c_str(),
               _data->to_string().c_str());
    std::vector<std::string> _domains;
    boost::algorithm::split(_domains, _info->to, boost::algorithm::is_any_of("."));

    if (_domains.size() == 0) {
        return 0;
    }
    return m_root_domain->route_read_data(_domains, _data, _info, (int)_domains.size());
}

int channel_pipeline_context::route_write_data(std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info)
{
    AUTO_TRACE(m_context->uuid());
    ret_map ret;
    LOG_INFO_3(m_context->uuid(),
               "channel_pipeline_context::route_write_data from [%s] to [%s]:%s",
               _info->from.c_str(),
               _info->to.c_str(),
               _data->to_string().c_str());
    std::vector<std::string> _domains;
    boost::algorithm::split(_domains, _info->to, boost::algorithm::is_any_of("."));

    if (_domains.size() == 0) {
        return 0;
    }
    return m_root_domain->route_write_data(_domains, _data, _info, (int)_domains.size());
}

void channel_pipeline_context::dump()
{
    AUTO_TRACE(m_context->uuid());
    LOG_INFO_1(m_context->uuid(), "\n%s", to_string().c_str());
}

channel_pipeline_domain::channel_pipeline_domain(std::shared_ptr<channel_pipeline_domain> _parent, const std::string &_name)
    : m_parent(_parent)
    , m_domain_name(_name)
{

}

bool channel_pipeline_domain::add_pipeline(
        std::vector<std::string> _domains,
        std::shared_ptr<channel_pipeline> _pipeline)
{
    if (_domains.size() == 0) {
        auto pipeline = m_pipeline.lock();
        if (pipeline == _pipeline) {
            return false;
        }
        auto parent = m_parent.lock();
        auto full_domain_name = get_full_domain_name();
        if (pipeline.get() && parent.get()) {
            parent->notify_pipeline_removed(full_domain_name);
        }
        m_pipeline = _pipeline;
        pipeline = m_pipeline.lock();
        if (pipeline.get() && parent.get()) {
            parent->notify_pipeline_added(full_domain_name);
        }
        return true;
    }

    auto domain = _domains.front();
    _domains.erase(_domains.begin());

    // if found.
    std::shared_ptr<channel_pipeline_domain> sub;
    if (m_domain_map.find(domain) != m_domain_map.end()) {
        sub = m_domain_map[domain];
    }
    else {
        sub.reset(new channel_pipeline_domain(shared_this, domain));
        m_domain_map[domain] = sub;
    }
    return sub->add_pipeline(_domains, _pipeline);
}

bool channel_pipeline_domain::remove_pipeline(std::vector<std::string> _domains)
{
    if (_domains.size() == 0) {
        auto parent = m_parent.lock();
        auto pipeline = m_pipeline.lock();
        if (parent.get()) {
            parent->notify_pipeline_removed(get_full_domain_name());
        }
        m_pipeline.reset();
        if (domain_map_size() == 0) {
            // Tell parent erase me.
            if (parent.get()) {
                parent->erase_sub_domain(shared_this);
            }
        }
        return true;
    }
    
    auto domain = _domains.front();
    _domains.erase(_domains.begin());
    // if found.
    if (m_domain_map.find(domain) != m_domain_map.end()) {
        auto sub = m_domain_map[domain];
        bool suc = sub->remove_pipeline(_domains);
        if (domain_map_size() == 0 && !m_pipeline.lock().get()) {
            // Tell parent erase me.
            auto parent = m_parent.lock();
            if (parent.get()) {
                parent->erase_sub_domain(shared_this);
            }
        }
        return suc;
    }
    return false;
}

int channel_pipeline_domain::route_read_data(std::vector<std::string> _domains, std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info, int _depth)
{
    typedef std::map<std::string, std::shared_ptr<channel_pipeline_domain>>::iterator iterator;
    int count = 0;
    if (_domains.size() == 0) {
        if (_info->wild == router_info::wild_all) {
            for (iterator it = m_domain_map.begin(); it != m_domain_map.end(); ++it) {
                int sub_count = it->second->route_read_data(_domains, _data, _info, _depth - 1);
                count += sub_count;
            }
            
            auto pipeline = m_pipeline.lock();
            if (pipeline.get() && _depth < 0) {
                pipeline->post_handle_read_data(_data, _info, nullptr);
                count++;
            }
        }
        else if (_info->wild == router_info::wild_any) {
            auto pipeline_domain = rand_pipeline_domain();
            if (pipeline_domain.get()) {
                int sub_count = pipeline_domain->route_read_data(_domains, _data, _info, _depth - 1);
                count += sub_count;
            }
            auto pipeline = m_pipeline.lock();
            if (pipeline.get() && _depth < 0 && count == 0) {
                pipeline->post_handle_read_data(_data, _info, nullptr);
                count++;
            }
        }
        else {
            auto pipeline = m_pipeline.lock();
            if (pipeline.get() && _depth == 0) {
                pipeline->post_handle_read_data(_data, _info, nullptr);
                count++;
            }
        }
    }
    else {
        auto domain = _domains.front();
        _domains.erase(_domains.begin());
        // if found.
        if (m_domain_map.find(domain) != m_domain_map.end()) {
            auto pipeline_domain = m_domain_map[domain];
            int sub_count = pipeline_domain->route_read_data(_domains, _data, _info, _depth - 1);
            count += sub_count;
        }
    }
    return count;
}

int channel_pipeline_domain::route_write_data(std::vector<std::string> _domains, std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info, int _depth)
{
    typedef std::map<std::string, std::shared_ptr<channel_pipeline_domain>>::iterator iterator;
    int count = 0;
    if (_domains.size() == 0) {
        if (_info->wild == router_info::wild_all) {
            for (iterator it = m_domain_map.begin(); it != m_domain_map.end(); ++it) {
                int sub_count = it->second->route_write_data(_domains, _data, _info, _depth - 1);
                count += sub_count;
            }
            
            auto pipeline = m_pipeline.lock();
            if (pipeline.get() && _depth < 0) {
                pipeline->post_handle_write_data(_data, _info, nullptr);
                count++;
            }
        }
        else if (_info->wild == router_info::wild_any) {
            auto pipeline_domain = rand_pipeline_domain();
            if (pipeline_domain.get()) {
                int sub_count = pipeline_domain->route_write_data(_domains, _data, _info, _depth - 1);
                count += sub_count;
            }
            auto pipeline = m_pipeline.lock();
            if (pipeline.get() && _depth < 0 && count == 0) {
                pipeline->post_handle_write_data(_data, _info, nullptr);
                count++;
            }
        }
        else {
            auto pipeline = m_pipeline.lock();
            if (pipeline.get() && _depth == 0) {
                pipeline->post_handle_write_data(_data, _info, nullptr);
                count++;
            }
        }
    }
    else {
        auto domain = _domains.front();
        _domains.erase(_domains.begin());
        // if found.
        if (m_domain_map.find(domain) != m_domain_map.end()) {
            auto pipeline_domain = m_domain_map[domain];
            int sub_count = pipeline_domain->route_write_data(_domains, _data, _info, _depth - 1);
            count += sub_count;
        }
    }
    return count;
}

std::shared_ptr<channel_pipeline_domain> channel_pipeline_domain::rand_pipeline_domain()
{
    std::shared_ptr<channel_pipeline_domain> pipeline_domain;
    if (m_domain_map.size()) {
        int position = rand() % m_domain_map.size();
        typedef std::map<std::string, std::shared_ptr<channel_pipeline_domain>>::iterator iterator;
        int i = 0;
        for (iterator it = m_domain_map.begin();  it != m_domain_map.end(); it++, i++) {
            if (i == position) {
                pipeline_domain = it->second;
                break;
            }
        }
    }
    return pipeline_domain;
}

const std::string &channel_pipeline_domain::get_domain_name() const
{
    return m_domain_name;
}

std::string channel_pipeline_domain::get_full_domain_name() const
{
    auto parent = m_parent.lock();
    if (parent.get()) {
        auto parent_full = parent->get_full_domain_name();
        if (parent_full.size()) {
            return parent_full + "." + m_domain_name;
        }
        else {
            return m_domain_name;
        }
    }
    else {
        return "";
    }
}

std::shared_ptr<channel_pipeline> channel_pipeline_domain::pipeline() const
{
    return m_pipeline.lock();
}

std::size_t channel_pipeline_domain::domain_map_size() const
{
    return m_domain_map.size();
}

void channel_pipeline_domain::erase_sub_domain(std::shared_ptr<channel_pipeline_domain> _domain)
{
    typedef std::map<std::string, std::shared_ptr<channel_pipeline_domain>>::iterator Iterator;
    Iterator pos = m_domain_map.find(_domain->get_domain_name());
    if (pos != m_domain_map.end()) {
        m_domain_map.erase(pos);
    }
    
    if (domain_map_size() == 0 && !m_pipeline.lock().get()) {
        // Tell parent erase me.
        auto parent = m_parent.lock();
        if (parent.get()) {
            parent->erase_sub_domain(shared_this);
        }
    }
}

void channel_pipeline_domain::notify_pipeline_added(const std::string &_name)
{
    auto pipeline = m_pipeline.lock();
    if (pipeline.get()) {
        auto added = std::make_shared<pipeline_added>(_name);
        pipeline->post_handle_read_data(added, nullptr, nullptr);
    }
}

void channel_pipeline_domain::notify_pipeline_removed(const std::string &_name)
{
    auto pipeline = m_pipeline.lock();
    if (pipeline.get()) {
        auto removed = std::make_shared<pipeline_removed>(_name);
        pipeline->post_handle_read_data(removed, nullptr, nullptr);
    }
}

std::string channel_pipeline_domain::to_string(int level)
{
    std::stringstream ss;
    for (int i = 0; i < level; ++i) {
        ss<<"\t";
    }
    typedef std::map<std::string, std::shared_ptr<channel_pipeline_domain>>::iterator iterator;

    if (m_pipeline.lock().get()) {
        ss<<m_domain_name<<"(pipeline)\n";
    }
    else {
        ss<<m_domain_name<<"\n";
    }
    for (iterator it  = m_domain_map.begin();  it != m_domain_map.end(); it++) {
        auto domain = it->second;
        ss<<domain->to_string(level + 1);
    }
    return ss.str();
}

CLOSE_LOCKLESS_NAMESPACE
