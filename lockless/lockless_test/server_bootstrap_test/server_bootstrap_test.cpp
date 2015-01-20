//
//  server_bootstrap_test.cpp
//  lockless
//
//  Created by liuhaibo on 14-12-8.
//  Copyright (c) 2014年 liuhaibo. All rights reserved.
//

#include "server_bootstrap_test.h"
#include "lockless_test_utils.h"
#include "client_server_mock.h"
#include <memory>

ACTION_P2(action_bootstrap_init_channel, _context, _h1)
{
    std::shared_ptr<channel_pipeline> pipeline = arg0;
    pipeline->post_add_to_context(_context, "com.test.bootstrap", [](std::future<ret_map> _ret)
    {
        
    });
    pipeline->post_add_channel_handler(_h1, "h1", false, nullptr);
}

void server_bootstrap_test::SetUp()
{
    g_object_count = 0;
    std::shared_ptr<object> monitor(new object);
    m_pipeline_context = std::make_shared<channel_pipeline_context>();
    ASSERT_TRUE(wait_for(g_wait_interval, [monitor, this](std::promise<void> &go) {
        m_pipeline_context->ps_started.subscribe(monitor, m_pipeline_context->context(), [&go, monitor](std::shared_ptr<object> _subscriber) {
            ASSERT_EQ(monitor, _subscriber);
            go.set_value();
        });
        m_pipeline_context->startup();
    }));
    
    auto bootstrap_factory = std::make_shared<server_bootstrap_initializer_factory_mock>();
    auto bootstrap_initializer = std::make_shared<server_bootstrap_initializer_mock>();
    auto frontend_factory = std::make_shared<frontend_initializer_factory_mock>();
    m_server_bootstrap = std::make_shared<server_bootstrap>(bootstrap_factory, frontend_factory);
    
    EXPECT_CALL(*bootstrap_factory, new_initializer()).WillOnce(testing::Return(bootstrap_initializer));
    
    ASSERT_TRUE(wait_for(g_wait_interval, [this, bootstrap_initializer](std::promise<void> &go){
        
        auto bootstrap_handler = std::make_shared<channel_handler>();
        bootstrap_handler->set_read_handler(lockless_data::accept_started, [&go](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>){
            
            go.set_value();
        });
        
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string("127.0.0.1"), 9999);
        m_server_bootstrap->start_accept(ep);
        EXPECT_CALL(*bootstrap_initializer, init_channel(testing::_)).WillOnce(action_bootstrap_init_channel(m_pipeline_context, bootstrap_handler));
    }));
}

void server_bootstrap_test::TearDown()
{
    m_server_bootstrap->shutdown();
    m_server_bootstrap->synchronize();
    m_server_bootstrap.reset();
    
    m_pipeline_context->shutdown();
    m_pipeline_context->synchronize();
    m_pipeline_context.reset();
    
    ASSERT_EQ(g_object_count, 0);
}

TEST_F(server_bootstrap_test, start_accept_twice)
{
    std::shared_ptr<object> monitor(new object);
    
    auto bootstrap_factory = std::make_shared<server_bootstrap_initializer_factory_mock>();
    auto bootstrap_initializer = std::make_shared<server_bootstrap_initializer_mock>();
    auto frontend_factory = std::make_shared<frontend_initializer_factory_mock>();
    auto bootstrap = std::make_shared<server_bootstrap>(bootstrap_factory, frontend_factory);
    
    
    EXPECT_CALL(*bootstrap_factory, new_initializer()).WillOnce(testing::Return(bootstrap_initializer));
    
    ASSERT_TRUE(wait_for(g_wait_interval, [this, bootstrap, bootstrap_initializer](std::promise<void> &go){
        
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string("127.0.0.1"), 9999);
        bootstrap->start_accept(ep);
        
        auto bootstrap_handler = std::make_shared<channel_handler>();
        bootstrap_handler->set_read_handler(lockless_data::accept_started, [&go](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>){
            ASSERT_TRUE(false);
        });
        bootstrap_handler->set_read_handler(lockless_data::accept_error, [&go](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>){
            auto real = channel_data::convert<accept_error>(_data);
            ASSERT_EQ("127.0.0.1", real->get_endpoint().address().to_string());
            ASSERT_EQ(9999, real->get_endpoint().port());
            real->get_error_code().code().default_error_condition().value();
            ASSERT_EQ(static_cast<int>(std::errc::address_in_use), real->get_error_code().code().default_error_condition().value());
            
            go.set_value();
        });
        EXPECT_CALL(*bootstrap_initializer, init_channel(testing::_)).WillOnce(action_bootstrap_init_channel(m_pipeline_context, bootstrap_handler));
    }));
    
    bootstrap->shutdown();
    bootstrap->synchronize();
    
    testing::Mock::AllowLeak(bootstrap_factory.get());
    testing::Mock::AllowLeak(bootstrap_initializer.get());
    testing::Mock::AllowLeak(frontend_factory.get());
}

