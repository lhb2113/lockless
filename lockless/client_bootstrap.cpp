#include "client_bootstrap.h"
#include "client_socket_channel.h"
#include "channel_pipeline_initializer.h"
#include "executor.h"

OPEN_LOCKLESS_NAMESPACE

client_bootstrap::client_bootstrap(std::shared_ptr<channel_pipeline_initializer_factory> _factory)
    : m_executor(new executor(GEN_UUID, 1))
    , m_context(new runtime_context(m_executor->worker_service()))
    , m_initializer_factory(_factory)
{
    AUTO_TRACE(m_context->uuid());
}

client_bootstrap::~client_bootstrap()
{
    
}

void client_bootstrap::connect(asio::ip::tcp::endpoint _ep)
{
    AUTO_TRACE(m_context->uuid());

    m_executor->ps_started.subscribe(
                shared_this,
                m_context,
                std::bind(&client_bootstrap::ps_executor_started, this, std::placeholders::_1, std::placeholders::_2));
    m_executor->startup();

    auto service = m_executor->worker_service();
    auto socket = std::make_shared<asio::ip::tcp::socket>(*service);
    m_channel = std::make_shared<client_socket_channel>(service,
                                                        socket,
                                                        m_initializer_factory->new_initializer());
    m_channel->ps_finish.subscribe(shared_this,
                                   m_context,
                                   std::bind(&client_bootstrap::ps_channel_finished, this, std::placeholders::_1, std::placeholders::_2));
    m_channel->post_start(_ep, nullptr);
}

void client_bootstrap::disconnect()
{
    AUTO_TRACE(m_context->uuid());
    m_channel->post_finish_channel(nullptr);
    m_executor->syncronize();
}

void client_bootstrap::ps_executor_started(std::shared_ptr<object> /*_this*/, int _index)
{
    AUTO_TRACE(m_context->uuid());
}

void client_bootstrap::ps_channel_finished(std::shared_ptr<object> /*_this*/, std::shared_ptr<client_socket_channel> /*_channel*/)
{
    AUTO_TRACE(m_context->uuid());
    m_channel.reset();
    ps_finish.publish<std::shared_ptr<client_bootstrap>>(shared_this);
    m_executor->shutdown();
}

CLOSE_LOCKLESS_NAMESPACE
