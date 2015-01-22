//
//  main.cpp
//  client
//
//  Created by liuhaibo on 15-1-22.
//  Copyright (c) 2015年 liuhaibo. All rights reserved.
//

#include <iostream>
#include <lockless/lockless.h>

#include "main.h"

const std::string COM_BACKEND_CENTER_PIPELINE   = "com.client.stub.server";

client_pipeline_initializer::client_pipeline_initializer(std::shared_ptr<channel_pipeline_context> _context)
: m_pipeline_context(_context)
{
    
}

void client_pipeline_initializer::init_channel(std::shared_ptr<channel_pipeline> _pipeline)
{
    AUTO_TRACE(_pipeline->context()->uuid());
    _pipeline->post_add_to_context(m_pipeline_context, COM_BACKEND_CENTER_PIPELINE, nullptr);
    _pipeline->post_add_channel_handler(std::shared_ptr<channel_handler>(new client_logger_handler),
                                        "client_logger_handler",
                                        false,
                                        nullptr);
}

client_pipeline_initializer_factory::client_pipeline_initializer_factory(std::shared_ptr<channel_pipeline_context> _context)
: m_pipeline_context(_context)
{
    
}

std::shared_ptr<channel_pipeline_initializer> client_pipeline_initializer_factory::new_initializer() const
{
    return std::shared_ptr<channel_pipeline_initializer>(new client_pipeline_initializer(m_pipeline_context));
}

client_logger_handler::client_logger_handler()
{
    SET_READ_HANDLER(lockless_data::connect_status, client_logger_handler::handle_read_any);
}

void client_logger_handler::handle_read_any(std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info)
{
    LOG_DEBUG_1(get_handler_context()->context()->uuid(),
                "client_logger_handler::handle_read_any:%s", _data->to_string().c_str());
    get_handler_context()->next_read_data(_data);
}

int main(int argc, const char * argv[]) {
    // insert code here...
    std::shared_ptr<channel_pipeline_context> pipeline_context(new channel_pipeline_context);
    std::shared_ptr<channel_pipeline_initializer_factory> factory(new client_pipeline_initializer_factory(pipeline_context));
    std::shared_ptr<client_bootstrap> bootstrap(new client_bootstrap(factory));
    asio::ip::tcp::endpoint ep(asio::ip::address::from_string("127.0.0.1"), 6666);
    bootstrap->connect(ep);
    pipeline_context->startup();
    pipeline_context->synchronize();
    return 0;
}
