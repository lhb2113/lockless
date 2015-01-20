//
//  route_read_data.cpp
//  lockless
//
//  Created by liuhaibo on 14-12-3.
//  Copyright (c) 2014年 liuhaibo. All rights reserved.
//

#include "channel_pipeline_context_test.h"
#include "lockless_test_utils.h"

/**
 * route data to the child pipelines of "com".
 */
TEST_F(channel_pipeline_context_test, route_read_data_all_1)
{
    preparePipelineContext();
    
    std::promise<void> go_test;
    std::promise<void> go_a;
    std::promise<void> go_a1;
    std::promise<void> go_a2;
    std::promise<void> go_a3;
    std::promise<void> go_b1;
    std::promise<void> go_b2;
    std::promise<void> go_b3;
    std::promise<void> go_c;
    std::promise<void> go_c1;
    
    auto htest = std::make_shared<channel_handler>();
    htest->set_read_handler(lockless_data::socket_stream, [&go_test](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                            {
                                ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                                go_test.set_value();
                            });
    pipeline_test->post_add_channel_handler(htest, "htest", false, nullptr);
    
    auto ha = std::make_shared<channel_handler>();
    ha->set_read_handler(lockless_data::socket_stream, [&go_a](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                         {
                             ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                             go_a.set_value();
                         });
    pipeline_a->post_add_channel_handler(ha, "ha", false, nullptr);
    
    auto ha1 = std::make_shared<channel_handler>();
    ha1->set_read_handler(lockless_data::socket_stream, [&go_a1](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_a1.set_value();
                          });
    pipeline_a1->post_add_channel_handler(ha1, "ha1", false, nullptr);
    
    auto ha2 = std::make_shared<channel_handler>();
    ha2->set_read_handler(lockless_data::socket_stream, [&go_a2](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_a2.set_value();
                          });
    pipeline_a2->post_add_channel_handler(ha2, "ha2", false, nullptr);
    
    auto ha3 = std::make_shared<channel_handler>();
    ha3->set_read_handler(lockless_data::socket_stream, [&go_a3](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_a3.set_value();
                          });
    pipeline_a3->post_add_channel_handler(ha3, "ha3", false, nullptr);
    
    auto hb1 = std::make_shared<channel_handler>();
    hb1->set_read_handler(lockless_data::socket_stream, [&go_b1](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_b1.set_value();
                          });
    pipeline_b1->post_add_channel_handler(hb1, "hb1", false, nullptr);
    
    auto hb2 = std::make_shared<channel_handler>();
    hb2->set_read_handler(lockless_data::socket_stream, [&go_b2](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_b2.set_value();
                          });
    pipeline_b2->post_add_channel_handler(hb2, "hb2", false, nullptr);
    
    auto hb3 = std::make_shared<channel_handler>();
    hb3->set_read_handler(lockless_data::socket_stream, [&go_b3](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_b3.set_value();
                          });
    pipeline_b3->post_add_channel_handler(hb3, "hb3", false, nullptr);
    
    auto hc = std::make_shared<channel_handler>();
    hc->set_read_handler(lockless_data::socket_stream, [&go_c](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                         {
                             ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                             go_c.set_value();
                         });
    pipeline_c->post_add_channel_handler(hc, "hc", false, nullptr);
    
    auto hc1 = std::make_shared<channel_handler>();
    hc1->set_read_handler(lockless_data::socket_stream, [&go_c1](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_c1.set_value();
                          });
    pipeline_c1->post_add_channel_handler(hc1, "hc1", false, nullptr);
    
    ASSERT_TRUE(wait_for(g_wait_interval, [this](std::promise<void> &go){
        
        auto data = std::make_shared<channel_data>(lockless_data::socket_stream);
        auto info = std::make_shared<router_info>();
        info->from = router_info::wild_any;
        info->to = "com";
        info->wild = router_info::wild_all;
        m_pipeline_context->post_route_read_data(data, info, [&go](std::future<int> _fut)
                                                 {
                                                     int count = _fut.get();
                                                     ASSERT_EQ(count, 10);
                                                     go.set_value();
                                                 });
    }));
    
    ASSERT_TRUE(wait_for(g_wait_interval, go_test));
    ASSERT_TRUE(wait_for(g_wait_interval, go_a));
    ASSERT_TRUE(wait_for(g_wait_interval, go_a1));
    ASSERT_TRUE(wait_for(g_wait_interval, go_a2));
    ASSERT_TRUE(wait_for(g_wait_interval, go_a3));
    ASSERT_TRUE(wait_for(g_wait_interval, go_b1));
    ASSERT_TRUE(wait_for(g_wait_interval, go_b2));
    ASSERT_TRUE(wait_for(g_wait_interval, go_b3));
    ASSERT_TRUE(wait_for(g_wait_interval, go_c));
    ASSERT_TRUE(wait_for(g_wait_interval, go_c1));
}

/**
 * route data to the child pipelines of "com.test".
 */
TEST_F(channel_pipeline_context_test, route_read_data_all_2)
{
    preparePipelineContext();
    
    std::promise<void> go_test;
    std::promise<void> go_a;
    std::promise<void> go_a1;
    std::promise<void> go_a2;
    std::promise<void> go_a3;
    std::promise<void> go_b1;
    std::promise<void> go_b2;
    std::promise<void> go_b3;
    std::promise<void> go_c;
    std::promise<void> go_c1;
    
    auto htest = std::make_shared<channel_handler>();
    htest->set_read_handler(lockless_data::socket_stream, [&go_test](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                            {
                                ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                                go_test.set_value();
                            });
    pipeline_test->post_add_channel_handler(htest, "htest", false, nullptr);
    
    auto ha = std::make_shared<channel_handler>();
    ha->set_read_handler(lockless_data::socket_stream, [&go_a](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                         {
                             ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                             go_a.set_value();
                         });
    pipeline_a->post_add_channel_handler(ha, "ha", false, nullptr);
    
    auto ha1 = std::make_shared<channel_handler>();
    ha1->set_read_handler(lockless_data::socket_stream, [&go_a1](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_a1.set_value();
                          });
    pipeline_a1->post_add_channel_handler(ha1, "ha1", false, nullptr);
    
    auto ha2 = std::make_shared<channel_handler>();
    ha2->set_read_handler(lockless_data::socket_stream, [&go_a2](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_a2.set_value();
                          });
    pipeline_a2->post_add_channel_handler(ha2, "ha2", false, nullptr);
    
    auto ha3 = std::make_shared<channel_handler>();
    ha3->set_read_handler(lockless_data::socket_stream, [&go_a3](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_a3.set_value();
                          });
    pipeline_a3->post_add_channel_handler(ha3, "ha3", false, nullptr);
    
    auto hb1 = std::make_shared<channel_handler>();
    hb1->set_read_handler(lockless_data::socket_stream, [&go_b1](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_b1.set_value();
                          });
    pipeline_b1->post_add_channel_handler(hb1, "hb1", false, nullptr);
    
    auto hb2 = std::make_shared<channel_handler>();
    hb2->set_read_handler(lockless_data::socket_stream, [&go_b2](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_b2.set_value();
                          });
    pipeline_b2->post_add_channel_handler(hb2, "hb2", false, nullptr);
    
    auto hb3 = std::make_shared<channel_handler>();
    hb3->set_read_handler(lockless_data::socket_stream, [&go_b3](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_b3.set_value();
                          });
    pipeline_b3->post_add_channel_handler(hb3, "hb3", false, nullptr);
    
    auto hc = std::make_shared<channel_handler>();
    hc->set_read_handler(lockless_data::socket_stream, [&go_c](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                         {
                             ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                             go_c.set_value();
                         });
    pipeline_c->post_add_channel_handler(hc, "hc", false, nullptr);
    
    auto hc1 = std::make_shared<channel_handler>();
    hc1->set_read_handler(lockless_data::socket_stream, [&go_c1](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_c1.set_value();
                          });
    pipeline_c1->post_add_channel_handler(hc1, "hc1", false, nullptr);
    
    ASSERT_TRUE(wait_for(g_wait_interval, [this](std::promise<void> &go){
        
        auto data = std::make_shared<channel_data>(lockless_data::socket_stream);
        auto info = std::make_shared<router_info>();
        info->from = router_info::wild_any;
        info->to = "com.test";
        info->wild = router_info::wild_all;
        m_pipeline_context->post_route_read_data(data, info, [&go](std::future<int> _fut)
                                                 {
                                                     int count = _fut.get();
                                                     ASSERT_EQ(count, 9);
                                                     go.set_value();
                                                 });
    }));
    
    ASSERT_TRUE(wait_for(g_wait_interval, go_a));
    ASSERT_TRUE(wait_for(g_wait_interval, go_a1));
    ASSERT_TRUE(wait_for(g_wait_interval, go_a2));
    ASSERT_TRUE(wait_for(g_wait_interval, go_a3));
    ASSERT_TRUE(wait_for(g_wait_interval, go_b1));
    ASSERT_TRUE(wait_for(g_wait_interval, go_b2));
    ASSERT_TRUE(wait_for(g_wait_interval, go_b3));
    ASSERT_TRUE(wait_for(g_wait_interval, go_c));
    ASSERT_TRUE(wait_for(g_wait_interval, go_c1));
    wait_until(g_wait_interval);
}

/**
 * route data to the child pipelines of "com.test.a".
 */
TEST_F(channel_pipeline_context_test, route_read_data_all_3)
{
    preparePipelineContext();
    
    std::promise<void> go_test;
    std::promise<void> go_a;
    std::promise<void> go_a1;
    std::promise<void> go_a2;
    std::promise<void> go_a3;
    std::promise<void> go_b1;
    std::promise<void> go_b2;
    std::promise<void> go_b3;
    std::promise<void> go_c;
    std::promise<void> go_c1;
    
    auto htest = std::make_shared<channel_handler>();
    htest->set_read_handler(lockless_data::socket_stream, [&go_test](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                            {
                                ASSERT_TRUE(false);
                                go_test.set_value();
                            });
    pipeline_test->post_add_channel_handler(htest, "htest", false, nullptr);
    
    auto ha = std::make_shared<channel_handler>();
    ha->set_read_handler(lockless_data::socket_stream, [&go_a](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                         {
                             ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                             go_a.set_value();
                         });
    pipeline_a->post_add_channel_handler(ha, "ha", false, nullptr);
    
    auto ha1 = std::make_shared<channel_handler>();
    ha1->set_read_handler(lockless_data::socket_stream, [&go_a1](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_a1.set_value();
                          });
    pipeline_a1->post_add_channel_handler(ha1, "ha1", false, nullptr);
    
    auto ha2 = std::make_shared<channel_handler>();
    ha2->set_read_handler(lockless_data::socket_stream, [&go_a2](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_a2.set_value();
                          });
    pipeline_a2->post_add_channel_handler(ha2, "ha2", false, nullptr);
    
    auto ha3 = std::make_shared<channel_handler>();
    ha3->set_read_handler(lockless_data::socket_stream, [&go_a3](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_a3.set_value();
                          });
    pipeline_a3->post_add_channel_handler(ha3, "ha3", false, nullptr);
    
    auto hb1 = std::make_shared<channel_handler>();
    hb1->set_read_handler(lockless_data::socket_stream, [&go_b1](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_TRUE(false);
                              go_b1.set_value();
                          });
    pipeline_b1->post_add_channel_handler(hb1, "hb1", false, nullptr);
    
    auto hb2 = std::make_shared<channel_handler>();
    hb2->set_read_handler(lockless_data::socket_stream, [&go_b2](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_TRUE(false);
                              go_b2.set_value();
                          });
    pipeline_b2->post_add_channel_handler(hb2, "hb2", false, nullptr);
    
    auto hb3 = std::make_shared<channel_handler>();
    hb3->set_read_handler(lockless_data::socket_stream, [&go_b3](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_TRUE(false);
                              go_b3.set_value();
                          });
    pipeline_b3->post_add_channel_handler(hb3, "hb3", false, nullptr);
    
    auto hc = std::make_shared<channel_handler>();
    hc->set_read_handler(lockless_data::socket_stream, [&go_c](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                         {
                             ASSERT_TRUE(false);
                             go_c.set_value();
                         });
    pipeline_c->post_add_channel_handler(hc, "hc", false, nullptr);
    
    auto hc1 = std::make_shared<channel_handler>();
    hc1->set_read_handler(lockless_data::socket_stream, [&go_c1](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_TRUE(false);
                              go_c1.set_value();
                          });
    pipeline_c1->post_add_channel_handler(hc1, "hc1", false, nullptr);
    
    ASSERT_TRUE(wait_for(g_wait_interval, [this](std::promise<void> &go){
        
        auto data = std::make_shared<channel_data>(lockless_data::socket_stream);
        auto info = std::make_shared<router_info>();
        info->from = router_info::wild_any;
        info->to = "com.test.a";
        info->wild = router_info::wild_all;
        m_pipeline_context->post_route_read_data(data, info, [&go](std::future<int> _fut)
                                                 {
                                                     int count = _fut.get();
                                                     ASSERT_EQ(count, 3);
                                                     go.set_value();
                                                 });
    }));
    
    ASSERT_TRUE(wait_for(g_wait_interval, go_a1));
    ASSERT_TRUE(wait_for(g_wait_interval, go_a2));
    ASSERT_TRUE(wait_for(g_wait_interval, go_a3));
    wait_until(g_wait_interval);
}

/**
 * route data to the exact "com.test.a.1" pipline.
 */
TEST_F(channel_pipeline_context_test, route_read_data_exactly_1)
{
    preparePipelineContext();
    
    std::promise<void> go_test;
    std::promise<void> go_a;
    std::promise<void> go_a1;
    std::promise<void> go_a2;
    std::promise<void> go_a3;
    std::promise<void> go_b1;
    std::promise<void> go_b2;
    std::promise<void> go_b3;
    std::promise<void> go_c;
    std::promise<void> go_c1;
    
    auto htest = std::make_shared<channel_handler>();
    htest->set_read_handler(lockless_data::socket_stream, [&go_test](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                            {
                                ASSERT_TRUE(false);
                                go_test.set_value();
                            });
    pipeline_test->post_add_channel_handler(htest, "htest", false, nullptr);
    
    auto ha = std::make_shared<channel_handler>();
    ha->set_read_handler(lockless_data::socket_stream, [&go_a](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                         {
                             ASSERT_TRUE(false);
                             go_a.set_value();
                         });
    pipeline_a->post_add_channel_handler(ha, "ha", false, nullptr);
    
    auto ha1 = std::make_shared<channel_handler>();
    ha1->set_read_handler(lockless_data::socket_stream, [&go_a1](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_a1.set_value();
                          });
    pipeline_a1->post_add_channel_handler(ha1, "ha1", false, nullptr);
    
    auto ha2 = std::make_shared<channel_handler>();
    ha2->set_read_handler(lockless_data::socket_stream, [&go_a2](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_TRUE(false);
                              go_a2.set_value();
                          });
    pipeline_a2->post_add_channel_handler(ha2, "ha2", false, nullptr);
    
    auto ha3 = std::make_shared<channel_handler>();
    ha3->set_read_handler(lockless_data::socket_stream, [&go_a3](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_TRUE(false);
                              go_a3.set_value();
                          });
    pipeline_a3->post_add_channel_handler(ha3, "ha3", false, nullptr);
    
    auto hb1 = std::make_shared<channel_handler>();
    hb1->set_read_handler(lockless_data::socket_stream, [&go_b1](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_TRUE(false);
                              go_b1.set_value();
                          });
    pipeline_b1->post_add_channel_handler(hb1, "hb1", false, nullptr);
    
    auto hb2 = std::make_shared<channel_handler>();
    hb2->set_read_handler(lockless_data::socket_stream, [&go_b2](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_TRUE(false);
                              go_b2.set_value();
                          });
    pipeline_b2->post_add_channel_handler(hb2, "hb2", false, nullptr);
    
    auto hb3 = std::make_shared<channel_handler>();
    hb3->set_read_handler(lockless_data::socket_stream, [&go_b3](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_TRUE(false);
                              go_b3.set_value();
                          });
    pipeline_b3->post_add_channel_handler(hb3, "hb3", false, nullptr);
    
    auto hc = std::make_shared<channel_handler>();
    hc->set_read_handler(lockless_data::socket_stream, [&go_c](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                         {
                             ASSERT_TRUE(false);
                             go_c.set_value();
                         });
    pipeline_c->post_add_channel_handler(hc, "hc", false, nullptr);
    
    auto hc1 = std::make_shared<channel_handler>();
    hc1->set_read_handler(lockless_data::socket_stream, [&go_c1](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_TRUE(false);
                              go_c1.set_value();
                          });
    pipeline_c1->post_add_channel_handler(hc1, "hc1", false, nullptr);
    
    ASSERT_TRUE(wait_for(g_wait_interval, [this](std::promise<void> &go){
        
        auto data = std::make_shared<channel_data>(lockless_data::socket_stream);
        auto info = std::make_shared<router_info>();
        info->from = router_info::wild_any;
        info->to = "com.test.a.1";
        info->wild = router_info::wild_exactly;
        m_pipeline_context->post_route_read_data(data, info, [&go](std::future<int> _fut)
                                                 {
                                                     int count = _fut.get();
                                                     ASSERT_EQ(count, 1);
                                                     go.set_value();
                                                 });
    }));
    
    ASSERT_TRUE(wait_for(g_wait_interval, go_a1));
    wait_until(g_wait_interval);
}

/**
 * route data to the exact "com.test.a" pipline.
 */
TEST_F(channel_pipeline_context_test, route_read_data_exactly_2)
{
    preparePipelineContext();
    
    std::promise<void> go_test;
    std::promise<void> go_a;
    std::promise<void> go_a1;
    std::promise<void> go_a2;
    std::promise<void> go_a3;
    std::promise<void> go_b1;
    std::promise<void> go_b2;
    std::promise<void> go_b3;
    std::promise<void> go_c;
    std::promise<void> go_c1;
    
    auto htest = std::make_shared<channel_handler>();
    htest->set_read_handler(lockless_data::socket_stream, [&go_test](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                            {
                                ASSERT_TRUE(false);
                                go_test.set_value();
                            });
    pipeline_test->post_add_channel_handler(htest, "htest", false, nullptr);
    
    auto ha = std::make_shared<channel_handler>();
    ha->set_read_handler(lockless_data::socket_stream, [&go_a](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                         {
                             ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                             go_a.set_value();
                         });
    pipeline_a->post_add_channel_handler(ha, "ha", false, nullptr);
    
    auto ha1 = std::make_shared<channel_handler>();
    ha1->set_read_handler(lockless_data::socket_stream, [&go_a1](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_TRUE(false);
                              go_a1.set_value();
                          });
    pipeline_a1->post_add_channel_handler(ha1, "ha1", false, nullptr);
    
    auto ha2 = std::make_shared<channel_handler>();
    ha2->set_read_handler(lockless_data::socket_stream, [&go_a2](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_TRUE(false);
                              go_a2.set_value();
                          });
    pipeline_a2->post_add_channel_handler(ha2, "ha2", false, nullptr);
    
    auto ha3 = std::make_shared<channel_handler>();
    ha3->set_read_handler(lockless_data::socket_stream, [&go_a3](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_TRUE(false);
                              go_a3.set_value();
                          });
    pipeline_a3->post_add_channel_handler(ha3, "ha3", false, nullptr);
    
    auto hb1 = std::make_shared<channel_handler>();
    hb1->set_read_handler(lockless_data::socket_stream, [&go_b1](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_TRUE(false);
                              go_b1.set_value();
                          });
    pipeline_b1->post_add_channel_handler(hb1, "hb1", false, nullptr);
    
    auto hb2 = std::make_shared<channel_handler>();
    hb2->set_read_handler(lockless_data::socket_stream, [&go_b2](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_TRUE(false);
                              go_b2.set_value();
                          });
    pipeline_b2->post_add_channel_handler(hb2, "hb2", false, nullptr);
    
    auto hb3 = std::make_shared<channel_handler>();
    hb3->set_read_handler(lockless_data::socket_stream, [&go_b3](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_TRUE(false);
                              go_b3.set_value();
                          });
    pipeline_b3->post_add_channel_handler(hb3, "hb3", false, nullptr);
    
    auto hc = std::make_shared<channel_handler>();
    hc->set_read_handler(lockless_data::socket_stream, [&go_c](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                         {
                             ASSERT_TRUE(false);
                             go_c.set_value();
                         });
    pipeline_c->post_add_channel_handler(hc, "hc", false, nullptr);
    
    auto hc1 = std::make_shared<channel_handler>();
    hc1->set_read_handler(lockless_data::socket_stream, [&go_c1](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_TRUE(false);
                              go_c1.set_value();
                          });
    pipeline_c1->post_add_channel_handler(hc1, "hc1", false, nullptr);
    
    ASSERT_TRUE(wait_for(g_wait_interval, [this](std::promise<void> &go){
        
        auto data = std::make_shared<channel_data>(lockless_data::socket_stream);
        auto info = std::make_shared<router_info>();
        info->from = router_info::wild_any;
        info->to = "com.test.a";
        info->wild = router_info::wild_exactly;
        m_pipeline_context->post_route_read_data(data, info, [&go](std::future<int> _fut)
                                                 {
                                                     int count = _fut.get();
                                                     ASSERT_EQ(count, 1);
                                                     go.set_value();
                                                 });
    }));
    
    ASSERT_TRUE(wait_for(g_wait_interval, go_a));
    wait_until(g_wait_interval);
}

/**
 * route data to the any "com.test.a" child pipline.
 */
TEST_F(channel_pipeline_context_test, route_read_data_any_1)
{
    preparePipelineContext();
    
    srand((unsigned)time(NULL));
    
    std::promise<void> go_test;
    std::promise<void> go_a;
    std::promise<void> go_a1;
    std::promise<void> go_a2;
    std::promise<void> go_a3;
    std::promise<void> go_b1;
    std::promise<void> go_b2;
    std::promise<void> go_b3;
    std::promise<void> go_c;
    std::promise<void> go_c1;
    
    auto htest = std::make_shared<channel_handler>();
    htest->set_read_handler(lockless_data::socket_stream, [&go_test](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                            {
                                ASSERT_TRUE(false);
                                go_test.set_value();
                            });
    pipeline_test->post_add_channel_handler(htest, "htest", false, nullptr);
    
    auto ha = std::make_shared<channel_handler>();
    ha->set_read_handler(lockless_data::socket_stream, [&go_a](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                         {
                             ASSERT_TRUE(false);
                             go_a.set_value();
                         });
    pipeline_a->post_add_channel_handler(ha, "ha", false, nullptr);
    
    auto ha1 = std::make_shared<channel_handler>();
    ha1->set_read_handler(lockless_data::socket_stream, [&go_a1](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_a1.set_value();
                          });
    pipeline_a1->post_add_channel_handler(ha1, "ha1", false, nullptr);
    
    auto ha2 = std::make_shared<channel_handler>();
    ha2->set_read_handler(lockless_data::socket_stream, [&go_a2](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_a2.set_value();
                          });
    pipeline_a2->post_add_channel_handler(ha2, "ha2", false, nullptr);
    
    auto ha3 = std::make_shared<channel_handler>();
    ha3->set_read_handler(lockless_data::socket_stream, [&go_a3](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_a3.set_value();
                          });
    pipeline_a3->post_add_channel_handler(ha3, "ha3", false, nullptr);
    
    auto hb1 = std::make_shared<channel_handler>();
    hb1->set_read_handler(lockless_data::socket_stream, [&go_b1](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_TRUE(false);
                              go_b1.set_value();
                          });
    pipeline_b1->post_add_channel_handler(hb1, "hb1", false, nullptr);
    
    auto hb2 = std::make_shared<channel_handler>();
    hb2->set_read_handler(lockless_data::socket_stream, [&go_b2](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_TRUE(false);
                              go_b2.set_value();
                          });
    pipeline_b2->post_add_channel_handler(hb2, "hb2", false, nullptr);
    
    auto hb3 = std::make_shared<channel_handler>();
    hb3->set_read_handler(lockless_data::socket_stream, [&go_b3](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_TRUE(false);
                              go_b3.set_value();
                          });
    pipeline_b3->post_add_channel_handler(hb3, "hb3", false, nullptr);
    
    auto hc = std::make_shared<channel_handler>();
    hc->set_read_handler(lockless_data::socket_stream, [&go_c](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                         {
                             ASSERT_TRUE(false);
                             go_c.set_value();
                         });
    pipeline_c->post_add_channel_handler(hc, "hc", false, nullptr);
    
    auto hc1 = std::make_shared<channel_handler>();
    hc1->set_read_handler(lockless_data::socket_stream, [&go_c1](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_TRUE(false);
                              go_c1.set_value();
                          });
    pipeline_c1->post_add_channel_handler(hc1, "hc1", false, nullptr);
    
    ASSERT_TRUE(wait_for(g_wait_interval, [this](std::promise<void> &go){
        
        auto data = std::make_shared<channel_data>(lockless_data::socket_stream);
        auto info = std::make_shared<router_info>();
        info->from = router_info::wild_any;
        info->to = "com.test.a";
        info->wild = router_info::wild_any;
        m_pipeline_context->post_route_read_data(data, info, [&go](std::future<int> _fut)
                                                 {
                                                     int count = _fut.get();
                                                     ASSERT_EQ(count, 1);
                                                     go.set_value();
                                                 });
    }));
    
    int result_a1 = wait_for(g_wait_interval, go_a1);
    int result_a2 = wait_for(g_wait_interval, go_a2);
    int result_a3 = wait_for(g_wait_interval, go_a3);
    ASSERT_EQ(result_a1 + result_a2 + result_a3, 1);
    wait_until(g_wait_interval);
}


/**
 * route data to the any "com" child pipline.
 */
TEST_F(channel_pipeline_context_test, route_read_data_any_2)
{
    preparePipelineContext();
    
    srand((unsigned)time(NULL));
    
    std::promise<void> go_test;
    std::promise<void> go_a;
    std::promise<void> go_a1;
    std::promise<void> go_a2;
    std::promise<void> go_a3;
    std::promise<void> go_b1;
    std::promise<void> go_b2;
    std::promise<void> go_b3;
    std::promise<void> go_c;
    std::promise<void> go_c1;
    
    auto htest = std::make_shared<channel_handler>();
    htest->set_read_handler(lockless_data::socket_stream, [&go_test](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                            {
                                ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                                go_test.set_value();
                            });
    pipeline_test->post_add_channel_handler(htest, "htest", false, nullptr);
    
    auto ha = std::make_shared<channel_handler>();
    ha->set_read_handler(lockless_data::socket_stream, [&go_a](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                         {
                             ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                             go_a.set_value();
                         });
    pipeline_a->post_add_channel_handler(ha, "ha", false, nullptr);
    
    auto ha1 = std::make_shared<channel_handler>();
    ha1->set_read_handler(lockless_data::socket_stream, [&go_a1](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_a1.set_value();
                          });
    pipeline_a1->post_add_channel_handler(ha1, "ha1", false, nullptr);
    
    auto ha2 = std::make_shared<channel_handler>();
    ha2->set_read_handler(lockless_data::socket_stream, [&go_a2](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_a2.set_value();
                          });
    pipeline_a2->post_add_channel_handler(ha2, "ha2", false, nullptr);
    
    auto ha3 = std::make_shared<channel_handler>();
    ha3->set_read_handler(lockless_data::socket_stream, [&go_a3](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_a3.set_value();
                          });
    pipeline_a3->post_add_channel_handler(ha3, "ha3", false, nullptr);
    
    auto hb1 = std::make_shared<channel_handler>();
    hb1->set_read_handler(lockless_data::socket_stream, [&go_b1](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_b1.set_value();
                          });
    pipeline_b1->post_add_channel_handler(hb1, "hb1", false, nullptr);
    
    auto hb2 = std::make_shared<channel_handler>();
    hb2->set_read_handler(lockless_data::socket_stream, [&go_b2](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_b2.set_value();
                          });
    pipeline_b2->post_add_channel_handler(hb2, "hb2", false, nullptr);
    
    auto hb3 = std::make_shared<channel_handler>();
    hb3->set_read_handler(lockless_data::socket_stream, [&go_b3](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_b3.set_value();
                          });
    pipeline_b3->post_add_channel_handler(hb3, "hb3", false, nullptr);
    
    auto hc = std::make_shared<channel_handler>();
    hc->set_read_handler(lockless_data::socket_stream, [&go_c](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                         {
                             ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                             go_c.set_value();
                         });
    pipeline_c->post_add_channel_handler(hc, "hc", false, nullptr);
    
    auto hc1 = std::make_shared<channel_handler>();
    hc1->set_read_handler(lockless_data::socket_stream, [&go_c1](std::shared_ptr<channel_data> _data, std::shared_ptr<router_info>)
                          {
                              ASSERT_EQ(_data->get_type(), lockless_data::socket_stream);
                              go_c1.set_value();
                          });
    pipeline_c1->post_add_channel_handler(hc1, "hc1", false, nullptr);
    
    ASSERT_TRUE(wait_for(g_wait_interval, [this](std::promise<void> &go){
        
        auto data = std::make_shared<channel_data>(lockless_data::socket_stream);
        auto info = std::make_shared<router_info>();
        info->from = router_info::wild_any;
        info->to = "com";
        info->wild = router_info::wild_any;
        m_pipeline_context->post_route_read_data(data, info, [&go](std::future<int> _fut)
                                                 {
                                                     int count = _fut.get();
                                                     ASSERT_EQ(count, 1);
                                                     go.set_value();
                                                 });
    }));
    
    int result = wait_for(g_wait_interval, go_test);
    result += wait_for(g_wait_interval, go_a);
    result += wait_for(g_wait_interval, go_a1);
    result += wait_for(g_wait_interval, go_a2);
    result += wait_for(g_wait_interval, go_a3);
    result += wait_for(g_wait_interval, go_b1);
    result += wait_for(g_wait_interval, go_b2);
    result += wait_for(g_wait_interval, go_b3);
    result += wait_for(g_wait_interval, go_c);
    result += wait_for(g_wait_interval, go_c1);
    
    ASSERT_EQ(result, 1);
}

