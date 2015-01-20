#include "server_socket_channel.h"
#include "channel_handler.h"
#include "channel_data.h"
#include "channel_pipeline_initializer.h"
#include "channel_pipeline.h"

OPEN_LOCKLESS_NAMESPACE

#define BUF_SIZE 256

server_socket_channel::server_socket_channel(
        std::shared_ptr<asio::io_service> _service,
        std::shared_ptr<asio::ip::tcp::socket> _socket,
        asio::ip::tcp::endpoint _remote_ep,
        std::shared_ptr<channel_pipeline_initializer> _initializer)
    : m_context(new runtime_context(_service))
    , m_socket(_socket)
    , m_handler_initializer(_initializer)
    , m_pipeline(new channel_pipeline(m_context))
    , m_remote_endpoint(_remote_ep)
    , m_finished(false)
{
    AUTO_TRACE(m_context->uuid());
}

std::shared_ptr<channel_pipeline> server_socket_channel::get_pipeline() const
{
    return m_pipeline;
}

void server_socket_channel::ps_pipeline_finish(std::shared_ptr<object> /*_this*/)
{
    AUTO_TRACE(m_context->uuid());
    if (!m_finished) {
        m_socket->close();
        ps_finish.publish<std::shared_ptr<server_socket_channel>>(shared_this);
        m_finished = true;
    }
}

void server_socket_channel::ps_pipeline_data(std::shared_ptr<object> /*_this*/, std::shared_ptr<channel_data> _data)
{
    AUTO_TRACE(m_context->uuid());
    if (_data->get_type() == lockless_data::socket_stream) {
        auto real = channel_data::convert<socket_stream>(_data);
        write(real->get_payload());
    }
}

void server_socket_channel::start()
{
    AUTO_TRACE(m_context->uuid());
    m_handler_initializer->init_channel(m_pipeline);

    m_pipeline->ps_write_data.subscribe(
                shared_this,
                m_context,
                std::bind(&server_socket_channel::ps_pipeline_data, this, std::placeholders::_1, std::placeholders::_2));
    m_pipeline->ps_finish.subscribe(
                shared_this,
                m_context,
                std::bind(&server_socket_channel::ps_pipeline_finish, this, std::placeholders::_1));

    auto data = std::make_shared<connect_status>(connect_status::connected,
                                                 m_remote_endpoint.address().to_string(),
                                                 m_remote_endpoint.port());
    m_pipeline->post_handle_read_data(data, std::shared_ptr<router_info>(), nullptr);

    read();
}

void server_socket_channel::write(std::shared_ptr<std::string> _buffer)
{
    AUTO_TRACE(m_context->uuid());
    m_socket->async_write_some(
                asio::buffer(*_buffer),
                std::bind(&server_socket_channel::on_write, this, std::placeholders::_1, std::placeholders::_2));
}

void server_socket_channel::read()
{
    AUTO_TRACE(m_context->uuid());
    auto buffer = std::make_shared<std::vector<char>>();
    buffer->resize(BUF_SIZE);
    m_socket->async_read_some(
                asio::buffer(*buffer),
                std::bind(&server_socket_channel::on_read, this, buffer, std::placeholders::_1, std::placeholders::_2));
}

void server_socket_channel::on_read(
        std::shared_ptr<std::vector<char>> _buffer,
        const asio::error_code& _error,
        std::size_t _bytes)
{
    AUTO_TRACE(m_context->uuid());
    if (_error) {
        LOG_ERROR_2(m_context->uuid(),
                    "server_socket_channel::on_read error_code:%d error_message:%s",
                    _error.value(),
                    _error.message().c_str());

        if (!m_finished) {
            auto data = std::make_shared<connect_status>(connect_status::read_error,
                                                         m_remote_endpoint.address().to_string(),
                                                         m_remote_endpoint.port());
            m_pipeline->post_handle_read_data(data, std::shared_ptr<router_info>(), nullptr);
        }
    }
    else {
        LOG_INFO_1(m_context->uuid(), "server_socket_channel::on_read bytes:%lu", _bytes);
        m_total_read += _bytes;
        auto payload = std::make_shared<std::string>();
        payload->append(_buffer->begin(), _buffer->begin() + _bytes);

        auto data = std::make_shared<socket_stream>(payload);
        m_pipeline->post_handle_read_data(data, std::shared_ptr<router_info>(), nullptr);
        read();
    }
}

void server_socket_channel::on_write(
        const asio::error_code& _error,
        std::size_t _bytes)
{
    AUTO_TRACE(m_context->uuid());
    if (_error) {
        LOG_ERROR_2(m_context->uuid(),
                    "server_socket_channel::on_write error_code:%d error_message:%s",
                    _error.value(),
                    _error.message().c_str());
        if (!m_finished) {
            auto data = std::make_shared<connect_status>(connect_status::write_error,
                                                         m_remote_endpoint.address().to_string(),
                                                         m_remote_endpoint.port());
            m_pipeline->post_handle_read_data(data, std::shared_ptr<router_info>(), nullptr);
        }
    }
    else {
        LOG_INFO_1(m_context->uuid(), "server_socket_channel::on_write bytes:%lu", _bytes);
        m_total_write += _bytes;
    }
}

CLOSE_LOCKLESS_NAMESPACE
