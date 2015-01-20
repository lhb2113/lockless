//
//  memory_leak_test.cpp
//  lockless
//
//  Created by liuhaibo on 14-12-22.
//  Copyright (c) 2014年 liuhaibo. All rights reserved.
//

#include "memory_leak_test.h"
#include "lockless_test_utils.h"

void backend_initializer::init_channel(std::shared_ptr<channel_pipeline> _pipeline)
{
    _pipeline->post_add_to_context(m_context, "com.test.backend", [](std::future<ret_map> _ret)
                                  {
                                      
                                  });
}

std::shared_ptr<channel_pipeline_initializer> backend_initializer_factory::new_initializer() const
{
    return std::make_shared<backend_initializer>(m_context);
}

void frontend_initializer::init_channel(std::shared_ptr<channel_pipeline> _pipeline)
{
    
}

std::shared_ptr<channel_pipeline_initializer> frontend_initializer_factory::new_initializer() const
{
    return std::make_shared<frontend_initializer>();
}

void client_initializer::init_channel(std::shared_ptr<channel_pipeline> _pipeline)
{
    _pipeline->post_add_to_context(m_context, "com.test.backend", [](std::future<ret_map> _ret)
                                   {
                                       
                                   });
}

std::shared_ptr<channel_pipeline_initializer> client_initializer_factory::new_initializer() const
{
    return std::make_shared<client_initializer>(m_context);
}

void memory_leak_test::SetUp()
{
    g_object_count = 0;
    m_pipeline_context = std::make_shared<channel_pipeline_context>();
    auto monitor = std::make_shared<object>();
    wait_for(g_wait_interval, [monitor, this](std::promise<void> &go) {
        m_pipeline_context->ps_started.subscribe(monitor, m_pipeline_context->context(), [&go, monitor](std::shared_ptr<object> _subscriber) {
            go.set_value();
        });
        m_pipeline_context->startup();
    });
    
    auto backend_factory = std::make_shared<backend_initializer_factory>(m_pipeline_context);
    auto frontend_factory = std::make_shared<frontend_initializer_factory>();
    m_server_bootstrap = std::make_shared<server_bootstrap>(backend_factory, frontend_factory);
    
    asio::ip::tcp::endpoint ep(asio::ip::address::from_string("127.0.0.1"), 9999);
    m_server_bootstrap->start_accept(ep);
}

void memory_leak_test::TearDown()
{
    wait_until(g_wait_interval);
    
    m_server_bootstrap->shutdown();
    m_server_bootstrap->synchronize();
    m_server_bootstrap.reset();
    
    m_pipeline_context->shutdown();
    m_pipeline_context->synchronize();
    m_pipeline_context.reset();
    
    ASSERT_EQ(g_object_count, 0);
}

TEST_F(memory_leak_test, connect_server)
{
    std::shared_ptr<client_channel_pipeline> pipeline_client = std::make_shared<client_channel_pipeline>(m_pipeline_context->context());
    pipeline_client->set_name("com.test.client");
    
    // 1. config pipeline handler "client_handler"
    auto client_handler = std::make_shared<channel_handler>();
    
    // 2. add "client_handler" to pipeline "com.test.client"
    ASSERT_TRUE(wait_for(g_wait_interval, [pipeline_client, client_handler](std::promise<void> &go){
        
        pipeline_client->post_add_channel_handler(client_handler, "h1", false, [&go](std::future<ret_map>){
            go.set_value();
        });
    }));
    
    // 3. add "com.test.client" pipeline to pipeline context.
    ASSERT_TRUE(wait_for(g_wait_interval, [this, pipeline_client](std::promise<void> &go){
        m_pipeline_context->post_add_pipeline(pipeline_client, [&go](std::future<bool>){
            go.set_value();
        });
    }));
}
