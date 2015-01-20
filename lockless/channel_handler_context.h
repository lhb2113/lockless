#ifndef CHANNEL_HANDLER_CONTEXT_H
#define CHANNEL_HANDLER_CONTEXT_H

#include "defines.h"
#include "object.h"
#include "runtime_context.h"

OPEN_LOCKLESS_NAMESPACE

class channel_handler;
class channel_pipeline;
class channel_data;
class router_info;

class channel_handler_context : public object
{
public:
    POST_CONTEXT(m_context)
public:
    channel_handler_context(
            std::string _name,
            std::shared_ptr<runtime_context> _context,
            std::shared_ptr<channel_handler> _handler,
            std::shared_ptr<channel_pipeline> _pipeline);
    ~channel_handler_context();
public:
    void next_read_data(std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info);
    void next_write_data(std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info);
    void next_read_data(std::shared_ptr<channel_data> _data);
    void next_write_data(std::shared_ptr<channel_data> _data);
    void route_read_data(std::shared_ptr<channel_data> _data, const std::string &_to, const std::string &_wild);
    void route_write_data(std::shared_ptr<channel_data> _data, const std::string &_to, const std::string &_wild);
    void rename(const std::string &_new_name);
    void finish();
public:
    const std::string &get_name() const;
    std::string get_pipeline_name() const;
    std::shared_ptr<runtime_context> get_runtime_context() const;
    std::shared_ptr<channel_handler> get_channel_handler() const;
    std::shared_ptr<channel_pipeline> get_channel_pipeline() const;
private:
    POST_FUNCTION_2_FUTURE(channel_handler_context, handle_read_data,
                           std::shared_ptr<channel_data>, _data,
                           std::shared_ptr<router_info>, _info,
                           void)
    POST_FUNCTION_2_FUTURE(channel_handler_context, handle_write_data,
                           std::shared_ptr<channel_data>, _data,
                           std::shared_ptr<router_info>, _info,
                           void)
    SYNC_FUNCTION_2(handle_read_data, std::shared_ptr<channel_data>, _data, std::shared_ptr<router_info>, _info)
    SYNC_FUNCTION_2(handle_write_data, std::shared_ptr<channel_data>, _data, std::shared_ptr<router_info>, _info)
private:
    void set_next(std::shared_ptr<channel_handler_context> _context);
    void set_prev(std::shared_ptr<channel_handler_context> _context);
    void handle_read_data(std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info);
    void handle_write_data(std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info);
private:
    std::string m_name;
    std::shared_ptr<runtime_context> m_context;
    std::shared_ptr<channel_handler> m_handler;
    std::weak_ptr<channel_pipeline> m_pipeline;

    std::shared_ptr<channel_handler_context> m_next;
    std::weak_ptr<channel_handler_context> m_prev;

    friend class channel_pipeline;
};

CLOSE_LOCKLESS_NAMESPACE

#endif // CHANNEL_HANDLER_CONTEXT_H
