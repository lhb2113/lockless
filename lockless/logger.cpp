#include "logger.h"
#include <asio.hpp>
#include <pthread.h>
#include <thread>

static int __logger_level__ = LOG_DEBUG;

int get_logger_level()
{
    return __logger_level__;
}

void set_logger_level(int _level)
{
    __logger_level__ = _level;
}

boost::posix_time::ptime get_current_date_time()
{
    boost::posix_time::ptime cur_ms = boost::posix_time::microsec_clock::local_time();
    long long ms = cur_ms.time_of_day().total_milliseconds();
    boost::posix_time::time_duration cur_time_ms = boost::posix_time::milliseconds(ms);
    boost::posix_time::ptime cur_date_ms(cur_ms.date(), cur_time_ms);
    return cur_date_ms;
}

std::string get_prefix(boost::uuids::uuid _uuid, const std::string &_tag, bool _need_date_time)
{
    std::stringstream ss;
    ss << "[" << std::this_thread::get_id() << "]";
    const std::string &uuid = boost::lexical_cast<std::string>(_uuid);
    const std::string &sub_uuid = uuid.substr(uuid.length() - 12, 12);
    ss << "[" << sub_uuid << "]";

    if (_need_date_time) {
        ss << "[" << get_current_date_time() << "]";
    }
    ss << "[" << _tag << "]";
    return ss.str();
}

void byte_to_hex(char str, char* hex)
{
    const char* cHex = "0123456789abcdef";
    int i=0;
    unsigned int a =  (unsigned int) str;
    hex[i++] = cHex[(a & 0xf0) >> 4];
    hex[i++] = cHex[(a & 0x0f)];
    hex[i] = '\0';
}

std::string buffer_to_hex_string(const std::string &buffer)
{
    std::string output;
    for(std::size_t i = 0; i < buffer.size(); i++) {
        char c = buffer.at(i);
        char hex[3];
        byte_to_hex(c, hex);
        output.append(hex);
    }
    return output;
}

OPEN_LOCKLESS_NAMESPACE

auto_tracer::auto_tracer(boost::uuids::uuid _uuid, const std::string &_function)
//    : m_uuid(_uuid)
: m_function(_function)
{
//    LOG_TRACE_0(m_uuid, "Enter:%s", m_function.c_str());
//    LOG_TRACE_0(m_uuid, "==%s", m_function.c_str());
}

auto_tracer::~auto_tracer()
{
//    LOG_TRACE_0(m_uuid, "Exit:%s", m_function.c_str());
}

CLOSE_LOCKLESS_NAMESPACE
