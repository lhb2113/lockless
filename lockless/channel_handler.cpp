#include "channel_handler.h"
#include "channel_data.h"
#include "channel_handler_context.h"
#include "channel_pipeline.h"

OPEN_LOCKLESS_NAMESPACE

channel_handler::channel_handler()
{
}

channel_handler::~channel_handler()
{

}

void channel_handler::set_handler_context(std::shared_ptr<channel_handler_context> _context)
{
    m_handler_context = _context;
    handler_context_registered();
}

std::shared_ptr<channel_handler_context> channel_handler::get_handler_context() const
{
    return m_handler_context.lock();
}

bool channel_handler::can_handle_read(data_type _type)
{
    return m_read_handlers.end() != m_read_handlers.find(_type);
}

void channel_handler::set_read_handler(data_type _type, std::function<void(std::shared_ptr<channel_data>, std::shared_ptr<router_info>)> _handler)
{
    m_read_handlers[_type] = _handler;
}

bool channel_handler::can_handle_write(data_type _type)
{
    return m_write_handlers.end() != m_write_handlers.find(_type);
}

void channel_handler::set_write_handler(data_type _type, std::function<void(std::shared_ptr<channel_data>, std::shared_ptr<router_info>)> _handler)
{
    m_write_handlers[_type] = _handler;
}

void channel_handler::read_data(std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info)
{
    AUTO_TRACE(get_handler_context()->context()->uuid());
    if (can_handle_read(_data->get_type())) {
        m_read_handlers[_data->get_type()](_data, _info);
    }
    else {
        get_handler_context()->next_read_data(_data, _info);
    }
}

void channel_handler::write_data(std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info)
{
    AUTO_TRACE(get_handler_context()->context()->uuid());
    if (can_handle_write(_data->get_type())) {
        m_write_handlers[_data->get_type()](_data, _info);
    }
    else {
        get_handler_context()->next_write_data(_data, _info);
    }
}

void channel_handler::handler_context_registered()
{

}

CLOSE_LOCKLESS_NAMESPACE
