#ifndef OBJECT_H
#define OBJECT_H

#include <memory>
#include <boost/noncopyable.hpp>
#include <boost/uuid/uuid.hpp>

#include "defines.h"

OPEN_LOCKLESS_NAMESPACE

extern std::atomic<int> g_object_count;

class object : public std::enable_shared_from_this<object>, public boost::noncopyable
{
public:
    object();
    virtual ~object();
public:

    template<typename Type>
    typename std::shared_ptr<Type> shared(const Type */*_this*/)
    {
        return std::dynamic_pointer_cast<Type>(shared_from_this());
    }
};

#define shared_this shared(this)

CLOSE_LOCKLESS_NAMESPACE

#endif // OBJECT_H
