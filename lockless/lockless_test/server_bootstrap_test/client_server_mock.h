//
//  client_server_mock.h
//  lockless
//
//  Created by liuhaibo on 14-12-15.
//  Copyright (c) 2014年 liuhaibo. All rights reserved.
//

#ifndef CLIENT_SERVER_MOCK_H
#define CLIENT_SERVER_MOCK_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <lockless/lockless.h>

class server_bootstrap_initializer_mock : public channel_pipeline_initializer
{
public:
    MOCK_METHOD1(init_channel, void(std::shared_ptr<channel_pipeline>));
};

class server_bootstrap_initializer_factory_mock : public channel_pipeline_initializer_factory
{
public:
    MOCK_CONST_METHOD0(new_initializer, std::shared_ptr<channel_pipeline_initializer>());
};

class frontend_initializer_mock : public channel_pipeline_initializer
{
public:
    MOCK_METHOD1(init_channel, void(std::shared_ptr<channel_pipeline>));
};

class frontend_initializer_factory_mock : public channel_pipeline_initializer_factory
{
public:
    MOCK_CONST_METHOD0(new_initializer, std::shared_ptr<channel_pipeline_initializer>());
};

#endif
