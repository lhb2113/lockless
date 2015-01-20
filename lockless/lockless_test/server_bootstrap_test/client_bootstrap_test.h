//
//  client_bootstrap_test.h
//  lockless
//
//  Created by liuhaibo on 14-12-15.
//  Copyright (c) 2014年 liuhaibo. All rights reserved.
//

#ifndef CLIENT_BOOTSTRAP_TEST_H
#define CLIENT_BOOTSTRAP_TEST_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <lockless/lockless.h>

class client_bootstrap_initializer_mock : public channel_pipeline_initializer
{
public:
    MOCK_METHOD1(init_channel, void(std::shared_ptr<channel_pipeline>));
};

class client_bootstrap_initializer_factory_mock : public channel_pipeline_initializer_factory
{
public:
    MOCK_CONST_METHOD0(new_initializer, std::shared_ptr<channel_pipeline_initializer>());
};

class client_channel_pipeline_mock : public channel_pipeline
{
public:
    
    client_channel_pipeline_mock(std::shared_ptr<runtime_context> _context)
        : channel_pipeline(_context)
    {
        
    }
    void set_name(const std::string &_name)
    {
        channel_pipeline::set_name(_name);
    }
};

class client_bootstrap_test : public testing::Test
{
public:
    virtual void SetUp();
    virtual void TearDown();
protected:
    std::shared_ptr<channel_pipeline_context> m_pipeline_context;
    std::shared_ptr<server_bootstrap> m_server_bootstrap;
//    std::shared_ptr<client_bootstrap> m_client_bootstrap;
};

#endif //CLIENT_BOOTSTRAP_TEST_H
