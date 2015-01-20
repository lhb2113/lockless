#ifndef RUNTIME_CONTEXT_H
#define RUNTIME_CONTEXT_H

#include <future>
#include <asio.hpp>
#include <boost/uuid/uuid.hpp>
#include "defines.h"
#include "command.h"

OPEN_LOCKLESS_NAMESPACE

class runtime_context : public command_receiver
{
public:
    runtime_context(std::shared_ptr<asio::io_service> _service);
    boost::uuids::uuid uuid() const;
    
    template<class F, class... Args>
    void execute(std::function<void(std::future<typename std::result_of<F(Args...)>::type>)> _callback,
                 F f,
                 Args... args)
    {
        typedef std::function<typename std::result_of<F(Args...)>::type(Args...)> function_type;
        function_type func = f;
        if (func) {
            std::shared_ptr<command_wrapper<F, Args...>> command(
                     new command_wrapper<F, Args...>(shared_this, _callback, f, args...));
            command->perform();
        }
    }
    
    std::shared_ptr<asio::io_service> get_io_service() const;
private:
    std::shared_ptr<asio::strand> command_strand();
private:
    boost::uuids::uuid m_uuid;
    std::shared_ptr<asio::io_service> m_service;
    std::shared_ptr<asio::strand> m_strand;
};

CLOSE_LOCKLESS_NAMESPACE

#endif // RUNTIME_CONTEXT_H
