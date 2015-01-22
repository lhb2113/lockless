//
//  main.cpp
//  server
//
//  Created by liuhaibo on 15-1-22.
//  Copyright (c) 2015年 liuhaibo. All rights reserved.
//

#include <iostream>
#include <lockless/lockless.h>

#include "main.h"

const std::string COM_SERVER_FRONTEND  = "com.server.frontend";
const std::string COM_STUB   = "com.server.stub";

server_backend_pipeline_initializer::server_backend_pipeline_initializer(std::shared_ptr<channel_pipeline_context> _context)
: m_context(_context)
{
    
}

void server_backend_pipeline_initializer::init_channel(std::shared_ptr<channel_pipeline> _pipeline)
{
    _pipeline->post_add_to_context(m_context, COM_STUB, nullptr);
}

server_backend_pipeline_initializer_factory::server_backend_pipeline_initializer_factory(std::shared_ptr<channel_pipeline_context> _context)
: m_context(_context)
{
    
}

std::shared_ptr<channel_pipeline_initializer> server_backend_pipeline_initializer_factory::new_initializer() const
{
    return std::shared_ptr<server_backend_pipeline_initializer>(new server_backend_pipeline_initializer(m_context));
}

server_frontend_pipeline_initializer::server_frontend_pipeline_initializer(std::shared_ptr<channel_pipeline_context> _context)
: m_context(_context)
{
    
}

void server_frontend_pipeline_initializer::init_channel(std::shared_ptr<channel_pipeline> _pipeline)
{
    AUTO_TRACE(_pipeline->context()->uuid());
    
    std::stringstream ss;
    ss<<COM_SERVER_FRONTEND<<"."<<_pipeline->context()->uuid();
    _pipeline->post_add_to_context(m_context, ss.str(), nullptr);
    
    _pipeline->post_add_channel_handler(std::shared_ptr<channel_handler>(new server_frontend_logger_handler),
                                        "center_frontend_logger",
                                        false,
                                        nullptr
                                        );
}

server_frontend_pipeline_initializer_factory::server_frontend_pipeline_initializer_factory(std::shared_ptr<channel_pipeline_context> _context)
: m_context(_context)
{
    
}

std::shared_ptr<channel_pipeline_initializer> server_frontend_pipeline_initializer_factory::new_initializer() const
{
    return std::shared_ptr<channel_pipeline_initializer>(new server_frontend_pipeline_initializer(m_context));
}

server_frontend_logger_handler::server_frontend_logger_handler()
{
    SET_READ_HANDLER(lockless_data::socket_stream, server_frontend_logger_handler::handle_read_any_data);
    SET_READ_HANDLER(lockless_data::connect_status, server_frontend_logger_handler::handle_read_any_data);
}

void server_frontend_logger_handler::handle_read_any_data(std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info)
{
    LOG_DEBUG_1(get_handler_context()->context()->uuid(),
                "server_frontend_logger_handler::handle_read_any_data:%s", _data->to_string().c_str());
    get_handler_context()->next_read_data(_data);
}


int main(int argc, const char * argv[]) {
    // insert code here...
    std::shared_ptr<channel_pipeline_context> pipeline_context(new channel_pipeline_context);
    
    std::shared_ptr<channel_pipeline_initializer_factory> bootstrap_factory(new server_backend_pipeline_initializer_factory(pipeline_context));
    std::shared_ptr<channel_pipeline_initializer_factory> channel_factory(new server_frontend_pipeline_initializer_factory(pipeline_context));
    std::shared_ptr<server_bootstrap> bootstrap(new server_bootstrap(bootstrap_factory, channel_factory));
    
    asio::ip::tcp::endpoint ep(asio::ip::address::from_string("127.0.0.1"), 6666);
    bootstrap->start_accept(ep);
    pipeline_context->startup();
    pipeline_context->synchronize();
    return 0;
}
