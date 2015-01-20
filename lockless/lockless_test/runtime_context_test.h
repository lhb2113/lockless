//
//  runtime_test.h
//  lockless
//
//  Created by liuhaibo on 14-11-10.
//  Copyright (c) 2014年 liuhaibo. All rights reserved.
//

#ifndef RUNTIME_CONTEXT_TEST_H
#define RUNTIME_CONTEXT_TEST_H

#include <gtest/gtest.h>

class runtime_context_test : public testing::Test
{
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
public:
    virtual void SetUp();
    virtual void TearDown();
};

#endif //RUNTIME_CONTEXT_TEST_H
