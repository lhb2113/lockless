#ifndef THREADPOOLEXECUTOR_H
#define THREADPOOLEXECUTOR_H

#include <asio.hpp>
#include <boost/uuid/uuid.hpp>

#include "defines.h"
#include "object.h"
#include "publish_subscribe.h"

OPEN_LOCKLESS_NAMESPACE

class executor : public object
{
public:
    ps_connect<std::function<void(std::shared_ptr<object>, int)>> ps_started;
public:
    executor(boost::uuids::uuid _uuid, int _thread_count);
    ~executor();
public:
    void startup();
    void syncronize();
    void shutdown();
public:
    std::shared_ptr<asio::io_service> worker_service() const;
private:
    void execute_worker_service(int _position);
    void keepalive_worker_service(int _position, std::shared_ptr<asio::deadline_timer> _timer, const asio::error_code& _error);
private:
    boost::uuids::uuid m_uuid;
    boost::posix_time::time_duration m_duration;
    std::vector<std::shared_ptr<std::thread>> m_thread_group;
    std::vector<std::shared_ptr<asio::io_service>> m_worker_services;
};

CLOSE_LOCKLESS_NAMESPACE

#endif // THREADPOOLEXECUTOR_H
