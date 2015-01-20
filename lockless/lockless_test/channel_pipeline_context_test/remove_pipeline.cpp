//
//  remove_pipeline.cpp
//  lockless
//
//  Created by liuhaibo on 14-11-28.
//  Copyright (c) 2014年 liuhaibo. All rights reserved.
//

#include "channel_pipeline_context_test.h"
#include "lockless_test_utils.h"
#include <system_error>

TEST_F(channel_pipeline_context_test, remove_pipeline)
{
    std::shared_ptr<channel_pipeline_mock> pipeline_a = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    std::shared_ptr<channel_pipeline_mock> pipeline_a1 = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    std::shared_ptr<channel_pipeline_mock> pipeline_a2 = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    std::shared_ptr<channel_pipeline_mock> pipeline_b = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    std::shared_ptr<channel_pipeline_mock> pipeline_b1 = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    std::shared_ptr<channel_pipeline_mock> pipeline_b2 = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    std::shared_ptr<channel_pipeline_mock> pipeline_c = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    std::shared_ptr<channel_pipeline_mock> pipeline_c1 = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    std::shared_ptr<channel_pipeline_mock> pipeline_c2 = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    
    auto hc = std::make_shared<channel_handler>();
    pipeline_c->post_add_channel_handler(hc, "hc", false, nullptr);
    
    std::promise<void> go_a;
    std::promise<void> go_a1;
    std::promise<void> go_a2;
    std::promise<void> go_b;
    std::promise<void> go_b1;
    std::promise<void> go_b2;
    std::promise<void> go_c;
    std::promise<void> go_c1;
    std::promise<void> go_c2;
    
    int order = 0;
    pipeline_a->set_name("com.test.a");
    m_pipeline_context->post_add_pipeline(pipeline_a, [&go_a, &order](std::future<bool>)
                                          {
                                              ASSERT_EQ(order++, 0);
                                              go_a.set_value();
                                          });
    
    pipeline_a1->set_name("com.test.a.1");
    m_pipeline_context->post_add_pipeline(pipeline_a1, [&go_a1, &order](std::future<bool>)
                                          {
                                              ASSERT_EQ(order++, 1);
                                              go_a1.set_value();
                                          });
    
    pipeline_a2->set_name("com.test.a.2");
    m_pipeline_context->post_add_pipeline(pipeline_a2, [&go_a2, &order](std::future<bool>)
                                          {
                                              ASSERT_EQ(order++, 2);
                                              go_a2.set_value();
                                          });
    pipeline_b->set_name("com.test.b");
    m_pipeline_context->post_add_pipeline(pipeline_b, [&go_b, &order](std::future<bool>)
                                          {
                                              ASSERT_EQ(order++, 3);
                                              go_b.set_value();
                                          });
    pipeline_b1->set_name("com.test.b.1");
    m_pipeline_context->post_add_pipeline(pipeline_b1, [&go_b1, &order](std::future<bool>)
                                          {
                                              ASSERT_EQ(order++, 4);
                                              go_b1.set_value();
                                          });
    pipeline_b2->set_name("com.test.b.2");
    m_pipeline_context->post_add_pipeline(pipeline_b2, [&go_b2, &order](std::future<bool>)
                                          {
                                              ASSERT_EQ(order++, 5);
                                              go_b2.set_value();
                                          });
    pipeline_c->set_name("com.test.c");
    m_pipeline_context->post_add_pipeline(pipeline_c, [&go_c, &order](std::future<bool>)
                                          {
                                              ASSERT_EQ(order++, 6);
                                              go_c.set_value();
                                          });
    pipeline_c1->set_name("com.test.c.1");
    m_pipeline_context->post_add_pipeline(pipeline_c1, [&go_c1, &order](std::future<bool>)
                                          {
                                              ASSERT_EQ(order++, 7);
                                              go_c1.set_value();
                                          });
    pipeline_c2->set_name("com.test.c.2");
    m_pipeline_context->post_add_pipeline(pipeline_c2, [&go_c2, &order](std::future<bool>)
                                          {
                                              ASSERT_EQ(order++, 8);
                                              go_c2.set_value();
                                          });
    
    ASSERT_TRUE(wait_for(g_wait_interval, go_a));
    ASSERT_TRUE(wait_for(g_wait_interval, go_a1));
    ASSERT_TRUE(wait_for(g_wait_interval, go_a2));
    ASSERT_TRUE(wait_for(g_wait_interval, go_b));
    ASSERT_TRUE(wait_for(g_wait_interval, go_b1));
    ASSERT_TRUE(wait_for(g_wait_interval, go_b2));
    ASSERT_TRUE(wait_for(g_wait_interval, go_c));
    ASSERT_TRUE(wait_for(g_wait_interval, go_c1));
    ASSERT_TRUE(wait_for(g_wait_interval, go_c2));
    
    {
        std::stringstream ss;
        ss<<"root"<<"\n";
        ss<<"\t"<<"com"<<"\n";
        ss<<"\t\t"<<"test"<<"\n";
        ss<<"\t\t\t"<<"a(pipeline)"<<"\n";
        ss<<"\t\t\t\t"<<"1(pipeline)"<<"\n";
        ss<<"\t\t\t\t"<<"2(pipeline)"<<"\n";
        ss<<"\t\t\t"<<"b(pipeline)"<<"\n";
        ss<<"\t\t\t\t"<<"1(pipeline)"<<"\n";
        ss<<"\t\t\t\t"<<"2(pipeline)"<<"\n";
        ss<<"\t\t\t"<<"c(pipeline)"<<"\n";
        ss<<"\t\t\t\t"<<"1(pipeline)"<<"\n";
        ss<<"\t\t\t\t"<<"2(pipeline)"<<"\n";
        
        ASSERT_EQ(ss.str(), m_pipeline_context->to_string());
    }
    
    std::promise<void> go_r_a;
    std::promise<void> go_r_a1;
    std::promise<void> go_r_a2;
    std::promise<void> go_r_b;
    std::promise<void> go_r_b1;
    std::promise<void> go_r_b2;
    std::promise<void> go_r_c;
    std::promise<void> go_r_c1;
    std::promise<void> go_r_c2;
    
    order = 0;
    
    m_pipeline_context->post_remove_pipeline("com.test.b", [&go_r_b, &order](std::future<bool>)
                                             {
                                                 ASSERT_EQ(order++, 0);
                                                 go_r_b.set_value();
                                             });
    ASSERT_TRUE(wait_for(g_wait_interval, go_r_b));
    
    {
        std::stringstream ss;
        ss<<"root"<<"\n";
        ss<<"\t"<<"com"<<"\n";
        ss<<"\t\t"<<"test"<<"\n";
        ss<<"\t\t\t"<<"a(pipeline)"<<"\n";
        ss<<"\t\t\t\t"<<"1(pipeline)"<<"\n";
        ss<<"\t\t\t\t"<<"2(pipeline)"<<"\n";
        ss<<"\t\t\t"<<"b"<<"\n";
        ss<<"\t\t\t\t"<<"1(pipeline)"<<"\n";
        ss<<"\t\t\t\t"<<"2(pipeline)"<<"\n";
        ss<<"\t\t\t"<<"c(pipeline)"<<"\n";
        ss<<"\t\t\t\t"<<"1(pipeline)"<<"\n";
        ss<<"\t\t\t\t"<<"2(pipeline)"<<"\n";
        
        ASSERT_EQ(ss.str(), m_pipeline_context->to_string());
    }
    
    m_pipeline_context->post_remove_pipeline("com.test.b.2", [&go_r_b2, &order](std::future<bool>)
                                             {
                                                 ASSERT_EQ(order++, 1);
                                                 go_r_b2.set_value();
                                             });
    ASSERT_TRUE(wait_for(g_wait_interval, go_r_b2));
    
    {
        std::stringstream ss;
        ss<<"root"<<"\n";
        ss<<"\t"<<"com"<<"\n";
        ss<<"\t\t"<<"test"<<"\n";
        ss<<"\t\t\t"<<"a(pipeline)"<<"\n";
        ss<<"\t\t\t\t"<<"1(pipeline)"<<"\n";
        ss<<"\t\t\t\t"<<"2(pipeline)"<<"\n";
        ss<<"\t\t\t"<<"b"<<"\n";
        ss<<"\t\t\t\t"<<"1(pipeline)"<<"\n";
        ss<<"\t\t\t"<<"c(pipeline)"<<"\n";
        ss<<"\t\t\t\t"<<"1(pipeline)"<<"\n";
        ss<<"\t\t\t\t"<<"2(pipeline)"<<"\n";
        
        ASSERT_EQ(ss.str(), m_pipeline_context->to_string());
    }
    
    m_pipeline_context->post_remove_pipeline("com.test.b.1", [&go_r_b1, &order](std::future<bool>)
                                             {
                                                 ASSERT_EQ(order++, 2);
                                                 go_r_b1.set_value();
                                             });
    ASSERT_TRUE(wait_for(g_wait_interval, go_r_b1));
    
    {
        std::stringstream ss;
        ss<<"root"<<"\n";
        ss<<"\t"<<"com"<<"\n";
        ss<<"\t\t"<<"test"<<"\n";
        ss<<"\t\t\t"<<"a(pipeline)"<<"\n";
        ss<<"\t\t\t\t"<<"1(pipeline)"<<"\n";
        ss<<"\t\t\t\t"<<"2(pipeline)"<<"\n";
        ss<<"\t\t\t"<<"c(pipeline)"<<"\n";
        ss<<"\t\t\t\t"<<"1(pipeline)"<<"\n";
        ss<<"\t\t\t\t"<<"2(pipeline)"<<"\n";
        
        ASSERT_EQ(ss.str(), m_pipeline_context->to_string());
    }
    
    int c_remove_order = 0;
    std::promise<void> go_c_h;
    hc->set_read_handler(lockless_data::pipeline_removed, [&c_remove_order, &go_c_h](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info)
                         {
                             ASSERT_EQ(_data->get_type(), lockless_data::pipeline_removed);
                             auto real = channel_data::convert<pipeline_removed>(_data);
                             if (c_remove_order == 0) {
                                 ASSERT_EQ(real->get_pipeline_name(), "com.test.c.1");
                             }
                             else if (c_remove_order == 1) {
                                 ASSERT_EQ(real->get_pipeline_name(), "com.test.c.2");
                                 go_c_h.set_value();
                             }
                             else {
                                 ASSERT_TRUE(false);
                             }
                             c_remove_order++;
                         });
    m_pipeline_context->post_remove_pipeline("com.test.c.1", [&go_r_c1, &order](std::future<bool>)
                                             {
                                                 ASSERT_EQ(order++, 3);
                                                 go_r_c1.set_value();
                                             });
    ASSERT_TRUE(wait_for(g_wait_interval, go_r_c1));
    {
        std::stringstream ss;
        ss<<"root"<<"\n";
        ss<<"\t"<<"com"<<"\n";
        ss<<"\t\t"<<"test"<<"\n";
        ss<<"\t\t\t"<<"a(pipeline)"<<"\n";
        ss<<"\t\t\t\t"<<"1(pipeline)"<<"\n";
        ss<<"\t\t\t\t"<<"2(pipeline)"<<"\n";
        ss<<"\t\t\t"<<"c(pipeline)"<<"\n";
        ss<<"\t\t\t\t"<<"2(pipeline)"<<"\n";
        
        ASSERT_EQ(ss.str(), m_pipeline_context->to_string());
    }
    
    m_pipeline_context->post_remove_pipeline("com.test.c.2", [&go_r_c2, &order](std::future<bool>)
                                             {
                                                 ASSERT_EQ(order++, 4);
                                                 go_r_c2.set_value();
                                             });
    ASSERT_TRUE(wait_for(g_wait_interval, go_r_c2));
    {
        std::stringstream ss;
        ss<<"root"<<"\n";
        ss<<"\t"<<"com"<<"\n";
        ss<<"\t\t"<<"test"<<"\n";
        ss<<"\t\t\t"<<"a(pipeline)"<<"\n";
        ss<<"\t\t\t\t"<<"1(pipeline)"<<"\n";
        ss<<"\t\t\t\t"<<"2(pipeline)"<<"\n";
        ss<<"\t\t\t"<<"c(pipeline)"<<"\n";
        
        ASSERT_EQ(ss.str(), m_pipeline_context->to_string());
    }
    
    m_pipeline_context->post_remove_pipeline("com.test.c", [&go_r_c, &order](std::future<bool>)
                                             {
                                                 ASSERT_EQ(order++, 5);
                                                 go_r_c.set_value();
                                             });
    ASSERT_TRUE(wait_for(g_wait_interval, go_r_c));
    {
        std::stringstream ss;
        ss<<"root"<<"\n";
        ss<<"\t"<<"com"<<"\n";
        ss<<"\t\t"<<"test"<<"\n";
        ss<<"\t\t\t"<<"a(pipeline)"<<"\n";
        ss<<"\t\t\t\t"<<"1(pipeline)"<<"\n";
        ss<<"\t\t\t\t"<<"2(pipeline)"<<"\n";
        
        ASSERT_EQ(ss.str(), m_pipeline_context->to_string());
    }
    ASSERT_TRUE(wait_for(g_wait_interval, go_c_h));
    
    m_pipeline_context->post_remove_pipeline("com.test.a", [&go_r_a, &order](std::future<bool>)
                                             {
                                                 ASSERT_EQ(order++, 6);
                                                 go_r_a.set_value();
                                             });
    ASSERT_TRUE(wait_for(g_wait_interval, go_r_a));
    {
        std::stringstream ss;
        ss<<"root"<<"\n";
        ss<<"\t"<<"com"<<"\n";
        ss<<"\t\t"<<"test"<<"\n";
        ss<<"\t\t\t"<<"a"<<"\n";
        ss<<"\t\t\t\t"<<"1(pipeline)"<<"\n";
        ss<<"\t\t\t\t"<<"2(pipeline)"<<"\n";
        
        ASSERT_EQ(ss.str(), m_pipeline_context->to_string());
    }
    
    m_pipeline_context->post_remove_pipeline("com.test.a.1", [&go_r_a1, &order](std::future<bool>)
                                             {
                                                 ASSERT_EQ(order++, 7);
                                                 go_r_a1.set_value();
                                             });
    ASSERT_TRUE(wait_for(g_wait_interval, go_r_a1));
    {
        std::stringstream ss;
        ss<<"root"<<"\n";
        ss<<"\t"<<"com"<<"\n";
        ss<<"\t\t"<<"test"<<"\n";
        ss<<"\t\t\t"<<"a"<<"\n";
        ss<<"\t\t\t\t"<<"2(pipeline)"<<"\n";
        
        ASSERT_EQ(ss.str(), m_pipeline_context->to_string());
    }
    
    bool result = false;
    m_pipeline_context->post_remove_pipeline("com.test.a.2", [&result, &order, &go_r_a2](std::future<bool> _fut)
                                             {
                                                 ASSERT_EQ(order++, 8);
                                                 result = _fut.get();
                                                 go_r_a2.set_value();
                                             });
    
    ASSERT_TRUE(wait_for(g_wait_interval, go_r_a2));
    {
        std::stringstream ss;
        ss<<"root"<<"\n";
        ASSERT_TRUE(result);
        ASSERT_EQ(ss.str(), m_pipeline_context->to_string());
    }
    
    result = true;
    ASSERT_TRUE(wait_for(g_wait_interval, [this, &result](std::promise<void> &go)
    {
        m_pipeline_context->post_remove_pipeline("com.test.a.2", [&result, &go](std::future<bool> _fut)
        {
            result = _fut.get();
            go.set_value();
        });
    }));
    ASSERT_FALSE(result);
}
