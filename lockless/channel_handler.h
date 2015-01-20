#ifndef CHANNEL_HANDLER_H
#define CHANNEL_HANDLER_H

#include <map>
#include <functional>

#include "defines.h"
#include "object.h"
#include "data_type.h"

OPEN_LOCKLESS_NAMESPACE

class channel_handler;
class channel_handler_context;
class channel_pipeline;
class channel_pipeline_initializer;
class channel_data;
class router_info;

class channel_handler : public object
{
public:
    channel_handler();
    ~channel_handler();
public:
    void set_handler_context(std::shared_ptr<channel_handler_context> _context);
    std::shared_ptr<channel_handler_context> get_handler_context() const;

    bool can_handle_read(data_type _type);
    void set_read_handler(data_type _type, std::function<void(std::shared_ptr<channel_data>, std::shared_ptr<router_info>)> _handler);

    bool can_handle_write(data_type _type);
    void set_write_handler(data_type _type, std::function<void(std::shared_ptr<channel_data>, std::shared_ptr<router_info>)> _handler);
private:
    void read_data(std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info);
    void write_data(std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info);
protected:
    virtual void handler_context_registered();
private:
    std::weak_ptr<channel_handler_context> m_handler_context;
    std::map<data_type, std::function<void(std::shared_ptr<channel_data>, std::shared_ptr<router_info>)>> m_read_handlers;
    std::map<data_type, std::function<void(std::shared_ptr<channel_data>, std::shared_ptr<router_info>)>> m_write_handlers;

    friend class channel_handler_context;
};

#define SET_READ_HANDLER(Type, Handler) \
set_read_handler(Type, std::bind(&Handler, this, std::placeholders::_1, std::placeholders::_2))

#define SET_WRITE_HANDLER(Type, Handler) \
set_write_handler(Type, std::bind(&Handler, this, std::placeholders::_1, std::placeholders::_2))

CLOSE_LOCKLESS_NAMESPACE

#endif // CHANNEL_HANDLER_H
