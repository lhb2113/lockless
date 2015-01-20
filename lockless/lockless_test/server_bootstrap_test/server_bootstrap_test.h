//
//  server_bootstrap_test.h
//  lockless
//
//  Created by liuhaibo on 14-12-8.
//  Copyright (c) 2014年 liuhaibo. All rights reserved.
//

#ifndef SERVER_BOOTSTRAP_TEST_H
#define SERVER_BOOTSTRAP_TEST_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <lockless/lockless.h>

class server_bootstrap_test : public testing::Test
{
public:
    virtual void SetUp();
    virtual void TearDown();
protected:
    std::shared_ptr<channel_pipeline_context> m_pipeline_context;
    std::shared_ptr<server_bootstrap> m_server_bootstrap;
};

#endif //SERVER_BOOTSTRAP_TEST_H
