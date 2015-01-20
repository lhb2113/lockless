//
//  data_type.cpp
//  lockless
//
//  Created by liuhaibo on 14-12-12.
//  Copyright (c) 2014年 liuhaibo. All rights reserved.
//

#include "data_type.h"

OPEN_LOCKLESS_NAMESPACE

bool operator==(const data_type& __x, const data_type& __y)
{
    return __x.category() == __y.category() && __x.type() == __y.type();
}

bool operator<(const data_type& __x, const data_type& __y)
{
    return __x.category() < __y.category() || (__x.category() == __y.category() && __x.type() < __y.type());
}

CLOSE_LOCKLESS_NAMESPACE