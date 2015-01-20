#include "channel_handler_context.h"
#include "channel_data.h"
#include "channel_handler.h"
#include "channel_pipeline.h"

OPEN_LOCKLESS_NAMESPACE

channel_handler_context::channel_handler_context(
        std::string _name,
        std::shared_ptr<runtime_context> _context,
        std::shared_ptr<channel_handler> _handler,
        std::shared_ptr<channel_pipeline> _pipeline)
    : m_name(_name)
    , m_context(_context)
    , m_handler(_handler)
    , m_pipeline(_pipeline)
{
    AUTO_TRACE(m_context->uuid());
}

channel_handler_context::~channel_handler_context()
{
    AUTO_TRACE(m_context->uuid());
}

void channel_handler_context::next_read_data(std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info)
{
    AUTO_TRACE(m_context->uuid());
    auto pipeline = m_pipeline.lock();
    if (pipeline.get()) {
        LOG_DEBUG_3(m_context->uuid(), "[%s]<%s> next_read_data:%s",
                   pipeline->get_name().c_str(),
                   get_name().c_str(),
                   _data->to_string().c_str());
    }
    else {
        LOG_DEBUG_2(m_context->uuid(), "<%s> next_read_data:%s",
                   get_name().c_str(),
                   _data->to_string().c_str());
    }

    if (m_next.get() && m_next->context() == context()) {
        m_next->sync_handle_read_data(_data, _info);
    }
    else if (m_next.get()) {
        m_next->post_handle_read_data(_data, _info, nullptr);
    }
    else {
        auto pipeline = m_pipeline.lock();
        if (pipeline.get()) {
            pipeline->post_publish_read_data(_data, nullptr);
        }
    }
}

void channel_handler_context::next_write_data(std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info)
{
    AUTO_TRACE(m_context->uuid());
    auto pipeline = m_pipeline.lock();
    if (pipeline.get()) {
        LOG_DEBUG_3(m_context->uuid(), "[%s]<%s> next_write_data:%s",
                   pipeline->get_name().c_str(),
                   get_name().c_str(),
                   _data->to_string().c_str());
    }
    else {
        LOG_DEBUG_2(m_context->uuid(), "<%s> next_write_data:%s",
                   get_name().c_str(),
                   _data->to_string().c_str());
    }

    auto prev = m_prev.lock();
    if (prev.get() && prev->context() == context()) {
        prev->sync_handle_write_data(_data, _info);
    }
    else if (prev.get()) {
        prev->post_handle_write_data(_data, _info, nullptr);
    }
    else {
        auto pipeline = m_pipeline.lock();
        if (pipeline.get()) {
            pipeline->post_publish_write_data(_data, nullptr);
        }
    }
}


void channel_handler_context::next_read_data(std::shared_ptr<channel_data> _data)
{
    next_read_data(_data, std::shared_ptr<router_info>());
}

void channel_handler_context::next_write_data(std::shared_ptr<channel_data> _data)
{
    next_write_data(_data, std::shared_ptr<router_info>());
}

void channel_handler_context::route_read_data(std::shared_ptr<channel_data> _data, const std::string &_to, const std::string &_wild)
{
    AUTO_TRACE(m_context->uuid());

    auto pipeline = m_pipeline.lock();
    if (pipeline.get()) {
        LOG_DEBUG_4(m_context->uuid(), "[%s]<%s> route_read_data:%s to %s",
                   pipeline->get_name().c_str(),
                   get_name().c_str(),
                   _data->to_string().c_str(),
                   _to.c_str());
    }
    else {
        LOG_DEBUG_3(m_context->uuid(), "<%s> route_read_data:%s to %s",
                   get_name().c_str(),
                   _data->to_string().c_str(),
                   _to.c_str());
    }

    if (pipeline.get()) {
        pipeline->post_handle_route_read_data(_data, _to, _wild, nullptr);
    }
}

void channel_handler_context::route_write_data(std::shared_ptr<channel_data> _data, const std::string &_to, const std::string &_wild)
{
    AUTO_TRACE(m_context->uuid());

    auto pipeline = m_pipeline.lock();
    if (pipeline.get()) {
        LOG_DEBUG_4(m_context->uuid(), "[%s]<%s> route_write_data to %s:%s",
                   pipeline->get_name().c_str(),
                    get_name().c_str(),
                    _to.c_str(),
                   _data->to_string().c_str());
    }
    else {
        LOG_DEBUG_3(m_context->uuid(), "<%s> route_write_data to %s:%s",
                    get_name().c_str(),
                    _to.c_str(),
                   _data->to_string().c_str());
    }

    if (pipeline.get()) {
        pipeline->post_handle_route_write_data(_data, _to, _wild, nullptr);
    }
}

void channel_handler_context::rename(const std::string &_new_name)
{
    AUTO_TRACE(m_context->uuid());

    auto pipeline = m_pipeline.lock();
    if (pipeline.get()) {
        pipeline->post_rename(_new_name, nullptr);
    }
}

std::string channel_handler_context::get_pipeline_name() const
{
    AUTO_TRACE(m_context->uuid());
    
    auto pipeline = m_pipeline.lock();
    if (pipeline.get()) {
        return pipeline->get_name();
    }
    return "";
}


std::shared_ptr<runtime_context> channel_handler_context::get_runtime_context() const
{
    return m_context;
}

std::shared_ptr<channel_handler> channel_handler_context::get_channel_handler() const
{
    return m_handler;
}

std::shared_ptr<channel_pipeline> channel_handler_context::get_channel_pipeline() const
{
    return m_pipeline.lock();
}

void channel_handler_context::finish()
{
    AUTO_TRACE(m_context->uuid());
    auto pipeline = m_pipeline.lock();
    if (pipeline.get()) {
        pipeline->post_finish(nullptr);
    }
}

const std::string &channel_handler_context::get_name() const
{
    return m_name;
}

void channel_handler_context::set_next(std::shared_ptr<channel_handler_context> _context)
{
    m_next = _context;
}

void channel_handler_context::set_prev(std::shared_ptr<channel_handler_context> _context)
{
    m_prev = _context;
}

void channel_handler_context::handle_read_data(std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info)
{
    AUTO_TRACE(m_context->uuid());
    auto pipeline = m_pipeline.lock();
    if (pipeline.get()) {
        LOG_DEBUG_3(m_context->uuid(), "[%s]<%s> handle_read_data:%s",
                   pipeline->get_name().c_str(),
                   get_name().c_str(),
                   _data->to_string().c_str());
    }
    else {
        LOG_DEBUG_2(m_context->uuid(), "<%s> handle_read_data:%s",
                   get_name().c_str(),
                   _data->to_string().c_str());
    }
    m_handler->read_data(_data, _info);
}

void channel_handler_context::handle_write_data(std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info)
{
    AUTO_TRACE(m_context->uuid());
    auto pipeline = m_pipeline.lock();
    m_handler->write_data(_data, _info);
}

CLOSE_LOCKLESS_NAMESPACE
