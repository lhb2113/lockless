//
//  channel_pipeline_test.h
//  lockless
//
//  Created by liuhaibo on 14-11-13.
//  Copyright (c) 2014年 liuhaibo. All rights reserved.
//

#ifndef CHANNEL_PIPELINE_TEST_H
#define CHANNEL_PIPELINE_TEST_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <lockless/lockless.h>

USING_LOCKLESS_NAMESPACE

class channel_pipeline_context_mock : public channel_pipeline_context
{
public:
    ~channel_pipeline_context_mock()
    {
    };
    MOCK_METHOD1(remove_pipeline_posted, void(std::string));
    MOCK_METHOD2(route_read_data_posted, void(std::shared_ptr<channel_data>, std::shared_ptr<router_info>));
    MOCK_METHOD2(route_write_data_posted, void(std::shared_ptr<channel_data>, std::shared_ptr<router_info>));
    
    MOCK_METHOD1(add_pipeline_posted, void(std::shared_ptr<channel_pipeline>));
};

class channel_pipeline_test : public testing::Test
{
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
public:
    virtual void SetUp();
    virtual void TearDown();
protected:
    std::shared_ptr<executor> m_executor;
    std::shared_ptr<runtime_context> m_runtime_context;
    std::shared_ptr<channel_pipeline> m_pipeline;
    std::shared_ptr<channel_pipeline_context_mock> m_pipeline_context_mock;
};

#endif //CHANNEL_PIPELINE_TEST_H
