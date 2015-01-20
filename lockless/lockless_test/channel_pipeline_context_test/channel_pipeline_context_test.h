//
//  channel_pipeline_context_test.h
//  lockless
//
//  Created by liuhaibo on 14-11-27.
//  Copyright (c) 2014年 liuhaibo. All rights reserved.
//

#ifndef CHANNEL_PIPELINE_CONTEXT_TEST_H
#define CHANNEL_PIPELINE_CONTEXT_TEST_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <lockless/lockless.h>

USING_LOCKLESS_NAMESPACE

class channel_pipeline_mock : public channel_pipeline
{
public:
    
    channel_pipeline_mock(std::shared_ptr<runtime_context> _context)
    : channel_pipeline(_context)
    {
        
    }
    void set_name(const std::string &_name)
    {
        channel_pipeline::set_name(_name);
    }
};

class channel_pipeline_context_test : public testing::Test
{
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
public:
    virtual void SetUp();
    virtual void TearDown();
protected:
    void preparePipelineContext();
protected:
    std::shared_ptr<executor> m_executor;
    std::shared_ptr<runtime_context> m_runtime_context;
    std::shared_ptr<channel_pipeline_context> m_pipeline_context;
    
    std::shared_ptr<channel_pipeline_mock> pipeline_a;
    std::shared_ptr<channel_pipeline_mock> pipeline_a1;
    std::shared_ptr<channel_pipeline_mock> pipeline_a2;
    std::shared_ptr<channel_pipeline_mock> pipeline_a3;
    std::shared_ptr<channel_pipeline_mock> pipeline_b1;
    std::shared_ptr<channel_pipeline_mock> pipeline_b2;
    std::shared_ptr<channel_pipeline_mock> pipeline_b3;
    std::shared_ptr<channel_pipeline_mock> pipeline_c;
    std::shared_ptr<channel_pipeline_mock> pipeline_c1;
    std::shared_ptr<channel_pipeline_mock> pipeline_test;
};

#endif //CHANNEL_PIPELINE_CONTEXT_TEST_H
