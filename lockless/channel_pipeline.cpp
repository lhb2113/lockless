#include "channel_pipeline.h"
#include "channel_pipeline_context.h"
#include "channel_data.h"
#include "channel_handler.h"
#include "channel_handler_context.h"


#include <future>

OPEN_LOCKLESS_NAMESPACE

channel_pipeline::channel_pipeline(std::shared_ptr<runtime_context> _context)
    : m_runtime_context(_context)
{
    AUTO_TRACE(m_runtime_context->uuid());
}

channel_pipeline::~channel_pipeline()
{
    AUTO_TRACE(m_runtime_context->uuid());
    auto context = m_pipeline_context.lock();
    if (context.get()) {
        context->post_remove_pipeline(get_name(), nullptr);
    }
}

const std::string &channel_pipeline::get_name() const
{
    return m_name;
}

void channel_pipeline::set_name(const std::string &_name)
{
    m_name = _name;
}

ret_map channel_pipeline::add_to_context(std::shared_ptr<channel_pipeline_context> _context, std::string _name)
{
    AUTO_TRACE(m_runtime_context->uuid());
    ret_map ret;
    ret["context"] = _context;
    m_pipeline_context = _context;
    if (_context.get()) {
        set_name(_name);
        ret["name"] = get_name();
        ret["pipeline"] = shared_this;
        _context->post_add_pipeline(shared_this, nullptr);
    }
    return ret;
}

ret_map channel_pipeline::rename(std::string _new_name)
{
    AUTO_TRACE(m_runtime_context->uuid());
    ret_map ret;
    auto context = m_pipeline_context.lock();
    if (context.get()) {
        context->post_remove_pipeline(get_name(), nullptr);
        ret["old_name"] = get_name();
        set_name(_new_name);
        ret["new_name"] = get_name();
        context->post_add_pipeline(shared_this, nullptr);
    }
    return ret;
}

void channel_pipeline::handle_read_data(std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info)
{
    AUTO_TRACE(m_runtime_context->uuid());
    if (_info.get()) {
        LOG_INFO_3(m_runtime_context->uuid(),
                    "[%s] handle_read_data from:%s data:%s",
                    get_name().c_str(),
                    _info->from.c_str(),
                    _data->to_string().c_str());
    }
    else {
        LOG_INFO_2(m_runtime_context->uuid(),
                    "[%s]<pipeline> handle_read_data:%s",
                    get_name().c_str(),
                    _data->to_string().c_str());
    }
    if (m_head_context.get()) {
        m_head_context->post_handle_read_data(_data, _info, nullptr);
    }
}

void channel_pipeline::handle_write_data(std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info)
{
    AUTO_TRACE(m_runtime_context->uuid());
    if (_info.get()) {
        LOG_INFO_3(m_runtime_context->uuid(),
                    "[%s] handle_write_data from:%s data:%s",
                    get_name().c_str(),
                    _info->from.c_str(),
                    _data->to_string().c_str());
    }
    else {
        LOG_INFO_2(m_runtime_context->uuid(),
                    "[%s]<pipeline> handle_write_data:%s",
                    get_name().c_str(),
                    _data->to_string().c_str());
    }
    if (m_tail_context.get()) {
        m_tail_context->post_handle_write_data(_data, _info, nullptr);
    }
}

void channel_pipeline::publish_write_data(std::shared_ptr<channel_data> _data)
{
    AUTO_TRACE(m_runtime_context->uuid());
    LOG_INFO_2(m_runtime_context->uuid(), "[%s] publish_write_data:%s",
               get_name().c_str(),
               _data->to_string().c_str());
    ps_write_data.publish<std::shared_ptr<channel_data>>(_data);
}

void channel_pipeline::publish_read_data(std::shared_ptr<channel_data> _data)
{
    AUTO_TRACE(m_runtime_context->uuid());
    LOG_INFO_2(m_runtime_context->uuid(), "[%s] publish_read_data:%s",
               get_name().c_str(),
               _data->to_string().c_str());
    ps_read_data.publish<std::shared_ptr<channel_data>>(_data);
}

void channel_pipeline::handle_route_read_data(std::shared_ptr<channel_data> _data, std::string _to, std::string _wild)
{
    AUTO_TRACE(m_runtime_context->uuid());
    LOG_INFO_3(m_runtime_context->uuid(), "[%s] handle_route_read_data to %s:%s",
               get_name().c_str(),
               _to.c_str(),
               _data->to_string().c_str());
    auto pipeline_context = m_pipeline_context.lock();
    if (pipeline_context.get()) {
        auto info = std::make_shared<router_info>();
        info->from = get_name();
        info->to = _to;
        info->wild = _wild;
        pipeline_context->post_route_read_data(_data, info, nullptr);
    }
}

void channel_pipeline::handle_route_write_data(std::shared_ptr<channel_data> _data, std::string _to, std::string _wild)
{
    AUTO_TRACE(m_runtime_context->uuid());
    LOG_INFO_3(m_runtime_context->uuid(), "[%s] handle_route_write_data to %s:%s",
               get_name().c_str(),
               _to.c_str(),
               _data->to_string().c_str());
    auto pipeline_context = m_pipeline_context.lock();
    if (pipeline_context.get()) {
        auto info = std::make_shared<router_info>();
        info->from = get_name();
        info->to = _to;
        info->wild = _wild;
        pipeline_context->post_route_write_data(_data, info, nullptr);
    }
}

ret_map channel_pipeline::add_channel_handler(std::shared_ptr<channel_handler> _handler, std::string _name, bool _standalone)
{
    AUTO_TRACE(m_runtime_context->uuid());
    ret_map ret;
    std::shared_ptr<runtime_context> context;
    std::shared_ptr<channel_handler_context> handler_context;
    if (_standalone) {
        context = std::make_shared<runtime_context>(m_runtime_context->get_io_service());
    }
    else {
        context = m_runtime_context;
    }
    handler_context = std::make_shared<channel_handler_context>(_name, context, _handler, shared_this);
    ret["context"] = handler_context;
    _handler->set_handler_context(handler_context);
    if (!m_head_context.get()) {
        m_head_context = handler_context;
        m_tail_context = handler_context;
    }
    else {
        m_tail_context->set_next(handler_context);
        handler_context->set_prev(m_tail_context);
        m_tail_context = handler_context;
    }
    return ret;
}

void channel_pipeline::finish()
{
    AUTO_TRACE(m_runtime_context->uuid());
    ps_finish.publish();
}

CLOSE_LOCKLESS_NAMESPACE
