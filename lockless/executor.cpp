
#include "executor.h"
#include "logger.h"

OPEN_LOCKLESS_NAMESPACE

executor::executor(boost::uuids::uuid _uuid, int _thread_count)
    : m_uuid(_uuid)
    , m_duration(boost::posix_time::seconds(KEEP_ALIVE_INTERVAL))
{
    AUTO_TRACE(m_uuid);
    for(int i = 0; i < _thread_count; i ++) {
        m_worker_services.push_back(std::make_shared<asio::io_service>());
    }
}
executor::~executor()
{
    AUTO_TRACE(m_uuid);
}

void executor::startup()
{
    AUTO_TRACE(m_uuid);
    for(std::size_t i = 0; i < m_worker_services.size(); i++) {
        LOG_DEBUG_1(m_uuid, "executor::startup:%lu", i)
        auto thread = std::make_shared<std::thread>(std::bind(&executor::execute_worker_service, this, i));
        m_thread_group.push_back(thread);
    }
}

void executor::syncronize()
{
    AUTO_TRACE(m_uuid);
    for (std::size_t i = 0; i < m_thread_group.size(); i++) {
        LOG_DEBUG_1(m_uuid, "executor::syncronize:%lu", i)
        m_thread_group[i]->join();
    }
}

void executor::shutdown()
{
    AUTO_TRACE(m_uuid);
    for(size_t i = 0; i < m_thread_group.size(); i++) {
        LOG_DEBUG_1(m_uuid, "executor::shutdown:%lu", i)
        if (!m_worker_services[i]->stopped()) {
            m_worker_services[i]->reset();
            m_worker_services[i]->stop();
        }
    }
}

std::shared_ptr<asio::io_service> executor::worker_service() const
{
    if (m_worker_services.size()) {
        int index = rand() % m_worker_services.size();
        index = index / m_worker_services.size();
        auto service = m_worker_services[index];
        return service;
    }
    else {
        return nullptr;
    }
}

void executor::execute_worker_service(int _position)
{
    AUTO_TRACE(m_uuid);
    LOG_DEBUG_1(m_uuid, "executor::execute_worker_service:>>>%d", _position)
    
    ps_started.publish<int>(_position);
    auto service = m_worker_services[_position];
    auto timer = std::make_shared<asio::deadline_timer>(*service);

    timer->expires_from_now(m_duration);
    timer->async_wait(std::bind(&executor::keepalive_worker_service, this, _position, timer, std::placeholders::_1));
    
    service->reset();
    service->run();
    LOG_DEBUG_1(m_uuid, "executor::execute_worker_service:<<<%d", _position)
}

void executor::keepalive_worker_service(int _position, std::shared_ptr<asio::deadline_timer> _timer, const asio::error_code& _error)
{
    AUTO_TRACE(m_uuid);
    if (_error) {
        return;
    }
    _timer->expires_from_now(m_duration);
    _timer->async_wait(std::bind(&executor::keepalive_worker_service, this, _position, _timer, std::placeholders::_1));
}

CLOSE_LOCKLESS_NAMESPACE
