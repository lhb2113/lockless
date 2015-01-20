//
//  data_type.h
//  lockless
//
//  Created by liuhaibo on 14-12-12.
//  Copyright (c) 2014年 liuhaibo. All rights reserved.
//

#ifndef DATA_TYPE_H
#define DATA_TYPE_H

#include "defines.h"

OPEN_LOCKLESS_NAMESPACE

class data_category
{
public:
    virtual const char* name() const _NOEXCEPT = 0;
    virtual std::string message(int _type) const _NOEXCEPT = 0;
public:
    bool operator == (const data_category& __rhs) const _NOEXCEPT {return this == &__rhs;}
    bool operator != (const data_category& __rhs) const _NOEXCEPT {return !(*this == __rhs);}
    bool operator < (const data_category& __rhs) const _NOEXCEPT {return this < &__rhs;}
};

template<typename Type>
struct data_category_trait;

class data_type
{
public:
    data_type(int _type, const data_category& _category) _NOEXCEPT : m_type(_type) , m_catetory(&_category){}
    
    template<typename Type>
    data_type(Type _type) _NOEXCEPT : m_type(static_cast<int>(_type)), m_catetory(&data_category_trait<Type>::category()){}
public:
    int type() const _NOEXCEPT { return m_type; }
    const data_category &category() const _NOEXCEPT { return *m_catetory; }
    std::string message() const _NOEXCEPT { return m_catetory->message(m_type); }
public:
    
private:
    int m_type;
    const data_category* m_catetory;
};

bool operator==(const data_type& __x, const data_type& __y);
bool operator<(const data_type& __x, const data_type& __y);

CLOSE_LOCKLESS_NAMESPACE

#endif //DATA_TYPE_H
