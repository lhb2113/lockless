#ifndef SERVER_SOCKET_CHANNEL_H
#define SERVER_SOCKET_CHANNEL_H

#include "defines.h"
#include "object.h"
#include "runtime_context.h"
#include "publish_subscribe.h"

OPEN_LOCKLESS_NAMESPACE

class channel_pipeline_initializer;
class channel_pipeline;
class channel_handler;
class runtime_context;
class channel_data;

class server_socket_channel : public object
{
public:
    ps_connect<std::function<void(std::shared_ptr<object>, std::shared_ptr<server_socket_channel>)>> ps_finish;
public:
    server_socket_channel(
            std::shared_ptr<asio::io_service> _service,
            std::shared_ptr<asio::ip::tcp::socket> _socket,
            asio::ip::tcp::endpoint _remote_ep,
            std::shared_ptr<channel_pipeline_initializer> _initializer);
public:
    POST_CONTEXT(m_context)
    POST_FUNCTION_0_FUTURE(server_socket_channel, start,
                           void)
public:
    std::shared_ptr<channel_pipeline> get_pipeline() const;
private:
    void ps_pipeline_finish(std::shared_ptr<object> _this);
    void ps_pipeline_data(std::shared_ptr<object> _this, std::shared_ptr<channel_data> _data);
private:
    void start();

    void write(std::shared_ptr<std::string> _buffer);
    void read();

    void on_read(std::shared_ptr<std::vector<char>> _buffer,
                const asio::error_code& _error,
                std::size_t _bytes);
    void on_write(const asio::error_code& _error,
                 std::size_t _bytes);
private:
    std::shared_ptr<runtime_context> m_context;
    std::shared_ptr<asio::ip::tcp::socket> m_socket;
    std::shared_ptr<channel_pipeline_initializer> m_handler_initializer;
    std::shared_ptr<channel_pipeline> m_pipeline;
    asio::ip::tcp::endpoint m_remote_endpoint;
    bool m_finished;

    long m_total_write;
    long m_total_read;
};

CLOSE_LOCKLESS_NAMESPACE

#endif // SERVER_SOCKET_CHANNEL_H
