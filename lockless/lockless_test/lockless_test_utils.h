//
//  lockless_test_utils.h
//  lockless
//
//  Created by liuhaibo on 14-11-20.
//  Copyright (c) 2014年 liuhaibo. All rights reserved.
//

#ifndef LOCKLESS_TEST_UTILS_H
#define LOCKLESS_TEST_UTILS_H

#include <stdio.h>
#include <functional>
#include <future>

extern const int g_wait_interval;

bool wait_for(int _timeout, std::promise<void>& _promise);

bool wait_for(int _timeout, std::function<void(std::promise<void>&)> _block);

template<typename... Args>
bool wait_for(int _timeout, std::function<void(std::promise<void>&)> _block, Args... args)
{
    std::promise<void> p;
    _block(p);
    bool result = wait_for(_timeout, args...);
    if (!result) {
        return result;
    }
    if (p.get_future().wait_for(std::chrono::seconds(_timeout)) == std::future_status::timeout) {
        return false;
    }
    else {
        return true;
    }
}

bool wait_until(int _timeout, std::promise<void>& _promise);

bool wait_until(int _timeout);

#endif //LOCKLESS_TEST_UTILS_H
