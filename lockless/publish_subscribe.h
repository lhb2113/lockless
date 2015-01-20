#ifndef PUBLISH_SUBSCRIBE_H
#define PUBLISH_SUBSCRIBE_H

#include "defines.h"
#include "object.h"
#include "runtime_context.h"
#include "logger.h"

OPEN_LOCKLESS_NAMESPACE

template<typename FuncType>
class ps_connect : public object
{
public:
    ps_connect()
    {

    }

    void subscribe(
            std::shared_ptr<object> _subscriber,
            std::shared_ptr<runtime_context> _context,
            FuncType _func)
    {
        m_subscriber = _subscriber;
        m_context = _context;
        m_func = _func;
    }

    void publish()
    {
        std::shared_ptr<object> subscriber = m_subscriber.lock();
        std::shared_ptr<runtime_context> context = m_context.lock();
        if (context.get() && subscriber.get()) {
            context->execute(nullptr, std::bind(m_func, subscriber));
        }
    }

    template<typename Type>
    void publish(Type _p0)
    {
        std::shared_ptr<object> subscriber = m_subscriber.lock();
        std::shared_ptr<runtime_context> context = m_context.lock();
        if (context.get() && subscriber.get()) {
            context->execute(nullptr, std::bind(m_func, subscriber, _p0));
        }
    }

private:
    std::weak_ptr<object> m_subscriber;
    std::weak_ptr<runtime_context> m_context;
    FuncType m_func;
};

CLOSE_LOCKLESS_NAMESPACE

#endif // PUBLISH_SUBSCRIBE_H
