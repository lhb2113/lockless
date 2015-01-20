#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <syslog.h>
#include <stdio.h>
#include <boost/date_time.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

#include "defines.h"

boost::posix_time::ptime get_current_date_time();

std::string get_prefix(boost::uuids::uuid _uuid, const std::string &_tag, bool _need_date_time);

std::string buffer_to_hex_string(const std::string &buffer);

int get_logger_level();

void set_logger_level(int _level);

#define SYSLOG_0(UUID, PRI, TAG, STR) \
{ \
    const std::string &syslog_prefix = get_prefix(UUID, TAG, false); \
    if ( PRI <= get_logger_level()) { \
        syslog(LOG_LOCAL0|PRI, "%s " STR, syslog_prefix.c_str()); \
        \
        const std::string &console_prefix = get_prefix(UUID, TAG, true); \
        printf("%s " STR, console_prefix.c_str());printf("\n");\
    } \
}

#define SYSLOG_1(UUID, PRI, TAG, FMT, P0) \
{ \
    const std::string &syslog_prefix = get_prefix(UUID, TAG, false); \
    if ( PRI <= get_logger_level()) { \
        syslog(LOG_LOCAL0|PRI, "%s " FMT, syslog_prefix.c_str(), P0); \
        \
        const std::string &console_prefix = get_prefix(UUID, TAG, true); \
        printf("%s " FMT, console_prefix.c_str(), P0);printf("\n");\
    } \
}

#define SYSLOG_2(UUID, PRI, TAG, FMT, P0, P1) \
{ \
    const std::string &syslog_prefix = get_prefix(UUID, TAG, false); \
    if ( PRI <= get_logger_level()) { \
        syslog(LOG_LOCAL0|PRI, "%s " FMT, syslog_prefix.c_str(), P0, P1); \
        \
        const std::string &console_prefix = get_prefix(UUID, TAG, true); \
        printf("%s " FMT, console_prefix.c_str(), P0, P1);printf("\n");\
    } \
}

#define SYSLOG_3(UUID, PRI, TAG, FMT, P0, P1, P2) \
{ \
    const std::string &syslog_prefix = get_prefix(UUID, TAG, false); \
    if ( PRI <= get_logger_level()) { \
        syslog(LOG_LOCAL0|PRI, "%s " FMT, syslog_prefix.c_str(), P0, P1, P2); \
        \
        const std::string &console_prefix = get_prefix(UUID, TAG, true); \
        printf("%s " FMT, console_prefix.c_str(), P0, P1, P2);printf("\n");\
    } \
}

#define SYSLOG_4(UUID, PRI, TAG, FMT, P0, P1, P2, P3) \
{ \
    const std::string &syslog_prefix = get_prefix(UUID, TAG, false); \
    if ( PRI <= get_logger_level()) { \
        syslog(LOG_LOCAL0|PRI, "%s " FMT, syslog_prefix.c_str(), P0, P1, P2, P3); \
        \
        const std::string &console_prefix = get_prefix(UUID, TAG, true); \
        printf("%s " FMT, console_prefix.c_str(), P0, P1, P2, P3);printf("\n");\
    } \
}

//#define SYSLOG_0(UUID, PRI, TAG, STR) \
//{ \
//const std::string &syslog_prefix = get_prefix(UUID, TAG, false); \
//if ( PRI <= get_logger_level()) { \
//syslog(LOG_LOCAL0|PRI, "%s " STR, syslog_prefix.c_str()); \
//} \
//}
//
//#define SYSLOG_1(UUID, PRI, TAG, FMT, P0) \
//{ \
//const std::string &syslog_prefix = get_prefix(UUID, TAG, false); \
//if ( PRI <= get_logger_level()) { \
//syslog(LOG_LOCAL0|PRI, "%s " FMT, syslog_prefix.c_str(), P0); \
//} \
//}
//
//#define SYSLOG_2(UUID, PRI, TAG, FMT, P0, P1) \
//{ \
//const std::string &syslog_prefix = get_prefix(UUID, TAG, false); \
//if ( PRI <= get_logger_level()) { \
//syslog(LOG_LOCAL0|PRI, "%s " FMT, syslog_prefix.c_str(), P0, P1); \
//} \
//}
//
//#define SYSLOG_3(UUID, PRI, TAG, FMT, P0, P1, P2) \
//{ \
//const std::string &syslog_prefix = get_prefix(UUID, TAG, false); \
//if ( PRI <= get_logger_level()) { \
//syslog(LOG_LOCAL0|PRI, "%s " FMT, syslog_prefix.c_str(), P0, P1, P2); \
//} \
//}
//
//#define SYSLOG_4(UUID, PRI, TAG, FMT, P0, P1, P2, P3) \
//{ \
//const std::string &syslog_prefix = get_prefix(UUID, TAG, false); \
//if ( PRI <= get_logger_level()) { \
//syslog(LOG_LOCAL0|PRI, "%s " FMT, syslog_prefix.c_str(), P0, P1, P2, P3); \
//} \
//}

#define LOG_ERROR_0(UUID, STR) SYSLOG_0(UUID, LOG_ERR, "E", STR);
#define LOG_ERROR_1(UUID, FMT, P0) SYSLOG_1(UUID, LOG_ERR, "E", FMT, P0);
#define LOG_ERROR_2(UUID, FMT, P0, P1) SYSLOG_2(UUID, LOG_ERR, "E", FMT, P0, P1);
#define LOG_ERROR_3(UUID, FMT, P0, P1, P2) SYSLOG_3(UUID, LOG_ERR, "E", FMT, P0, P1, P2);
#define LOG_ERROR_4(UUID, FMT, P0, P1, P2, P3) SYSLOG_4(UUID, LOG_ERR, "E", FMT, P0, P1, P2, P3);

#define LOG_WARN_0(UUID, STR) SYSLOG_0(UUID, LOG_WARNING, "W", STR);
#define LOG_WARN_1(UUID, FMT, P0) SYSLOG_1(UUID, LOG_WARNING, "W", FMT, P0);
#define LOG_WARN_2(UUID, FMT, P0, P1) SYSLOG_2(UUID, LOG_WARNING, "W", FMT, P0, P1);
#define LOG_WARN_3(UUID, FMT, P0, P1, P2) SYSLOG_3(UUID, LOG_WARNING, "W", FMT, P0, P1, P2);
#define LOG_WARN_4(UUID, FMT, P0, P1, P2, P3) SYSLOG_4(UUID, LOG_WARNING, "W", FMT, P0, P1, P2, P3);

#define LOG_INFO_0(UUID, STR) SYSLOG_0(UUID, LOG_INFO, "I", STR);
#define LOG_INFO_1(UUID, FMT, P0) SYSLOG_1(UUID, LOG_INFO, "I", FMT, P0);
#define LOG_INFO_2(UUID, FMT, P0, P1) SYSLOG_2(UUID, LOG_INFO, "I", FMT, P0, P1);
#define LOG_INFO_3(UUID, FMT, P0, P1, P2) SYSLOG_3(UUID, LOG_INFO, "I", FMT, P0, P1, P2);
#define LOG_INFO_4(UUID, FMT, P0, P1, P2, P3) SYSLOG_4(UUID, LOG_INFO, "I", FMT, P0, P1, P2, P3);

#define LOG_DEBUG_0(UUID, STR) SYSLOG_0(UUID, LOG_DEBUG, "D", STR);
#define LOG_DEBUG_1(UUID, FMT, P0) SYSLOG_1(UUID, LOG_DEBUG, "D", FMT, P0);
#define LOG_DEBUG_2(UUID, FMT, P0, P1) SYSLOG_2(UUID, LOG_DEBUG, "D", FMT, P0, P1);
#define LOG_DEBUG_3(UUID, FMT, P0, P1, P2) SYSLOG_3(UUID, LOG_DEBUG, "D", FMT, P0, P1, P2);
#define LOG_DEBUG_4(UUID, FMT, P0, P1, P2, P3) SYSLOG_4(UUID, LOG_DEBUG, "D", FMT, P0, P1, P2, P3);

#define LOG_TRACE_0(UUID, FMT, P0) SYSLOG_1(UUID, LOG_DEBUG, "T", FMT, P0);
#define LOG_TRACE_1(UUID, FMT, P0, P1) SYSLOG_2(UUID, LOG_DEBUG, "T", FMT, P0, P1);

#define BUFFER(BUF) buffer_to_hex_string(BUF)

OPEN_LOCKLESS_NAMESPACE

class auto_tracer
{
public:
    auto_tracer(boost::uuids::uuid _uuid, const std::string &_function);
    ~auto_tracer();
private:
    boost::uuids::uuid m_uuid;
    std::string m_function;
};

CLOSE_LOCKLESS_NAMESPACE

USING_LOCKLESS_NAMESPACE

#define AUTO_TRACE(UUID) auto_tracer __t(UUID, __PRETTY_FUNCTION__)

#endif // LOGGER_H
