//
//  channel_pipeline_context_test.cpp
//  lockless
//
//  Created by liuhaibo on 14-11-27.
//  Copyright (c) 2014年 liuhaibo. All rights reserved.
//

#include "channel_pipeline_context_test.h"
#include "lockless_test_utils.h"

USING_LOCKLESS_NAMESPACE

void channel_pipeline_context_test::SetUpTestCase()
{
    
}

void channel_pipeline_context_test::TearDownTestCase()
{
    
}

void channel_pipeline_context_test::SetUp()
{
    g_object_count = 0;
    m_executor = std::make_shared<executor>(GEN_UUID, 1);
    m_runtime_context = std::make_shared<runtime_context>(m_executor->worker_service());
    
    std::shared_ptr<object> monitor(new object);
    ASSERT_TRUE(wait_for(g_wait_interval, [monitor, this](std::promise<void> &go) {
        m_executor->ps_started.subscribe(monitor, m_runtime_context, [&go](std::shared_ptr<object> _subscriber, int _index) {
            go.set_value();
        });
        m_executor->startup();
    }));
    
    m_pipeline_context = std::make_shared<channel_pipeline_context>();
    ASSERT_TRUE(wait_for(g_wait_interval, [monitor, this](std::promise<void> &go) {
        m_pipeline_context->ps_started.subscribe(monitor, m_pipeline_context->context(), [&go, monitor](std::shared_ptr<object> _subscriber) {
            ASSERT_EQ(monitor, _subscriber);
            go.set_value();
        });
        m_pipeline_context->startup();
    }));
}

void channel_pipeline_context_test::TearDown()
{
    m_executor->shutdown();
    m_executor->syncronize();
    m_executor.reset();
    
    m_pipeline_context->shutdown();
    m_pipeline_context->synchronize();
    m_pipeline_context.reset();
    
    m_runtime_context.reset();
    
    pipeline_a.reset();
    pipeline_a1.reset();
    pipeline_a2.reset();
    pipeline_a3.reset();
    pipeline_b1.reset();
    pipeline_b2.reset();
    pipeline_b3.reset();
    pipeline_c.reset();
    pipeline_c1.reset();
    pipeline_test.reset();
    
    ASSERT_EQ(g_object_count, 0);
}

void channel_pipeline_context_test::preparePipelineContext()
{
    pipeline_a = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    pipeline_a1 = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    pipeline_a2 = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    pipeline_a3 = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    pipeline_b1 = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    pipeline_b2 = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    pipeline_b3 = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    pipeline_c = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    pipeline_c1 = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    pipeline_test = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    
    std::promise<void> go_a;
    std::promise<void> go_a1;
    std::promise<void> go_a2;
    std::promise<void> go_a3;
    std::promise<void> go_b1;
    std::promise<void> go_b2;
    std::promise<void> go_b3;
    std::promise<void> go_c;
    std::promise<void> go_c1;
    std::promise<void> go_test;
    
    int order = 0;
    
    pipeline_a->set_name("com.test.a");
    m_pipeline_context->post_add_pipeline(pipeline_a, [&go_a, &order](std::future<bool> _fut)
                                          {
                                              ASSERT_EQ(order++, 0);
                                              ASSERT_TRUE(_fut.get());
                                              go_a.set_value();
                                          });
    
    pipeline_a1->set_name("com.test.a.1");
    m_pipeline_context->post_add_pipeline(pipeline_a1, [&go_a1, &order](std::future<bool> _fut)
                                          {
                                              ASSERT_EQ(order++, 1);
                                              ASSERT_TRUE(_fut.get());
                                              go_a1.set_value();
                                          });
    
    pipeline_a2->set_name("com.test.a.2");
    m_pipeline_context->post_add_pipeline(pipeline_a2, [&go_a2, &order](std::future<bool> _fut)
                                          {
                                              ASSERT_EQ(order++, 2);
                                              ASSERT_TRUE(_fut.get());
                                              go_a2.set_value();
                                          });
    
    pipeline_a3->set_name("com.test.a.3");
    m_pipeline_context->post_add_pipeline(pipeline_a3, [&go_a3, &order](std::future<bool> _fut)
                                          {
                                              ASSERT_EQ(order++, 3);
                                              ASSERT_TRUE(_fut.get());
                                              go_a3.set_value();
                                          });
    
    pipeline_b1->set_name("com.test.b.1");
    m_pipeline_context->post_add_pipeline(pipeline_b1, [&go_b1, &order](std::future<bool> _fut)
                                          {
                                              ASSERT_EQ(order++, 4);
                                              ASSERT_TRUE(_fut.get());
                                              go_b1.set_value();
                                          });
    
    pipeline_b2->set_name("com.test.b.2");
    m_pipeline_context->post_add_pipeline(pipeline_b2, [&go_b2, &order](std::future<bool> _fut)
                                          {
                                              ASSERT_EQ(order++, 5);
                                              ASSERT_TRUE(_fut.get());
                                              go_b2.set_value();
                                          });
    
    pipeline_b3->set_name("com.test.b.3");
    m_pipeline_context->post_add_pipeline(pipeline_b3, [&go_b3, &order](std::future<bool> _fut)
                                          {
                                              ASSERT_EQ(order++, 6);
                                              ASSERT_TRUE(_fut.get());
                                              go_b3.set_value();
                                          });
    
    pipeline_c->set_name("com.test.c");
    m_pipeline_context->post_add_pipeline(pipeline_c, [&go_c, &order](std::future<bool> _fut)
                                          {
                                              ASSERT_EQ(order++, 7);
                                              ASSERT_TRUE(_fut.get());
                                              go_c.set_value();
                                          });
    
    pipeline_c1->set_name("com.test.c.1");
    m_pipeline_context->post_add_pipeline(pipeline_c1, [&go_c1, &order](std::future<bool> _fut)
                                          {
                                              ASSERT_EQ(order++, 8);
                                              ASSERT_TRUE(_fut.get());
                                              go_c1.set_value();
                                          });
    
    pipeline_test->set_name("com.test");
    m_pipeline_context->post_add_pipeline(pipeline_test, [&go_test, &order](std::future<bool> _fut)
                                          {
                                              ASSERT_EQ(order++, 9);
                                              ASSERT_TRUE(_fut.get());
                                              go_test.set_value();
                                          });
    
    ASSERT_TRUE(wait_for(g_wait_interval, go_a));
    ASSERT_TRUE(wait_for(g_wait_interval, go_a1));
    ASSERT_TRUE(wait_for(g_wait_interval, go_a2));
    ASSERT_TRUE(wait_for(g_wait_interval, go_a3));
    ASSERT_TRUE(wait_for(g_wait_interval, go_b1));
    ASSERT_TRUE(wait_for(g_wait_interval, go_b2));
    ASSERT_TRUE(wait_for(g_wait_interval, go_b3));
    ASSERT_TRUE(wait_for(g_wait_interval, go_c));
    ASSERT_TRUE(wait_for(g_wait_interval, go_c1));
    ASSERT_TRUE(wait_for(g_wait_interval, go_test));
    
    std::string description = m_pipeline_context->to_string();
    std::stringstream ss;
    ss<<"root"<<"\n";
    ss<<"\tcom"<<"\n";
    ss<<"\t\t"<<"test(pipeline)"<<"\n";
    ss<<"\t\t\t"<<"a(pipeline)"<<"\n";
    ss<<"\t\t\t\t"<<"1(pipeline)"<<"\n";
    ss<<"\t\t\t\t"<<"2(pipeline)"<<"\n";
    ss<<"\t\t\t\t"<<"3(pipeline)"<<"\n";
    ss<<"\t\t\t"<<"b"<<"\n";
    ss<<"\t\t\t\t"<<"1(pipeline)"<<"\n";
    ss<<"\t\t\t\t"<<"2(pipeline)"<<"\n";
    ss<<"\t\t\t\t"<<"3(pipeline)"<<"\n";
    ss<<"\t\t\t"<<"c(pipeline)"<<"\n";
    ss<<"\t\t\t\t"<<"1(pipeline)"<<"\n";
    
    ASSERT_EQ(ss.str(), description);
}
