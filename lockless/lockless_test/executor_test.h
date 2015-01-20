//
//  executor_test.h
//  lockless
//
//  Created by liuhaibo on 14-11-8.
//  Copyright (c) 2014年 liuhaibo. All rights reserved.
//

#ifndef EXECUTOR_TEST_H
#define EXECUTOR_TEST_H

#include <gtest/gtest.h>

class executor_test : public testing::Test
{
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
public:
    virtual void SetUp();
    virtual void TearDown();
};

#endif //EXECUTOR_TEST_H
