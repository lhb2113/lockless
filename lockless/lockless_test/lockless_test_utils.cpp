//
//  lockless_test_utils.cpp
//  lockless
//
//  Created by liuhaibo on 14-11-20.
//  Copyright (c) 2014年 liuhaibo. All rights reserved.
//

#include <gtest/gtest.h>
#include "lockless_test_utils.h"

const int g_wait_interval = 1;

bool wait_for(int _timeout, std::promise<void>& _promise)
{
    if (_promise.get_future().wait_for(std::chrono::seconds(_timeout)) == std::future_status::timeout) {
        return false;
    }
    else {
        return true;
    }
}

bool wait_for(int _timeout, std::function<void(std::promise<void>&)> _block)
{
    std::promise<void> p;
    _block(p);
    if (p.get_future().wait_for(std::chrono::seconds(_timeout)) == std::future_status::timeout) {
        return false;
    }
    else {
        return true;
    }
}

bool wait_until(int _timeout, std::promise<void>& _promise)
{
    if (_promise.get_future().wait_for(std::chrono::seconds(_timeout)) == std::future_status::timeout) {
        return true;
    }
    else {
        return false;
    }
}

bool wait_until(int _timeout)
{
    std::promise<void> p;
    if (p.get_future().wait_for(std::chrono::seconds(_timeout)) == std::future_status::timeout) {
        return true;
    }
    else {
        return false;
    }
}
