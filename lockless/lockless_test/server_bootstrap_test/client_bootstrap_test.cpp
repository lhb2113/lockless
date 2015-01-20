//
//  client_bootstrap_test.cpp
//  lockless
//
//  Created by liuhaibo on 14-12-15.
//  Copyright (c) 2014年 liuhaibo. All rights reserved.
//

#include "client_bootstrap_test.h"
#include "lockless_test_utils.h"
#include "client_server_mock.h"
#include "memory_leak_test.h"

ACTION_P3(action_server_backend_init_channel, _context, _h1, _h1_name)
{
    std::shared_ptr<channel_pipeline> pipeline = arg0;
    pipeline->post_add_to_context(_context, "com.test.backend", [](std::future<ret_map> _ret)
                                  {
                                      
                                  });
    pipeline->post_add_channel_handler(_h1, _h1_name, false, nullptr);
}

ACTION_P(action_server_frontend_init_channel, _context)
{
    std::shared_ptr<channel_pipeline> pipeline = arg0;
    std::stringstream ss;
    ss<<"com.test.frontend"<<"."<<pipeline->context()->uuid();
    pipeline->post_add_to_context(_context, ss.str(), [](std::future<ret_map> _ret)
                                  {
                                      
                                  });
}

ACTION_P3(action_client_init_channel, _context, _h1, _h1_name)
{
    std::shared_ptr<channel_pipeline> pipeline = arg0;
    std::stringstream ss;
    ss<<"com.test.client"<<"."<<pipeline->context()->uuid();
    pipeline->post_add_to_context(_context, ss.str(), [](std::future<ret_map> _ret)
                                  {
                                      
                                  });
    pipeline->post_add_channel_handler(_h1, _h1_name, false, nullptr);
}

void client_bootstrap_test::SetUp()
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
    auto frontend_initializer = std::make_shared<frontend_initializer_mock>();
    
    m_server_bootstrap = std::make_shared<server_bootstrap>(bootstrap_factory, frontend_factory);
    
    EXPECT_CALL(*bootstrap_factory, new_initializer()).WillOnce(testing::Return(bootstrap_initializer));
    EXPECT_CALL(*frontend_factory, new_initializer()).Times(testing::AnyNumber()).WillRepeatedly(testing::Return(frontend_initializer));

    ASSERT_TRUE(wait_for(g_wait_interval, [this, bootstrap_initializer, frontend_initializer](std::promise<void> &go){
        
        auto bootstrap_handler = std::make_shared<channel_handler>();
        bootstrap_handler->set_read_handler(lockless_data::accept_started, [&go](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>){
            
            go.set_value();
        });
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string("127.0.0.1"), 9999);
        m_server_bootstrap->start_accept(ep);
        
        EXPECT_CALL(*bootstrap_initializer, init_channel(testing::_))
            .WillRepeatedly(action_server_backend_init_channel(m_pipeline_context, bootstrap_handler, "h1"));
        EXPECT_CALL(*frontend_initializer, init_channel(testing::_))
            .WillRepeatedly(action_server_frontend_init_channel(m_pipeline_context));
    }));
}

void client_bootstrap_test::TearDown()
{
    m_server_bootstrap->shutdown();
    m_server_bootstrap->synchronize();
    
    m_pipeline_context->shutdown();
    m_pipeline_context->synchronize();
    
    m_server_bootstrap.reset();
    m_pipeline_context.reset();
    
    ASSERT_EQ(0, g_object_count);
}

TEST_F(client_bootstrap_test, connect_success)
{
    std::shared_ptr<client_channel_pipeline_mock> pipeline_client = std::make_shared<client_channel_pipeline_mock>(m_pipeline_context->context());
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

    auto factory = std::make_shared<client_bootstrap_initializer_factory_mock>();
    auto initializer = std::make_shared<client_bootstrap_initializer_mock>();
    
    // 4. expect the call of factory->new_initializer() to return initializer.
    EXPECT_CALL(*factory, new_initializer()).Times(testing::AnyNumber()).WillRepeatedly(testing::Return(initializer));

    std::vector<std::shared_ptr<client_bootstrap>> clients;
    int max_count = 1;
    int added_count = 0;
    int removed_count = 0;
    int connected_count = 0;

    ASSERT_TRUE(wait_for(g_wait_interval, [max_count, client_handler, &added_count](std::promise<void> &go) {
        client_handler->set_read_handler(lockless_data::pipeline_added,[max_count, &go, &added_count](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>) {
            auto real = channel_data::convert<pipeline_added>(_data);
            added_count++;
            if (max_count == added_count) {
                go.set_value();
            }
        });
        
    }, [this, &connected_count, max_count, factory, initializer, &clients](std::promise<void> &go) {
        
        auto h1_handler = std::make_shared<channel_handler>();
        h1_handler->set_read_handler(lockless_data::connect_status, [&connected_count, &go, max_count](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>){
            auto real = channel_data::convert<connect_status>(_data);
            if (real->get_status() == connect_status::connected) {
                connected_count++;
                if (max_count == connected_count) {
                    go.set_value();
                }
            }
        });
        EXPECT_CALL(*initializer, init_channel(testing::_)).Times(testing::AnyNumber()).WillRepeatedly(action_client_init_channel(m_pipeline_context, h1_handler, "h1"));
        
        for(int i = 0; i < max_count; ++i) {
            auto client = std::make_shared<client_bootstrap>(factory);
            clients.push_back(client);
            asio::ip::tcp::endpoint ep(asio::ip::address::from_string("127.0.0.1"), 9999);
            client->connect(ep);
        }
    }));

    ASSERT_TRUE(wait_for(g_wait_interval, [&clients, &client_handler, max_count, &removed_count](std::promise<void> &go){
        client_handler->set_read_handler(lockless_data::pipeline_removed, [&removed_count, &go, max_count](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>) {
            auto real = channel_data::convert<pipeline_removed>(_data);
            removed_count++;
            if (max_count == removed_count) {
                go.set_value();
            }
        });
        for (int i = 0; i < clients.size(); i++) {
            std::shared_ptr<client_bootstrap> c = clients[i];
            c->disconnect();
        }
    }));
}

TEST_F(client_bootstrap_test, connect_refused)
{
    std::shared_ptr<client_channel_pipeline_mock> pipeline_client = std::make_shared<client_channel_pipeline_mock>(m_pipeline_context->context());
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
    
    auto factory = std::make_shared<client_bootstrap_initializer_factory_mock>();
    auto initializer = std::make_shared<client_bootstrap_initializer_mock>();
    
    // 4. expect the call of factory->new_initializer() to return initializer.
    EXPECT_CALL(*factory, new_initializer()).Times(testing::AnyNumber()).WillRepeatedly(testing::Return(initializer));
    
    std::vector<std::shared_ptr<client_bootstrap>> clients;
    int max_count = 10;
    int added_count = 0;
    int removed_count = 0;
    int refused_count = 0;
    
    ASSERT_TRUE(wait_for(g_wait_interval, [max_count, client_handler, &added_count](std::promise<void> &go){
        
        client_handler->set_read_handler(lockless_data::pipeline_added,[max_count, &go, &added_count](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>) {
            auto real = channel_data::convert<pipeline_added>(_data);
            added_count++;
            if (max_count == added_count) {
                go.set_value();
            }
        });
        
    }, [this, &refused_count, max_count, factory, initializer, &clients](std::promise<void> &go) {
        
        auto h1_handler = std::make_shared<channel_handler>();
        h1_handler->set_read_handler(lockless_data::connect_status, [&refused_count, &go, max_count](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>){
            auto real = channel_data::convert<connect_status>(_data);
            if (real->get_status() == connect_status::connect_refused) {
                refused_count++;
                if (max_count == refused_count) {
                    go.set_value();
                }
            }
        });
        EXPECT_CALL(*initializer, init_channel(testing::_)).Times(testing::AnyNumber()).WillRepeatedly(action_client_init_channel(m_pipeline_context, h1_handler, "h1"));
        
        for(int i = 0; i < max_count; ++i) {
            auto client = std::make_shared<client_bootstrap>(factory);
            clients.push_back(client);
            asio::ip::tcp::endpoint ep(asio::ip::address::from_string("127.0.0.1"), 9998);
            client->connect(ep);
        }
    }));
    
    ASSERT_TRUE(wait_for(g_wait_interval, [&clients, &client_handler, max_count, &removed_count](std::promise<void> &go){
        client_handler->set_read_handler(lockless_data::pipeline_removed, [&removed_count, &go, max_count](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>) {
            auto real = channel_data::convert<pipeline_removed>(_data);
            removed_count++;
            if (max_count == removed_count) {
                go.set_value();
            }
        });
        for (int i = 0; i < clients.size(); i++) {
            std::shared_ptr<client_bootstrap> c = clients[i];
            c->disconnect();
        }
    }));
    
    testing::Mock::AllowLeak(initializer.get());
    testing::Mock::AllowLeak(factory.get());
}

