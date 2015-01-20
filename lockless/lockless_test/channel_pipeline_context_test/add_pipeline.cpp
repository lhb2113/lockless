//
//  add_pipeline.cpp
//  lockless
//
//  Created by liuhaibo on 14-11-28.
//  Copyright (c) 2014年 liuhaibo. All rights reserved.
//

#include "channel_pipeline_context_test.h"
#include "lockless_test_utils.h"

TEST_F(channel_pipeline_context_test, add_pipeline)
{
    std::shared_ptr<channel_pipeline_mock> pipeline_a = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    std::shared_ptr<channel_pipeline_mock> pipeline_a1 = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    std::shared_ptr<channel_pipeline_mock> pipeline_a2 = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    std::shared_ptr<channel_pipeline_mock> pipeline_b1 = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    std::shared_ptr<channel_pipeline_mock> pipeline_b2 = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    std::shared_ptr<channel_pipeline_mock> pipeline_c = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    std::shared_ptr<channel_pipeline_mock> pipeline_c1 = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    std::shared_ptr<channel_pipeline_mock> pipeline_test = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    std::shared_ptr<channel_pipeline_mock> pipeline_another_test = std::make_shared<channel_pipeline_mock>(m_runtime_context);
    
    std::promise<void> go_a;
    std::promise<void> go_a1;
    std::promise<void> go_a2;
    std::promise<void> go_b1;
    std::promise<void> go_b2;
    std::promise<void> go_c;
    std::promise<void> go_c1;
    std::promise<void> go_test;
    std::promise<void> go_same_test;
    std::promise<void> go_another_test;
    
    auto ha = std::make_shared<channel_handler>();
    pipeline_a->post_add_channel_handler(ha, "ha", false, nullptr);
    
    auto hc = std::make_shared<channel_handler>();
    pipeline_c->post_add_channel_handler(hc, "hc", false, nullptr);
    
    int a_added_order = 0;
    std::promise<void> go_a_h1;
    std::promise<void> go_a_h2;
    ha->set_read_handler(lockless_data::pipeline_added, [&a_added_order, &go_a_h1, &go_a_h2](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info)
                         {
                             ASSERT_EQ(_data->get_type(), lockless_data::pipeline_added);
                             auto real = channel_data::convert<pipeline_added>(_data);
                             if (a_added_order == 0) {
                                 ASSERT_EQ(real->get_pipeline_name(), "com.test.a.1");
                             }
                             else if (a_added_order == 1) {
                                 ASSERT_EQ(real->get_pipeline_name(), "com.test.a.2");
                                 go_a_h1.set_value();
                             }
                             else if (a_added_order == 3) {
                                 ASSERT_EQ(real->get_pipeline_name(), "com.test.a.1");
                                 go_a_h2.set_value();
                             }
                             else {
                                 ASSERT_TRUE(false);
                             }
                             a_added_order++;
                         });
    
    ha->set_read_handler(lockless_data::pipeline_removed, [&a_added_order](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info)
                         {
                             ASSERT_EQ(_data->get_type(), lockless_data::pipeline_removed);
                             auto real = channel_data::convert<pipeline_removed>(_data);
                             if (a_added_order == 2) {
                                 ASSERT_EQ(real->get_pipeline_name(), "com.test.a.1");
                             }
                             else {
                                 ASSERT_TRUE(false);
                             }
                             a_added_order++;
                         });
    
    int c_added_order = 0;
    std::promise<void> go_c_h;
    hc->set_read_handler(lockless_data::pipeline_added, [&c_added_order, &go_c_h](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info> _info)
                         {
                             ASSERT_EQ(_data->get_type(), lockless_data::pipeline_added);
                             auto real = channel_data::convert<pipeline_added>(_data);
                             if (c_added_order == 0) {
                                 ASSERT_EQ(real->get_pipeline_name(), "com.test.c.1");
                                 go_c_h.set_value();
                             }
                             else {
                                 ASSERT_TRUE(false);
                             }
                             c_added_order++;
                         });
    
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
    
    pipeline_b1->set_name("com.test.b.1");
    m_pipeline_context->post_add_pipeline(pipeline_b1, [&go_b1, &order](std::future<bool> _fut)
                                          {
                                              ASSERT_EQ(order++, 3);
                                              ASSERT_TRUE(_fut.get());
                                              go_b1.set_value();
                                          });
    
    pipeline_b2->set_name("com.test.b.2");
    m_pipeline_context->post_add_pipeline(pipeline_b2, [&go_b2, &order](std::future<bool> _fut)
                                          {
                                              ASSERT_EQ(order++, 4);
                                              ASSERT_TRUE(_fut.get());
                                              go_b2.set_value();
                                          });
    
    pipeline_c->set_name("com.test.c");
    m_pipeline_context->post_add_pipeline(pipeline_c, [&go_c, &order](std::future<bool> _fut)
                                          {
                                              ASSERT_EQ(order++, 5);
                                              ASSERT_TRUE(_fut.get());
                                              go_c.set_value();
                                          });
    
    pipeline_c1->set_name("com.test.c.1");
    m_pipeline_context->post_add_pipeline(pipeline_c1, [&go_c1, &order](std::future<bool> _fut)
                                          {
                                              ASSERT_EQ(order++, 6);
                                              ASSERT_TRUE(_fut.get());
                                              go_c1.set_value();
                                          });
    
    pipeline_test->set_name("com.test");
    m_pipeline_context->post_add_pipeline(pipeline_test, [&go_test, &order](std::future<bool> _fut)
                                          {
                                              ASSERT_EQ(order++, 7);
                                              ASSERT_TRUE(_fut.get());
                                              go_test.set_value();
                                          });
    
    m_pipeline_context->post_add_pipeline(pipeline_test, [&go_same_test, &order](std::future<bool> _fut)
                                          {
                                              ASSERT_EQ(order++, 8);
                                              ASSERT_FALSE(_fut.get());
                                              go_same_test.set_value();
                                          });
    
    pipeline_another_test->set_name("com.test.a.1");
    m_pipeline_context->post_add_pipeline(pipeline_another_test, [&go_another_test, &order](std::future<bool> _fut)
                                          {
                                              ASSERT_EQ(order++, 9);
                                              ASSERT_TRUE(_fut.get());
                                              go_another_test.set_value();
                                          });
    
    ASSERT_TRUE(wait_for(g_wait_interval, go_a_h1));
    ASSERT_TRUE(wait_for(g_wait_interval, go_a_h2));
    ASSERT_TRUE(wait_for(g_wait_interval, go_c_h));
    
    ASSERT_TRUE(wait_for(g_wait_interval, go_a));
    ASSERT_TRUE(wait_for(g_wait_interval, go_a1));
    ASSERT_TRUE(wait_for(g_wait_interval, go_a2));
    ASSERT_TRUE(wait_for(g_wait_interval, go_b1));
    ASSERT_TRUE(wait_for(g_wait_interval, go_b2));
    ASSERT_TRUE(wait_for(g_wait_interval, go_c));
    ASSERT_TRUE(wait_for(g_wait_interval, go_c1));
    ASSERT_TRUE(wait_for(g_wait_interval, go_test));
    ASSERT_TRUE(wait_for(g_wait_interval, go_same_test));
    
    std::string description = m_pipeline_context->to_string();
    std::stringstream ss;
    ss<<"root"<<"\n";
    ss<<"\tcom"<<"\n";
    ss<<"\t\t"<<"test(pipeline)"<<"\n";
    ss<<"\t\t\t"<<"a(pipeline)"<<"\n";
    ss<<"\t\t\t\t"<<"1(pipeline)"<<"\n";
    ss<<"\t\t\t\t"<<"2(pipeline)"<<"\n";
    ss<<"\t\t\t"<<"b"<<"\n";
    ss<<"\t\t\t\t"<<"1(pipeline)"<<"\n";
    ss<<"\t\t\t\t"<<"2(pipeline)"<<"\n";
    ss<<"\t\t\t"<<"c(pipeline)"<<"\n";
    ss<<"\t\t\t\t"<<"1(pipeline)"<<"\n";
    
    ASSERT_EQ(ss.str(), description);
}
