#ifndef CHANNEL_PIPELINE_H
#define CHANNEL_PIPELINE_H

#include <map>

#include "defines.h"
#include "object.h"
#include "runtime_context.h"
#include "publish_subscribe.h"
#include <boost/any.hpp>

OPEN_LOCKLESS_NAMESPACE

class router_info;
class channel_data;
class channel_handler;
class channel_handler_context;
class channel_pipeline_context;

class channel_pipeline : public object
{
public:
    ps_connect<std::function<void(std::shared_ptr<object>)>> ps_finish;
    ps_connect<std::function<void(std::shared_ptr<object>, std::shared_ptr<channel_data>)>> ps_write_data;
    ps_connect<std::function<void(std::shared_ptr<object>, std::shared_ptr<channel_data>)>> ps_read_data;
public:
    POST_CONTEXT(m_runtime_context)
    
    POST_FUNCTION_3_FUTURE(channel_pipeline, add_channel_handler,
                           std::shared_ptr<channel_handler>, _handler,
                           std::string, _name,
                           bool, _standalone,
                           ret_map)
    POST_FUNCTION_0_FUTURE(channel_pipeline, finish,
                           void)
    
    POST_FUNCTION_2_FUTURE(channel_pipeline, add_to_context,
                           std::shared_ptr<channel_pipeline_context>, _context,
                           std::string, _name,
                           ret_map)
    POST_FUNCTION_1_FUTURE(channel_pipeline, rename,
                           std::string, _name,
                           ret_map)
    POST_FUNCTION_2_FUTURE(channel_pipeline, handle_read_data,
                           std::shared_ptr<channel_data>, _data,
                           std::shared_ptr<router_info>, _info,
                           void)
    POST_FUNCTION_2_FUTURE(channel_pipeline, handle_write_data,
                           std::shared_ptr<channel_data>, _data,
                           std::shared_ptr<router_info>, _info,
                           void)
    POST_FUNCTION_1_FUTURE(channel_pipeline, publish_write_data,
                           std::shared_ptr<channel_data>, _data,
                           void)
    POST_FUNCTION_1_FUTURE(channel_pipeline, publish_read_data,
                           std::shared_ptr<channel_data>, _data,
                           void)
    POST_FUNCTION_3_FUTURE(channel_pipeline, handle_route_read_data,
                           std::shared_ptr<channel_data>, _data,
                           std::string, _to,
                           std::string, _wild,
                           void)
    POST_FUNCTION_3_FUTURE(channel_pipeline, handle_route_write_data,
                           std::shared_ptr<channel_data>, _data,
                           std::string, _to,
                           std::string, _wild,
                           void)
public:
    channel_pipeline(std::shared_ptr<runtime_context> _context);
    ~channel_pipeline();
public:
    const std::string &get_name() const;
protected:
    void set_name(const std::string &_name);
private:
    
    /**
     *
     * @return. ret_map
     * context  =>  std::shared_ptr<channel_pipeline_context>
     * name     =>  std::string
     * pipeline =>  std::shared_ptr<channel_pipeline>
     */
    ret_map add_to_context(std::shared_ptr<channel_pipeline_context> _context, std::string _name);
    
    /**
     * @return. ret_map
     * old_name =>  std::string
     * new_name =>  std::string
     */
    ret_map rename(std::string _new_name);
    void handle_read_data(std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info);
    void handle_write_data(std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info);
    void publish_write_data(std::shared_ptr<channel_data> _data);
    void publish_read_data(std::shared_ptr<channel_data> _data);
    void handle_route_read_data(std::shared_ptr<channel_data> _data, std::string _to, std::string _wild);
    void handle_route_write_data(std::shared_ptr<channel_data> _data, std::string _to, std::string _wild);
    
    /**
     * @return. ret_map
     * context => std::shared_ptr<channel_handler_context>
     */
    ret_map add_channel_handler(std::shared_ptr<channel_handler> _handler, std::string _name, bool _standalone);
    void finish();
private:
    std::shared_ptr<runtime_context> m_runtime_context;
    std::shared_ptr<channel_handler_context> m_head_context;
    std::shared_ptr<channel_handler_context> m_tail_context;
    std::weak_ptr<channel_pipeline_context> m_pipeline_context;
    std::string m_name;
};

CLOSE_LOCKLESS_NAMESPACE

#endif // CHANNEL_PIPELINE_H
