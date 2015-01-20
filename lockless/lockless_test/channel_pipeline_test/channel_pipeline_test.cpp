//
//  channel_pipeline_test.cpp
//  lockless
//
//  Created by liuhaibo on 14-11-13.
//  Copyright (c) 2014年 liuhaibo. All rights reserved.
//

#include <boost/any.hpp>

#include "channel_pipeline_test.h"
#include "lockless_test_utils.h"

USING_LOCKLESS_NAMESPACE

MATCHER_P3(RouteInfoMatcher, from, to, wild, "")
{
    std::shared_ptr<router_info> info = (std::shared_ptr<router_info>)arg;
    return info->from == from && info->to == to && info->wild == wild;
}

void channel_pipeline_test::SetUpTestCase()
{
    
}

void channel_pipeline_test::TearDownTestCase()
{
    
}

void channel_pipeline_test::SetUp()
{
    g_object_count = 0;
    m_executor = std::make_shared<executor>(GEN_UUID, 1);
    m_runtime_context = std::make_shared<runtime_context>(m_executor->worker_service());
    m_pipeline = std::make_shared<channel_pipeline>(m_runtime_context);
    m_pipeline_context_mock = std::make_shared<channel_pipeline_context_mock>();

    std::shared_ptr<object> monitor(new object);
    ASSERT_TRUE(wait_for(g_wait_interval, [monitor, this](std::promise<void> &go) {
        m_executor->ps_started.subscribe(monitor, m_runtime_context, [&go](std::shared_ptr<object> _subscriber, int _index) {
             go.set_value();
        });
        m_executor->startup();
    }));
    
    ASSERT_TRUE(wait_for(g_wait_interval, [monitor, this](std::promise<void> &go) {
        m_pipeline_context_mock->ps_started.subscribe(monitor, m_runtime_context, [&go](std::shared_ptr<object> _subscriber) {
            go.set_value();
        });
        m_pipeline_context_mock->startup();
    }));

    typedef std::function<void(std::future<void>)> FuncType;
    EXPECT_CALL(*m_pipeline_context_mock, add_pipeline_posted(m_pipeline)).Times(1);
    ASSERT_TRUE(wait_for(g_wait_interval, [this](std::promise<void> &go) {
        m_pipeline->post_add_to_context(m_pipeline_context_mock, "com.test.a", [&go, this](std::future<ret_map> _fut) {
            ret_map ret = _fut.get();
            std::shared_ptr<channel_pipeline_context> context = boost::any_cast<std::shared_ptr<channel_pipeline_context>>(ret["context"]);
            std::string name = boost::any_cast<std::string>(ret["name"]);
            std::shared_ptr<channel_pipeline> pipeline = boost::any_cast<std::shared_ptr<channel_pipeline>>(ret["pipeline"]);
            
            GTEST_ASSERT_EQ(context, m_pipeline_context_mock);
            GTEST_ASSERT_EQ(name, "com.test.a");
            GTEST_ASSERT_EQ(pipeline, m_pipeline);
            go.set_value();
        });
    }));
}

void channel_pipeline_test::TearDown()
{
    wait_until(g_wait_interval);
    m_executor->shutdown();
    m_executor->syncronize();
    m_executor.reset();
    
    m_pipeline_context_mock->shutdown();
    m_pipeline_context_mock->synchronize();
    m_pipeline_context_mock.reset();
    
    m_runtime_context.reset();
    m_pipeline.reset();
    
    ASSERT_EQ(g_object_count, 0);
}

TEST_F(channel_pipeline_test, add_channel_handler)
{
    typedef std::function<void(std::future<void>)> FuncType;
    
    ASSERT_TRUE(wait_for(g_wait_interval, [this](std::promise<void> &go){
        auto handler = std::make_shared<channel_handler>();
        m_pipeline->post_add_channel_handler(handler, "h1", false, [&go, this, handler](std::future<ret_map> _fut) {
            
            ret_map ret = _fut.get();
            std::shared_ptr<channel_handler_context> hcontext = boost::any_cast<std::shared_ptr<channel_handler_context>>(ret["context"]);
            GTEST_ASSERT_EQ(hcontext->get_runtime_context(), m_runtime_context);
            GTEST_ASSERT_EQ(hcontext, handler->get_handler_context());
            go.set_value();
        });
    }));
    
    ASSERT_TRUE(wait_for(g_wait_interval, [this](std::promise<void> &go){
        auto handler = std::make_shared<channel_handler>();
        m_pipeline->post_add_channel_handler(handler, "h2", true, [&go, this, handler](std::future<ret_map> _fut) {
            
            ret_map ret = _fut.get();
            std::shared_ptr<channel_handler_context> hcontext = boost::any_cast<std::shared_ptr<channel_handler_context>>(ret["context"]);
            GTEST_ASSERT_NE(hcontext->get_runtime_context(), m_runtime_context);
            GTEST_ASSERT_EQ(hcontext, handler->get_handler_context());
            go.set_value();
        });
    }));
}


TEST_F(channel_pipeline_test, finish)
{
    ASSERT_TRUE(wait_for(g_wait_interval, [this](std::promise<void> &go){
        std::shared_ptr<object> subscriber = std::make_shared<object>();
        std::promise<void> cond1;
        std::promise<void> cond2;
        m_pipeline->ps_finish.subscribe(subscriber, m_runtime_context, [&cond1](std::shared_ptr<object> _subscriber)
        {
            cond1.set_value();
        });
        m_pipeline->post_finish([&cond2](std::future<void> _fut){
            cond2.set_value();
        });
        cond1.get_future().get();
        cond2.get_future().get();
        go.set_value();
    }));
}

TEST_F(channel_pipeline_test, rename)
{
    typedef std::function<void(std::future<void>)> FuncType;
    EXPECT_CALL(*m_pipeline_context_mock, remove_pipeline_posted("com.test.a")).Times(1);
    EXPECT_CALL(*m_pipeline_context_mock, remove_pipeline_posted("com.test.b")).Times(1);
    EXPECT_CALL(*m_pipeline_context_mock, add_pipeline_posted(m_pipeline)).Times(2);
    
    ASSERT_TRUE(wait_for(g_wait_interval, [this](std::promise<void> &go){
        
        m_pipeline->post_rename("com.test.b", [&go](std::future<ret_map> _fut){
            
            ret_map ret = _fut.get();
            std::string old_name = boost::any_cast<std::string>(ret["old_name"]);
            std::string new_name = boost::any_cast<std::string>(ret["new_name"]);
            GTEST_ASSERT_EQ(old_name, "com.test.a");
            go.set_value();
        });
        
    }));

    ASSERT_TRUE(wait_for(g_wait_interval, [this](std::promise<void> &go){
        
        m_pipeline->post_rename("com.test.a", [&go](std::future<ret_map> _fut){
            ret_map ret = _fut.get();
            std::string old_name = boost::any_cast<std::string>(ret["old_name"]);
            std::string new_name = boost::any_cast<std::string>(ret["new_name"]);
            GTEST_ASSERT_EQ(old_name, "com.test.b");
            go.set_value();
        });
        
    }));
}

TEST_F(channel_pipeline_test, handle_route_read_data)
{
    typedef std::function<void(std::future<void>)> FuncType;
    
    std::shared_ptr<channel_data> data(new channel_data(lockless_data::socket_stream));
    typedef std::shared_ptr<router_info> RouterInfo;
    EXPECT_CALL(*m_pipeline_context_mock,
                route_read_data_posted(data,
                                       testing::Matcher<RouterInfo>(RouteInfoMatcher("com.test.a", "com.test.b", router_info::wild_exactly))
                                       )
                ).Times(1);
    
    ASSERT_TRUE(wait_for(g_wait_interval, [data, this](std::promise<void> &go) {
        m_pipeline->post_handle_route_read_data(data, "com.test.b", router_info::wild_exactly, [&go](std::future<void> _fut){
            go.set_value();
        });
    }));
    
}

TEST_F(channel_pipeline_test, handle_route_write_data)
{
    typedef std::function<void(std::future<void>)> FuncType;
    
    std::shared_ptr<channel_data> data(new channel_data(lockless_data::socket_stream));
    typedef std::shared_ptr<router_info> RouterInfo;
    EXPECT_CALL(*m_pipeline_context_mock,
                route_write_data_posted(data,
                                     testing::Matcher<RouterInfo>(RouteInfoMatcher("com.test.a", "com.test.b", router_info::wild_exactly))
                                     )
                ).Times(1);
    
    ASSERT_TRUE(wait_for(g_wait_interval, [data, this](std::promise<void> &go) {
        m_pipeline->post_handle_route_write_data(data, "com.test.b", router_info::wild_exactly, [&go](std::future<void> _fut){
            go.set_value();
        });
    }));
    
}

TEST_F(channel_pipeline_test, publish_read_data)
{
    typedef std::function<void(std::future<void>)> FuncType;
    
    std::shared_ptr<channel_data> data(new channel_data(lockless_data::socket_stream));
    std::shared_ptr<object> subscriber = std::make_shared<object>();
    std::promise<void> go;
    m_pipeline->ps_read_data.subscribe(subscriber, m_runtime_context, [&go](std::shared_ptr<lockless::object>, std::shared_ptr<lockless::channel_data>){
        go.set_value();
    });
    ASSERT_TRUE(wait_for(g_wait_interval, [data, this](std::promise<void> &go) {
        m_pipeline->post_publish_read_data(data, [&go](std::future<void> _fut){
            go.set_value();
        });
    }));
    
    ASSERT_TRUE(wait_for(g_wait_interval, go));
}

TEST_F(channel_pipeline_test, publish_write_data)
{
    typedef std::function<void(std::future<void>)> FuncType;
    
    std::shared_ptr<channel_data> data(new channel_data(lockless_data::socket_stream));
    std::shared_ptr<object> subscriber = std::make_shared<object>();
    std::promise<void> go;
    m_pipeline->ps_write_data.subscribe(subscriber, m_runtime_context, [&go](std::shared_ptr<lockless::object>, std::shared_ptr<lockless::channel_data>){
        go.set_value();
    });
    ASSERT_TRUE(wait_for(g_wait_interval, [data, this](std::promise<void> &go) {
        m_pipeline->post_publish_write_data(data, [&go](std::future<void> _fut){
            go.set_value();
        });
    }));
    
    ASSERT_TRUE(wait_for(g_wait_interval, go));
}

