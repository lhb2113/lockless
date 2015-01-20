//
//  template_study.h
//  lockless
//
//  Created by liuhaibo on 14-11-23.
//  Copyright (c) 2014年 liuhaibo. All rights reserved.
//

#ifndef lockless_template_study_h
#define lockless_template_study_h

template<typename FunctionPtr>
struct function_traits_help;

template<typename R, typename T1>
struct function_traits_help<R(*)(T1)>
{
    enum {arty = 1 };
    typedef R result_type;
    typedef T1 arg1_type;
};

// ==========================================================

template<typename Function>
struct function_traits : public function_traits_help<Function*>
{
};

// ==========================================================

template<int N, typename Func>
struct show_args;

template<typename Func>
struct show_args<1, Func> {//1个参数
    void operator()(){
        printf("\targ1 = %s\n", typeid(typename Func::arg1_type).name());
    }
};

// ==========================================================

template<typename T>
void show_function_info(const char* name)
{
//    printf("%s: arg count=%d, result_type:%s\n", name, T::arty, typeid(typename T::result_type).name());
    show_args<T::arty, T> x;
    x();
}

#define SHOW_FUNC(T) \
show_function_info<T  > (#T)

#endif
