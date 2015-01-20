#ifndef CHANNEL_DATA_H
#define CHANNEL_DATA_H

#include <string>
#include <asio.hpp>
#include "defines.h"
#include "object.h"
#include "data_type.h"

OPEN_LOCKLESS_NAMESPACE

class router_info : public object
{
public:
    std::string from;
    std::string to;
    std::string wild;
public:
    static std::string wild_any;
    static std::string wild_exactly;
    static std::string wild_all;
};

enum class lockless_data
{
    socket_stream = 1, // socket stream data from tcp::socket
    connect_status,    // channel connect status
    accept_started,    // bootstrap start an accept
    accept_error,      // bootstrap accept error
    pipeline_added,    // sub pipeline added was post to its parent pipeline
    pipeline_removed,  // sub pipeline removed was post to its parent pipeline
};

template<>
struct data_category_trait<lockless_data>
{
    static const data_category &category();
};

class channel_data : public object
{
public:
    channel_data(data_type _type);
public:
    data_type get_type() const;
    virtual std::string to_string() const;
public:
    template<typename RealDataType>
    static std::shared_ptr<RealDataType> convert(std::shared_ptr<channel_data> _data)
    {
        return std::dynamic_pointer_cast<RealDataType>(_data);
    }

private:
    data_type m_type;
};

class socket_stream : public channel_data
{
public:
    socket_stream(std::shared_ptr<std::string> _payload);
public:
    std::shared_ptr<std::string> get_payload() const;
    std::string to_string() const;
private:
    std::shared_ptr<std::string> m_payload;
};

class connect_status : public channel_data
{
public:
    enum status
    {
        unkown,
        connecting,
        connected,
        read_error,
        write_error,
        accept_error,
        connect_refused
    };
    connect_status(status _status, const std::string &_addr, std::size_t _port);
public:
    status get_status() const;
    const std::string &get_address() const;
    std::size_t get_port() const;
    std::string to_string() const;
private:
    status m_status;
    std::string m_address;
    std::size_t m_port;
};

class accept_started : public channel_data
{
public:
    accept_started(const std::string &_addr, int _port);
public:
    const std::string &get_address() const;
    int get_port() const;
    std::string to_string() const;
private:
    std::string m_address;
    int m_port;
};

class accept_error : public channel_data
{
public:
    accept_error(const asio::system_error &_ec, const asio::ip::tcp::endpoint &_ep);
    const asio::system_error &get_error_code() const;
    const asio::ip::tcp::endpoint &get_endpoint() const;
    std::string to_string() const;
private:
    asio::system_error m_ec;
    asio::ip::tcp::endpoint m_ep;
};

class pipeline_added : public channel_data
{
public:
    pipeline_added(const std::string &_pipeline_name);
    const std::string &get_pipeline_name() const;
    std::string to_string() const;
private:
    std::string m_pipeline_name;
};

class pipeline_removed : public channel_data
{
public:
    pipeline_removed(const std::string &_pipeline_name);
    const std::string &get_pipeline_name() const;
    std::string to_string() const;
private:
    std::string m_pipeline_name;
};

CLOSE_LOCKLESS_NAMESPACE

#endif // CHANNEL_DATA_H
