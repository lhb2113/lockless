
#include "executor.h"
#include "logger.h"

OPEN_LOCKLESS_NAMESPACE

executor::executor(boost::uuids::uuid _uuid, int _thread_count)
    : m_uuid(_uuid)
    , m_thread_count(_thread_count)
{
    AUTO_TRACE(m_uuid);
    m_service = std::make_shared<asio::io_service>();
}

executor::~executor()
{
    AUTO_TRACE(m_uuid);
}

void executor::startup()
{
    AUTO_TRACE(m_uuid);
    m_work = std::make_shared<asio::io_service::work>(*m_service);
    for(std::size_t i = 0; i < m_thread_count; i++) {
        LOG_DEBUG_1(m_uuid, "executor::startup:%lu", i)
        m_thread_group.emplace_back(std::thread([this, i](){
            int position = (int)i;
            ps_started.publish<int>(position);
            m_service->run();
        }));
    }
}

void executor::syncronize()
{
    AUTO_TRACE(m_uuid);
    for (std::size_t i = 0; i < m_thread_group.size(); i++) {
        LOG_DEBUG_1(m_uuid, "executor::syncronize:%lu", i)
        m_thread_group[i].join();
    }
}

void executor::shutdown()
{
    AUTO_TRACE(m_uuid);
    m_work.reset();
    m_service->stop();
    
    for (std::size_t i = 0; i < m_thread_group.size(); i++) {
        m_thread_group[i].join();
    }
    
    m_thread_group.clear();
}

std::shared_ptr<asio::io_service> executor::worker_service() const
{
    return m_service;
}

CLOSE_LOCKLESS_NAMESPACE
