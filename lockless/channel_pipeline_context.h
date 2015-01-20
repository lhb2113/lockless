#ifndef CHANNEL_PIPELINE_CONTEXT_H
#define CHANNEL_PIPELINE_CONTEXT_H

#include "defines.h"
#include "object.h"
#include "runtime_context.h"
#include "publish_subscribe.h"

OPEN_LOCKLESS_NAMESPACE

class channel_pipeline;
class executor;
class channel_pipeline_domain;
class channel_data;
class router_info;

class channel_pipeline_context : public object
{
public:
    ps_connect<std::function<void(std::shared_ptr<object>)>> ps_started;
    ps_connect<std::function<void(std::shared_ptr<object>, std::shared_ptr<channel_pipeline>)>> ps_pipeline_added;
    ps_connect<std::function<void(std::shared_ptr<object>, std::string)>> ps_pipeline_removed;
public:
    POST_CONTEXT(m_context)
    POST_FUNCTION_0_FUTURE(channel_pipeline_context, dump,
                           void)
    POST_FUNCTION_1_FUTURE(channel_pipeline_context, add_pipeline,
                           std::shared_ptr<channel_pipeline>, _pipeline,
                           bool)
    POST_FUNCTION_1_FUTURE(channel_pipeline_context, remove_pipeline,
                           std::string, _domain_name,
                           bool)
    POST_FUNCTION_2_FUTURE(channel_pipeline_context, route_read_data,
                           std::shared_ptr<channel_data>, _data,
                           std::shared_ptr<router_info>, _info,
                           int)
    POST_FUNCTION_2_FUTURE(channel_pipeline_context, route_write_data,
                           std::shared_ptr<channel_data>, _data,
                           std::shared_ptr<router_info>, _info,
                           int)
public:
    channel_pipeline_context();
public:
    void synchronize();
    void startup();
    void shutdown();
    std::string to_string();
private:
    bool add_pipeline(std::shared_ptr<channel_pipeline> _pipeline);
    bool remove_pipeline(std::string _domain_name);
    int route_read_data(std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info);
    int route_write_data(std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info);
    void dump();
private:
    std::shared_ptr<executor> m_main_executor;
    std::shared_ptr<runtime_context> m_context;
    std::shared_ptr<channel_pipeline_domain> m_root_domain;
};

class channel_pipeline_domain : public object
{
public:
    channel_pipeline_domain(std::shared_ptr<channel_pipeline_domain> _parent, const std::string &_name);
public:
    bool add_pipeline(
            std::vector<std::string> _domains,
            std::shared_ptr<channel_pipeline> _pipeline);
    bool remove_pipeline(std::vector<std::string> _domains);
    int route_read_data(std::vector<std::string> _domains, std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info, int _depth);
    int route_write_data(std::vector<std::string> _domains, std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info, int _depth);
    
    std::shared_ptr<channel_pipeline_domain> rand_pipeline_domain();
    const std::string &get_domain_name() const;
    std::string get_full_domain_name() const;
    std::shared_ptr<channel_pipeline> pipeline() const;
    std::size_t domain_map_size() const;
    void erase_sub_domain(std::shared_ptr<channel_pipeline_domain> _domain);
    void notify_pipeline_added(const std::string &_name);
    void notify_pipeline_removed(const std::string &_name);
public:
    std::string to_string(int level);
private:
    std::string m_domain_name;
    std::weak_ptr<channel_pipeline_domain> m_parent;
    std::map<std::string, std::shared_ptr<channel_pipeline_domain>> m_domain_map;
    std::weak_ptr<channel_pipeline> m_pipeline;
};

CLOSE_LOCKLESS_NAMESPACE

#endif // CHANNEL_PIPELINE_CONTEXT_H
