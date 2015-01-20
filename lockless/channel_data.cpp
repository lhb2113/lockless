
#include <sstream>
#include "channel_data.h"
#include "logger.h"

OPEN_LOCKLESS_NAMESPACE

std::string router_info::wild_any = "?";
std::string router_info::wild_exactly = ".";
std::string router_info::wild_all = "*";

class lockless_data_category : public data_category
{
public:
    const char* name() const _NOEXCEPT { return "lockless_category_impl"; }
    std::string message(int _type) const _NOEXCEPT {
        switch (_type) {
            case static_cast<int>(lockless_data::socket_stream):
                return "lockless_data::socket_stream";
            case static_cast<int>(lockless_data::connect_status):
                return "lockless_data::connect_status";
            case static_cast<int>(lockless_data::accept_started):
                return "lockless_data::accept_started";
            case static_cast<int>(lockless_data::accept_error):
                return "lockless_data::accept_error";
            case static_cast<int>(lockless_data::pipeline_added):
                return "lockless_data::pipeline_added";
            case static_cast<int>(lockless_data::pipeline_removed):
                return "lockless_data::pipeline_removed";
            default:
                return "lockless_data::unknown";
        }
    }
};

lockless_data_category g_lockless_category;

const data_category& lockless_category()
{
    return g_lockless_category;
}

const data_category &data_category_trait<lockless_data>::category()
{
    return lockless_category();
}

channel_data::channel_data(data_type _type)
    : m_type(_type)
{
}

data_type channel_data::get_type() const
{
    return m_type;
}

std::string channel_data::to_string() const
{
    std::stringstream ss;
    ss<<"["<<"data_type:"<<m_type.message()<<"]";
    return ss.str();
}

socket_stream::socket_stream(std::shared_ptr<std::string> _payload)
    : channel_data(lockless_data::socket_stream)
    , m_payload(_payload)
{

}

std::shared_ptr<std::string> socket_stream::get_payload() const
{
    return m_payload;
}

std::string socket_stream::to_string() const
{
    std::stringstream ss;
    ss<<"[type_socket_stream:"<<BUFFER(*m_payload).c_str()<<"]";
    return ss.str();
}

connect_status::connect_status(status _status, const std::string &_addr, std::size_t _port)
    : channel_data(lockless_data::connect_status)
    , m_status(_status)
    , m_address(_addr)
    , m_port(_port)
{

}

connect_status::status connect_status::get_status() const
{
    return m_status;
}


const std::string &connect_status::get_address() const
{
    return m_address;
}

std::size_t connect_status::get_port() const
{
    return m_port;
}

std::string connect_status::to_string() const
{
    std::stringstream ss;
    switch (m_status) {
    case connecting:
        ss<<"connect_status:connecting=>"<<m_address<<":"<<m_port;
        break;
    case connected:
        ss<<"connect_status:connected=>"<<m_address<<":"<<m_port;
        break;
    case read_error:
        ss<<"connect_status:read_error=>"<<m_address<<":"<<m_port;
        break;
    case write_error:
        ss<<"connect_status:write_error=>"<<m_address<<":"<<m_port;
        break;
    case accept_error:
        ss<<"connect_status:accept_error=>"<<m_address<<":"<<m_port;
        break;
    case connect_refused:
        ss<<"connect_refused=>"<<m_address<<":"<<m_port;
        break;
    default:
        ss<<"connect_status:unknown";
        break;
    }
    return ss.str();
}

accept_started::accept_started(const std::string &_addr, int _port)
    : channel_data(lockless_data::accept_started)
    , m_address(_addr)
    , m_port(_port)
{

}

const std::string &accept_started::get_address() const
{
    return m_address;
}

int accept_started::get_port() const
{
    return m_port;
}

std::string accept_started::to_string() const
{
    std::stringstream ss;
    ss<<"accept_started:"<<m_address<<":"<<m_port;
    return ss.str();
}

accept_error::accept_error(const asio::system_error &_ec, const asio::ip::tcp::endpoint &_ep)
    : channel_data(lockless_data::accept_error)
    , m_ec(_ec)
    , m_ep(_ep)
{
    
}

const asio::system_error &accept_error::get_error_code() const
{
    return m_ec;
}

const asio::ip::tcp::endpoint &accept_error::get_endpoint() const
{
    return m_ep;
}

std::string accept_error::to_string() const
{
    std::stringstream ss;
    ss<<"accept_error:"<<m_ec.what()<<" "<<m_ep.address().to_string()<<":"<<m_ep.port();
    return ss.str();
}

pipeline_added::pipeline_added(const std::string &_pipeline_name)
    : channel_data(lockless_data::pipeline_added)
    , m_pipeline_name(_pipeline_name)
{
    
}

const std::string &pipeline_added::get_pipeline_name() const
{
    return m_pipeline_name;
}

std::string pipeline_added::to_string() const
{
    std::stringstream ss;
    ss<<"pipeline_added:"<<m_pipeline_name;
    return ss.str();
}

pipeline_removed::pipeline_removed(const std::string &_pipeline_name)
    : channel_data(lockless_data::pipeline_removed)
    , m_pipeline_name(_pipeline_name)
{
    
}

const std::string &pipeline_removed::get_pipeline_name() const
{
    return m_pipeline_name;
}

std::string pipeline_removed::to_string() const
{
    std::stringstream ss;
    ss<<"pipeline_removed:"<<m_pipeline_name;
    return ss.str();
}

CLOSE_LOCKLESS_NAMESPACE
