#ifndef CLIENT_BOOTSTRAP_H
#define CLIENT_BOOTSTRAP_H

#include <asio.hpp>
#include "publish_subscribe.h"

#include "defines.h"
#include "object.h"

OPEN_LOCKLESS_NAMESPACE

class executor;
class client_socket_channel;
class runtime_context;
class channel_pipeline_initializer_factory;

class client_bootstrap : public object
{
public:
    ps_connect<std::function<void(std::shared_ptr<object>, std::shared_ptr<client_bootstrap>)>> ps_finish;
public:
    client_bootstrap(std::shared_ptr<channel_pipeline_initializer_factory> _factory);
    ~client_bootstrap();
public:
    // functional
    void connect(asio::ip::tcp::endpoint _ep);
    void disconnect();
private:
    void ps_executor_started(std::shared_ptr<object> _this, int _index);
    void ps_channel_finished(std::shared_ptr<object> _this, std::shared_ptr<client_socket_channel> _channel);
private:
    std::shared_ptr<executor> m_executor;
    std::shared_ptr<runtime_context> m_context;
    std::shared_ptr<channel_pipeline_initializer_factory> m_initializer_factory;

    std::shared_ptr<client_socket_channel> m_channel;
    
    std::shared_ptr<client_bootstrap> m_this;
};

CLOSE_LOCKLESS_NAMESPACE

#endif // CLIENT_BOOTSTRAP_H
