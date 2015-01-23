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
    boost::uuids::uuid m_uuid;
    std::vector<std::thread> m_thread_group;
    std::shared_ptr<asio::io_service> m_service;
    std::shared_ptr<asio::io_service::work> m_work;
    int m_thread_count;
};

CLOSE_LOCKLESS_NAMESPACE

#endif // THREADPOOLEXECUTOR_H
