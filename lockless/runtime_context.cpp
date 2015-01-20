
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_serialize.hpp>
#include <boost/lexical_cast.hpp>

#include "runtime_context.h"

OPEN_LOCKLESS_NAMESPACE

runtime_context::runtime_context(std::shared_ptr<asio::io_service> _service)
    : m_uuid(GEN_UUID)
    , m_service(_service)
    , m_strand(new asio::strand(*_service))
{

}

boost::uuids::uuid runtime_context::uuid() const
{
    return m_uuid;
}

std::shared_ptr<asio::io_service> runtime_context::get_io_service() const
{
    return m_service;
}

std::shared_ptr<asio::strand> runtime_context::command_strand()
{
    return m_strand;
}

CLOSE_LOCKLESS_NAMESPACE
