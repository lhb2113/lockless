#ifndef DEFINES_H
#define DEFINES_H

#include <future>
#include <map>
#include <string>
#include <boost/any.hpp>

#define OPEN_LOCKLESS_NAMESPACE namespace lockless {

#define CLOSE_LOCKLESS_NAMESPACE }

#define USING_LOCKLESS_NAMESPACE using namespace lockless;

#define INTERVAL_SECOND 1
#define INTERVAL_MINUTE INTERVAL_SECOND * 60
#define INTERVAL_HOUR INTERVAL_MINUTE * 60

#define KEEP_ALIVE_INTERVAL INTERVAL_SECOND * 1

#define POST_CONTEXT(c) \
std::shared_ptr<runtime_context> context() const { \
    return c; \
}

/**
 * post function definition with 0 params.
 */
#define POST_FUNCTION_0_FUTURE(t, f, _Rp) \
    void post_##f(std::function<void(std::future<_Rp>)> _fut_func) { \
        context()->execute(_fut_func, std::bind(&t::f, this)); \
        f##_posted(); \
    } \
    virtual void f##_posted(){}

/**
 * post function definition with 1 params.
 */
#define POST_FUNCTION_1_FUTURE(t, f, t0, p0, _Rp) \
    void post_##f(t0 p0, std::function<void(std::future<_Rp>)> _fut_func) { \
        context()->execute(_fut_func, std::bind(&t::f, this, p0)); \
        f##_posted(p0); \
    } \
    virtual void f##_posted(t0 p0){}

/**
 * post function definition with 2 params.
 */
#define POST_FUNCTION_2_FUTURE(t, f, t0, p0, t1, p1, _Rp) \
    void post_##f(t0 p0, t1 p1, std::function<void(std::future<_Rp>)> _fut_func) { \
        context()->execute(_fut_func, std::bind(&t::f, this, p0, p1)); \
        f##_posted(p0, p1); \
    } \
    virtual void f##_posted(t0 p0, t1 p1){}

/**
 * post function definition with 3 params.
 */
#define POST_FUNCTION_3_FUTURE(t, f, t0, p0, t1, p1, t2, p2, _Rp) \
    void post_##f(t0 p0, t1 p1, t2 p2, std::function<void(std::future<_Rp>)> _fut_func) { \
        context()->execute(_fut_func, std::bind(&t::f, this, p0, p1, p2)); \
        f##_posted(p0, p1, p2); \
    } \
    virtual void f##_posted(t0 p0, t1 p1, t2 p2){}

//================================== Seperator ==================================//

/**
 * sync function definition with 0 params.
 */
#define SYNC_FUNCTION_0(f) \
    void sync_##f(){ f(); }

/**
 * sync function definition with 1 params.
 */
#define SYNC_FUNCTION_1(f, t0, p0) \
    void sync_##f(t0 p0){ f(p0); }

/**
 * sync function definition with 2 params.
 */
#define SYNC_FUNCTION_2(f, t0, p0, t1, p1) \
    void sync_##f(t0 p0, t1 p1){ f(p0, p1); }

#define GEN_UUID boost::uuids::random_generator()()

#define USING_PERFECT_FORWARD_COMMAND_WRAPPER
//#undef USING_PERFECT_FORWARD_COMMAND_WRAPPER

typedef std::map<std::string, boost::any> ret_map;

#endif // DEFINES_H
