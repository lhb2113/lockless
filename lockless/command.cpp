#include "command.h"

OPEN_LOCKLESS_NAMESPACE

void command_receiver::invoke(std::shared_ptr<command_base> _command)
{
    command_strand()->post(std::bind(&command_receiver::do_invoke, this, _command));
}

void command_receiver::do_invoke(std::shared_ptr<command_base> _command)
{
    _command->execute();
}

void command_base::perform()
{
    std::shared_ptr<command_receiver> receiver = m_receiver.lock();
    if (receiver.get()) {
        receiver->invoke(shared_this);
    }
}

command_base::command_base(std::shared_ptr<command_receiver> _receiver)
    : m_receiver(_receiver)
{

}


CLOSE_LOCKLESS_NAMESPACE
