//
//  handle_write_data.cpp
//  lockless
//
//  Created by liuhaibo on 14-11-26.
//  Copyright (c) 2014年 liuhaibo. All rights reserved.
//

#include "channel_pipeline_test.h"
#include "lockless_test_utils.h"

/**
 * install three handlers and named "h1", "h2", "h3"
 * go through "h3", "h2", consumed by "h1"
 */
TEST_F(channel_pipeline_test, handle_write_data_1)
{
    auto h1 = std::make_shared<channel_handler>();
    ASSERT_TRUE(wait_for(g_wait_interval, [h1, this](std::promise<void> &go){
        m_pipeline->post_add_channel_handler(h1, "h1", false, [&go, this, h1](std::future<ret_map> _fut) {
            go.set_value();
        });
    }));
    
    auto h2 = std::make_shared<channel_handler>();
    ASSERT_TRUE(wait_for(g_wait_interval, [h2, this](std::promise<void> &go){
        m_pipeline->post_add_channel_handler(h2, "h2", false, [&go, this, h2](std::future<ret_map> _fut) {
            go.set_value();
        });
    }));
    
    auto h3 = std::make_shared<channel_handler>();
    ASSERT_TRUE(wait_for(g_wait_interval, [h3, this](std::promise<void> &go){
        m_pipeline->post_add_channel_handler(h3, "h3", false, [&go, this, h3](std::future<ret_map> _fut) {
            go.set_value();
        });
    }));
    
    std::promise<void> go1;
    std::promise<void> go2;
    std::promise<void> go3;
    std::promise<void> go4;
    
    std::weak_ptr<channel_handler> weak_h1 = h1;
    h1->set_write_handler(lockless_data::socket_stream, [weak_h1, &go1](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info){
//        h1->get_handler_context()->next_write_data(_data, _info);
        go1.set_value();
    });
    
    std::weak_ptr<channel_handler> weak_h2 = h2;
    h2->set_write_handler(lockless_data::socket_stream, [weak_h2, &go2](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info){
        weak_h2.lock()->get_handler_context()->next_write_data(_data, _info);
        go2.set_value();
    });
    
    std::weak_ptr<channel_handler> weak_h3 = h3;
    h3->set_write_handler(lockless_data::socket_stream, [weak_h3, &go3](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info){
        weak_h3.lock()->get_handler_context()->next_write_data(_data, _info);
        go3.set_value();
    });
    
    std::shared_ptr<object> subscriber = std::make_shared<object>();
    m_pipeline->ps_write_data.subscribe(subscriber,
                                       m_runtime_context,
                                       [&go4](std::shared_ptr<lockless::object>, std::shared_ptr<lockless::channel_data>)
                                       {
                                           go4.set_value();
                                       });
    
    std::shared_ptr<channel_data> data(new channel_data(lockless_data::socket_stream));
    ASSERT_TRUE(wait_for(g_wait_interval, [data, this](std::promise<void>&go){
        m_pipeline->post_handle_write_data(data, nullptr, [&go](std::future<void> _fut){
            go.set_value();
        });
    }));
    
    ASSERT_TRUE(wait_for(g_wait_interval, go3));
    ASSERT_TRUE(wait_for(g_wait_interval, go2));
    ASSERT_TRUE(wait_for(g_wait_interval, go1));
    ASSERT_TRUE(wait_until(g_wait_interval, go4));
}

/**
 * install three handlers and named "h1", "h2", "h3"
 * go through "h3", consumed by "h2"
 */
TEST_F(channel_pipeline_test, handle_write_data_2)
{
    auto h1 = std::make_shared<channel_handler>();
    ASSERT_TRUE(wait_for(g_wait_interval, [h1, this](std::promise<void> &go){
        m_pipeline->post_add_channel_handler(h1, "h1", false, [&go, this, h1](std::future<ret_map> _fut) {
            go.set_value();
        });
    }));
    
    auto h2 = std::make_shared<channel_handler>();
    ASSERT_TRUE(wait_for(g_wait_interval, [h2, this](std::promise<void> &go){
        m_pipeline->post_add_channel_handler(h2, "h2", false, [&go, this, h2](std::future<ret_map> _fut) {
            go.set_value();
        });
    }));
    
    auto h3 = std::make_shared<channel_handler>();
    ASSERT_TRUE(wait_for(g_wait_interval, [h3, this](std::promise<void> &go){
        m_pipeline->post_add_channel_handler(h3, "h3", false, [&go, this, h3](std::future<ret_map> _fut) {
            go.set_value();
        });
    }));
    
    std::promise<void> go1;
    std::promise<void> go2;
    std::promise<void> go3;
    std::promise<void> go4;
    
    std::weak_ptr<channel_handler> weak_h1 = h1;
    h1->set_write_handler(lockless_data::socket_stream, [weak_h1, &go1](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info){
        weak_h1.lock()->get_handler_context()->next_write_data(_data, _info);
        go1.set_value();
    });
    
    std::weak_ptr<channel_handler> weak_h2 = h2;
    h2->set_write_handler(lockless_data::socket_stream, [weak_h2, &go2](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info){
//        h2->get_handler_context()->next_write_data(_data, _info);
        go2.set_value();
    });
    
    std::weak_ptr<channel_handler> weak_h3 = h3;
    h3->set_write_handler(lockless_data::socket_stream, [weak_h3, &go3](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info){
        weak_h3.lock()->get_handler_context()->next_write_data(_data, _info);
        go3.set_value();
    });
    
    std::shared_ptr<object> subscriber = std::make_shared<object>();
    m_pipeline->ps_write_data.subscribe(subscriber,
                                        m_runtime_context,
                                        [&go4](std::shared_ptr<lockless::object>, std::shared_ptr<lockless::channel_data>)
                                        {
                                            go4.set_value();
                                        });
    
    std::shared_ptr<channel_data> data(new channel_data(lockless_data::socket_stream));
    ASSERT_TRUE(wait_for(g_wait_interval, [data, this](std::promise<void>&go){
        m_pipeline->post_handle_write_data(data, nullptr, [&go](std::future<void> _fut){
            go.set_value();
        });
    }));
    
    ASSERT_TRUE(wait_for(g_wait_interval, go3));
    ASSERT_TRUE(wait_for(g_wait_interval, go2));
    ASSERT_TRUE(wait_until(g_wait_interval, go1));
    ASSERT_TRUE(wait_until(g_wait_interval, go4));
}

/**
 * install three handlers and named "h1", "h2", "h3"
 * consumed by "h3"
 */
TEST_F(channel_pipeline_test, handle_write_data_3)
{
    auto h1 = std::make_shared<channel_handler>();
    ASSERT_TRUE(wait_for(g_wait_interval, [h1, this](std::promise<void> &go){
        m_pipeline->post_add_channel_handler(h1, "h1", false, [&go, this, h1](std::future<ret_map> _fut) {
            go.set_value();
        });
    }));
    
    auto h2 = std::make_shared<channel_handler>();
    ASSERT_TRUE(wait_for(g_wait_interval, [h2, this](std::promise<void> &go){
        m_pipeline->post_add_channel_handler(h2, "h2", false, [&go, this, h2](std::future<ret_map> _fut) {
            go.set_value();
        });
    }));
    
    auto h3 = std::make_shared<channel_handler>();
    ASSERT_TRUE(wait_for(g_wait_interval, [h3, this](std::promise<void> &go){
        m_pipeline->post_add_channel_handler(h3, "h3", false, [&go, this, h3](std::future<ret_map> _fut) {
            go.set_value();
        });
    }));
    
    std::promise<void> go1;
    std::promise<void> go2;
    std::promise<void> go3;
    std::promise<void> go4;
    
    std::weak_ptr<channel_handler> weak_h1 = h1;
    h1->set_write_handler(lockless_data::socket_stream, [weak_h1, &go1](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info){
        weak_h1.lock()->get_handler_context()->next_write_data(_data, _info);
        go1.set_value();
    });
    
    std::weak_ptr<channel_handler> weak_h2 = h2;
    h2->set_write_handler(lockless_data::socket_stream, [weak_h2, &go2](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info){
        weak_h2.lock()->get_handler_context()->next_write_data(_data, _info);
        go2.set_value();
    });
    
    std::weak_ptr<channel_handler> weak_h3 = h3;
    h3->set_write_handler(lockless_data::socket_stream, [weak_h3, &go3](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info){
//        h3->get_handler_context()->next_write_data(_data, _info);
        go3.set_value();
    });
    
    std::shared_ptr<object> subscriber = std::make_shared<object>();
    m_pipeline->ps_write_data.subscribe(subscriber,
                                        m_runtime_context,
                                        [&go4](std::shared_ptr<lockless::object>, std::shared_ptr<lockless::channel_data>)
                                        {
                                            go4.set_value();
                                        });
    
    std::shared_ptr<channel_data> data(new channel_data(lockless_data::socket_stream));
    ASSERT_TRUE(wait_for(g_wait_interval, [data, this](std::promise<void>&go){
        m_pipeline->post_handle_write_data(data, nullptr, [&go](std::future<void> _fut){
            go.set_value();
        });
    }));
    
    ASSERT_TRUE(wait_for(g_wait_interval, go3));
    ASSERT_TRUE(wait_until(g_wait_interval, go2));
    ASSERT_TRUE(wait_until(g_wait_interval, go1));
    ASSERT_TRUE(wait_until(g_wait_interval, go4));
}

/**
 * install three handlers and named "h1", "h2", "h3"
 * go through all the handlers.
 */
TEST_F(channel_pipeline_test, handle_write_data_4)
{
    auto h1 = std::make_shared<channel_handler>();
    ASSERT_TRUE(wait_for(g_wait_interval, [h1, this](std::promise<void> &go){
        m_pipeline->post_add_channel_handler(h1, "h1", false, [&go, this, h1](std::future<ret_map> _fut) {
            go.set_value();
        });
    }));
    
    auto h2 = std::make_shared<channel_handler>();
    ASSERT_TRUE(wait_for(g_wait_interval, [h2, this](std::promise<void> &go){
        m_pipeline->post_add_channel_handler(h2, "h2", false, [&go, this, h2](std::future<ret_map> _fut) {
            go.set_value();
        });
    }));
    
    auto h3 = std::make_shared<channel_handler>();
    ASSERT_TRUE(wait_for(g_wait_interval, [h3, this](std::promise<void> &go){
        m_pipeline->post_add_channel_handler(h3, "h3", false, [&go, this, h3](std::future<ret_map> _fut) {
            go.set_value();
        });
    }));
    
    std::promise<void> go1;
    std::promise<void> go2;
    std::promise<void> go3;
    std::promise<void> go4;
    
    std::weak_ptr<channel_handler> weak_h1 = h1;
    h1->set_write_handler(lockless_data::socket_stream, [weak_h1, &go1](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info){
        weak_h1.lock()->get_handler_context()->next_write_data(_data, _info);
        go1.set_value();
    });
    
    std::weak_ptr<channel_handler> weak_h2 = h2;
    h2->set_write_handler(lockless_data::socket_stream, [weak_h2, &go2](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info){
        weak_h2.lock()->get_handler_context()->next_write_data(_data, _info);
        go2.set_value();
    });
    
    std::weak_ptr<channel_handler> weak_h3 = h3;
    h3->set_write_handler(lockless_data::socket_stream, [weak_h3, &go3](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info){
        weak_h3.lock()->get_handler_context()->next_write_data(_data, _info);
        go3.set_value();
    });
    
    std::shared_ptr<object> subscriber = std::make_shared<object>();
    m_pipeline->ps_write_data.subscribe(subscriber,
                                        m_runtime_context,
                                        [&go4](std::shared_ptr<lockless::object>, std::shared_ptr<lockless::channel_data>)
                                        {
                                            go4.set_value();
                                        });
    
    std::shared_ptr<channel_data> data(new channel_data(lockless_data::socket_stream));
    ASSERT_TRUE(wait_for(g_wait_interval, [data, this](std::promise<void>&go){
        m_pipeline->post_handle_write_data(data, nullptr, [&go](std::future<void> _fut){
            go.set_value();
        });
    }));
    
    ASSERT_TRUE(wait_for(g_wait_interval, go3));
    ASSERT_TRUE(wait_for(g_wait_interval, go2));
    ASSERT_TRUE(wait_for(g_wait_interval, go1));
    ASSERT_TRUE(wait_for(g_wait_interval, go4));
}

