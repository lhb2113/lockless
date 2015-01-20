//
//  main.cpp
//  lockless_test
//
//  Created by liuhaibo on 14-11-8.
//  Copyright (c) 2014年 liuhaibo. All rights reserved.
//

#include <gtest/gtest.h>
#include <lockless/lockless.h>
#include "template_study.h"
#include <math.h>

#include "lockless_test_utils.h"
#include "memory_leak_test.h"

//typedef function_traits<int (int,char*)> Function;

#define SHOW_FUNC(T) \
show_function_info<T  > (#T)

GTEST_API_ int main(int argc, char **argv) {
    printf("Running main() from gtest_main.cc\n");
    set_logger_level(LOG_EMERG);
    testing::InitGoogleTest(&argc, argv);
    std::stringstream ss;
//    ss<<"channel_pipeline_context_test.add_pipeline:";
//    ss<<"channel_pipeline_context_test.remove_pipeline:";
//    ss<<"channel_pipeline_context_test.route_write_data*:";
//    ss<<"channel_pipeline_context_test.route_read_data*:";
    
//    ss<<"channel_pipeline_test.*:";
//    ss<<"memory_leak_test.*:";
//    ss<<"server_bootstrap_test.*:";
//    ss<<"client_bootstrap_test.*:";
    
//    ss<<"executor_test.*:";
//    testing::GTEST_FLAG(filter) = ss.str();
    int ret = RUN_ALL_TESTS();
    return ret;
}