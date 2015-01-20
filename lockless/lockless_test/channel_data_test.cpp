//
//  channel_data_test.cpp
//  lockless
//
//  Created by liuhaibo on 14-12-12.
//  Copyright (c) 2014年 liuhaibo. All rights reserved.
//

#include <map>
#include "channel_data_test.h"

class common_data_category : public data_category
{
public:
    const char* name() const _NOEXCEPT { return "common_category_impl"; }
    std::string message(int _type) const _NOEXCEPT {
        switch (_type) {
            case static_cast<int>(common_data::center_gateway):
                return "center_gateway";
            case static_cast<int>(common_data::center_login):
                return "center_login";
            case static_cast<int>(common_data::center_master):
                return "center_master";
            case static_cast<int>(common_data::login_gateway):
                return "login_gateway";
            case static_cast<int>(common_data::master_gateway):
                return "master_gateway";
            default:
                return "unknown";
        }
    }
};

common_data_category g_common_data_category;

const data_category& get_common_category()
{
    return g_common_data_category;
}

template<>
struct data_category_trait<common_data>
{
    static const data_category &category(){ return get_common_category(); }
};

// ==========================================================================================

void channel_data_test::SetUp()
{
    g_object_count = 0;
}

void channel_data_test::TearDown()
{
    ASSERT_EQ(g_object_count, 0);
}

TEST_F(channel_data_test, multi_category_usage)
{
    data_type _socket_stream_data(lockless_data::socket_stream);
    data_type _connect_status_data(lockless_data::connect_status);
    data_type _center_gateway_data(common_data::center_gateway);
    data_type _center_login_data(common_data::center_login);
    
    std::map<data_type, int> data_map;
    data_map[_socket_stream_data] = 1000;
    data_map[_connect_status_data] = 2000;
    data_map[_center_gateway_data] = 3000;
    data_map[_center_login_data] = 4000;
    
    data_type _socket_stream_data_tmp = lockless_data::socket_stream;
    data_type _connect_status_data_tmp = lockless_data::connect_status;
    data_type _center_gateway_data_tmp = common_data::center_gateway;
    data_type _center_login_data_tmp = common_data::center_login;
    
    ASSERT_EQ(data_map[_socket_stream_data_tmp], 1000);
    ASSERT_EQ(data_map[_connect_status_data_tmp], 2000);
    ASSERT_EQ(data_map[_center_gateway_data_tmp], 3000);
    ASSERT_EQ(data_map[_center_login_data_tmp], 4000);
    
//    data_type2 _socket_stream_data_2 = lockless_data::socket_stream;
//    data_type2 a(lockless_data::socket_stream, get_common_category());
    
    std::error_code ec;
}