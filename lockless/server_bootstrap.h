#ifndef SERVER_BOOTSTRAP_H
#define SERVER_BOOTSTRAP_H

#include <boost/uuid/uuid.hpp>
#include <asio.hpp>

#include "object.h"
#include "defines.h"

OPEN_LOCKLESS_NAMESPACE

class runtime_context;
class channel_pipeline_initializer_factory;
class channel_pipeline_initializer_factory;
class server_socket_channel;
class executor;
class channel_pipeline;

class server_bootstrap : public object
{
public:
    server_bootstrap(
            std::shared_ptr<channel_pipeline_initializer_factory> _bootstrap_factory,
            std::shared_ptr<channel_pipeline_initializer_factory> _channel_factory);
    ~server_bootstrap();
public:
    POST_CONTEXT(m_context)
    void start_accept(asio::ip::tcp::endpoint _ep);
    void synchronize();
    void shutdown();
private:
    void ps_main_executor_started(std::shared_ptr<object> _this);
    void ps_pool_executor_started(std::shared_ptr<object> _this, int _position);
    void ps_channel_finished(std::shared_ptr<object> _this, std::shared_ptr<server_socket_channel> _channel);
private:
    void do_start_accept(asio::ip::tcp::endpoint _ep);
    void do_accept();
private:
    void on_accept(
            asio::error_code _error,
            std::shared_ptr<asio::ip::tcp::socket> _socket,
            std::shared_ptr<asio::io_service> _service);
private:
    std::shared_ptr<executor> m_main_executor;
    std::shared_ptr<executor> m_pool_executor;
    std::shared_ptr<runtime_context> m_context;
    std::shared_ptr<channel_pipeline_initializer_factory> m_bootstrap_factory;
    std::shared_ptr<channel_pipeline_initializer_factory> m_channel_factory;

    std::shared_ptr<asio::ip::tcp::acceptor> m_acceptor;
    std::map<boost::uuids::uuid, std::shared_ptr<server_socket_channel>> m_channels;

    std::shared_ptr<channel_pipeline> m_pipeline;

    std::function<void()> m_executor_started;
};

CLOSE_LOCKLESS_NAMESPACE

#endif // SERVER_BOOTSTRAP_H
