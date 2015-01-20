//
//  channel_data_test.h
//  lockless
//
//  Created by liuhaibo on 14-12-12.
//  Copyright (c) 2014年 liuhaibo. All rights reserved.
//

#ifndef CHANNEL_DATA_TEST_H
#define CHANNEL_DATA_TEST_H

#include <gtest/gtest.h>
#include <string>
#include <lockless/lockless.h>

USING_LOCKLESS_NAMESPACE

enum class common_data
{
    center_gateway = 1,
    center_login,
    center_master,
    login_gateway,
    master_gateway,
};

const data_category& get_common_category();

// ==========================================================================================

class channel_data_test : public testing::Test
{
public:
    virtual void SetUp();
    virtual void TearDown();
};

#endif //CHANNEL_DATA_TEST_H
