//
//  runtime_test.cpp
//  lockless
//
//  Created by liuhaibo on 14-11-10.
//  Copyright (c) 2014年 liuhaibo. All rights reserved.
//

#include <future>
#include <lockless/lockless.h>

#include "runtime_context_test.h"
#include "lockless_test_utils.h"

USING_LOCKLESS_NAMESPACE

void runtime_context_test::SetUpTestCase()
{
    
}

void runtime_context_test::TearDownTestCase()
{
    
}

void runtime_context_test::SetUp()
{
    g_object_count = 0;
}

void runtime_context_test::TearDown()
{
    ASSERT_EQ(g_object_count, 0);
}

TEST_F(runtime_context_test, execute)
{
    std::shared_ptr<executor> exe(new executor(GEN_UUID, 1));
    std::shared_ptr<runtime_context> context(new runtime_context(exe->worker_service()));
    
    exe->startup();
    
    ASSERT_TRUE(wait_for(g_wait_interval, [context](std::promise<void> &go){
        
        context->execute([&go](std::future<int> _fut)
                         {
                             GTEST_ASSERT_EQ(_fut.get(), 60);
                             go.set_value();
                         },[]()->int
                         {
                             return 60;
                         });
    }));
    
    ASSERT_TRUE(wait_for(g_wait_interval, [context](std::promise<void> &go){
        
        context->execute([&go](std::future<int> _fut)
                         {
                             GTEST_ASSERT_EQ(_fut.get(), 60);
                             go.set_value();
                         },[](int a, int b, int c)->int
                         {
                             return a + b + c;
                         }, 10, 20, 30);
    }));
    
    std::promise<void> go;
    std::function<int(int, int, int)> fun;
    context->execute([&go](std::future<int> _fut)
                     {
                         GTEST_ASSERT_EQ(_fut.get(), 60);
                         go.set_value();
                     },
                     fun, 10, 20, 30);
    ASSERT_TRUE(wait_until(g_wait_interval, go));
    
    exe->shutdown();
    exe->syncronize();
}


