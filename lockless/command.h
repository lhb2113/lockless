#ifndef COMMAND_H
#define COMMAND_H

#include <asio.hpp>
#include <future>

#include "defines.h"
#include "object.h"

OPEN_LOCKLESS_NAMESPACE

class command_base;

class command_receiver : public object
{
public:
    void invoke(std::shared_ptr<command_base> _command);
    virtual std::shared_ptr<asio::strand> command_strand() = 0;
private:
    void do_invoke(std::shared_ptr<command_base> _command);
};

class command_base : public object
{
public:
    command_base(std::shared_ptr<command_receiver> _receiver);
public:
    void perform();
private:
    virtual void execute() = 0;
protected:
    std::weak_ptr<command_receiver> m_receiver;
    friend class command_receiver;
};


template<class F, class... Args>
class command_wrapper : public command_base
{
public:
    typedef typename std::result_of<F(Args...)>::type return_type;
public:
    command_wrapper(std::shared_ptr<command_receiver> _receiver,
                    std::function<void(std::future<return_type>)> _callback,
                    F f,
                    Args... args)
    : command_base(_receiver)
    , m_command(new std::packaged_task<return_type()>(std::bind(std::forward<F>(f), std::forward<Args>(args)...)))
    , m_callback(_callback)
    {
        
    }
private:
    void execute()
    {
        if (m_command.get() && m_callback) {
            (*m_command)();
            m_callback(m_command->get_future());
        }
        else if (m_command && !m_callback) {
            (*m_command)();
        }
        else {
            // Do nothing.
        }
    }
protected:
    std::shared_ptr<std::packaged_task<return_type()>> m_command;
    std::function<void(std::future<return_type>)> m_callback;
};

CLOSE_LOCKLESS_NAMESPACE

#endif // COMMAND_H
