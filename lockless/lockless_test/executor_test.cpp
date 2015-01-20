//
//  executor_test.cpp
//  lockless
//
//  Created by liuhaibo on 14-11-8.
//  Copyright (c) 2014年 liuhaibo. All rights reserved.
//

#include <future>
#include <lockless/lockless.h>

#include "executor_test.h"
#include "lockless_test_utils.h"

USING_LOCKLESS_NAMESPACE

void executor_test::SetUpTestCase()
{
    
}

void executor_test::TearDownTestCase()
{
    
}

void executor_test::SetUp()
{
    g_object_count = 0;
}

void executor_test::TearDown()
{
    ASSERT_EQ(g_object_count, 0);
}

TEST_F(executor_test, startup)
{
    int max_count = 8;
    std::shared_ptr<object> monitor(new object());
    std::shared_ptr<executor> exe(new executor(GEN_UUID, max_count));
    std::shared_ptr<runtime_context> context(new runtime_context(exe->worker_service()));
    
    ASSERT_TRUE(wait_for(g_wait_interval, [exe, monitor, context, max_count](std::promise<void> &go){
        exe->ps_started.subscribe(monitor, context, [&go, max_count](std::shared_ptr<lockless::object>, int){
            static int count = 0;
            count++;
            if (count == max_count) {
                go.set_value();
            }
        });
        exe->startup();
    }));
    
    wait_until(g_wait_interval);
    exe->shutdown();
    exe->syncronize();
}