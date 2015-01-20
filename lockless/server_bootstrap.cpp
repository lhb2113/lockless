
#include <boost/uuid/random_generator.hpp>

#include "server_bootstrap.h"
#include "logger.h"
#include "channel_handler.h"
#include "channel_pipeline_initializer.h"
#include "channel_data.h"
//#include "signal_handler.h"
#include "executor.h"
#include "server_socket_channel.h"
#include "channel_pipeline.h"

OPEN_LOCKLESS_NAMESPACE

server_bootstrap::server_bootstrap(
        std::shared_ptr<channel_pipeline_initializer_factory> _bootstrap_factory,
        std::shared_ptr<channel_pipeline_initializer_factory> _channel_factory)
    : m_main_executor(new executor(GEN_UUID, 1))
    , m_pool_executor(new executor(GEN_UUID, 8))
    , m_context(new runtime_context(m_main_executor->worker_service()))
    , m_bootstrap_factory(_bootstrap_factory)
    , m_channel_factory(_channel_factory)
{
    AUTO_TRACE(m_context->uuid());
}

server_bootstrap::~server_bootstrap()
{
    
}

void server_bootstrap::synchronize()
{
    AUTO_TRACE(m_context->uuid());
    m_main_executor->syncronize();
    m_pool_executor->syncronize();
}

void server_bootstrap::shutdown()
{
    AUTO_TRACE(m_context->uuid());
    if (m_acceptor) {
        m_acceptor->close();
    }
    m_executor_started = nullptr;
    m_main_executor->shutdown();
    m_pool_executor->shutdown();
}

void server_bootstrap::ps_main_executor_started(std::shared_ptr<object> /*_this*/)
{
    AUTO_TRACE(m_context->uuid());
    m_executor_started();
    m_executor_started = nullptr;
    LOG_INFO_0(m_context->uuid(), "server_bootstrap::ps_main_executor_started");
}

void server_bootstrap::ps_pool_executor_started(std::shared_ptr<object> /*_this*/, int _position)
{
    AUTO_TRACE(m_context->uuid());
    LOG_INFO_1(m_context->uuid(), "server_bootstrap::ps_pool_executor_started %d", _position);
}

void server_bootstrap::ps_channel_finished(std::shared_ptr<object> /*_this*/, std::shared_ptr<server_socket_channel> _channel)
{
    AUTO_TRACE(m_context->uuid());
    m_channels.erase(_channel->context()->uuid());
    LOG_INFO_1(m_context->uuid(), "<==m_channels count:%lu", m_channels.size());
}

void server_bootstrap::start_accept(asio::ip::tcp::endpoint _ep)
{
    AUTO_TRACE(m_context->uuid());
    m_main_executor->ps_started.subscribe(
             shared_this,
             m_context,
             std::bind(&server_bootstrap::ps_main_executor_started, this, std::placeholders::_1));

    m_pool_executor->ps_started.subscribe(
             shared_this,
             m_context,
             std::bind(&server_bootstrap::ps_pool_executor_started, this, std::placeholders::_1, std::placeholders::_2));

    m_executor_started = std::bind(&server_bootstrap::do_start_accept, this, _ep);

    m_main_executor->startup();
    m_pool_executor->startup();
}

void server_bootstrap::do_start_accept(asio::ip::tcp::endpoint _ep)
{
    AUTO_TRACE(m_context->uuid());
    if (m_bootstrap_factory.get()) {
        
        auto context = std::make_shared<runtime_context>(m_main_executor->worker_service());
        m_pipeline = std::make_shared<channel_pipeline>(context);
        m_bootstrap_factory->new_initializer()->init_channel(m_pipeline);
    }
    try {
        m_acceptor = std::make_shared<asio::ip::tcp::acceptor>(*m_context->get_io_service(), _ep);
        do_accept();
        if (m_pipeline.get()) {
            // report accept action has been started.
            auto data = std::make_shared<accept_started>(_ep.address().to_string(), _ep.port());
            m_pipeline->post_handle_read_data(data, std::shared_ptr<router_info>(), nullptr);
        }
    }
    catch (const asio::system_error &_ec) {
        LOG_ERROR_3(m_context->uuid(), "create acceptor error on %s:%u %s", _ep.address().to_string().c_str(), _ep.port(), _ec.what());
        if (m_pipeline.get()) {
            // report accept action has been started.
            auto data = std::make_shared<accept_error>(_ec, _ep);
            m_pipeline->post_handle_read_data(data, std::shared_ptr<router_info>(), nullptr);
        }
    }
}

void server_bootstrap::do_accept()
{
    AUTO_TRACE(m_context->uuid());
    auto service = m_pool_executor->worker_service();
    auto socket = std::make_shared<asio::ip::tcp::socket>(*service);
    m_acceptor->async_accept(*socket, std::bind(&server_bootstrap::on_accept, this, std::placeholders::_1, socket, service));
}

void server_bootstrap::on_accept(
        asio::error_code _error,
        std::shared_ptr<asio::ip::tcp::socket> _socket,
        std::shared_ptr<asio::io_service> _service)
{
    AUTO_TRACE(m_context->uuid());
    if(_error) {
        LOG_ERROR_1(m_context->uuid(), "server_bootstrap::on_accept error:%s", _error.message().c_str());
    }
    else {
        auto channel = std::make_shared<server_socket_channel>(_service, _socket, _socket->remote_endpoint(), m_channel_factory->new_initializer());
        channel->ps_finish.subscribe(shared_this,
                                     m_context,
                                     std::bind(&server_bootstrap::ps_channel_finished,
                                               this,
                                               std::placeholders::_1,
                                               std::placeholders::_2)
                                     );
        m_channels[channel->context()->uuid()] = channel;
        LOG_INFO_1(m_context->uuid(), "==>m_channels count:%lu", m_channels.size());
        channel->post_start(nullptr);
        m_context->execute(nullptr, std::bind(&server_bootstrap::do_accept, this));
    }
}

CLOSE_LOCKLESS_NAMESPACE
