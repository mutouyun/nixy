/*
    Modified from The Boost Library
    Modified by : 2013 darkcat

    Copyright Kevlin Henney, 2000, 2001, 2002. All rights reserved.

    Distributed under the Boost Software License, Version 1.0. (See
    accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
*/

#pragma once

#include "bugfix/assert.h"
#include "memory/alloc.h"
#include "pattern/prototype.h"

#include "general/general.h"
#include "utility/utility.h"
#include "algorithm/algorithm.h"

// typeid
#include <typeinfo>

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

class any
{
protected:
    class PlaceHolder
    {
    public:
        virtual ~PlaceHolder() {}

    public:
        virtual const std::type_info& type(void) const = 0;
        virtual PlaceHolder* clone(void) = 0;
    };

    template <typename T>
    class Holder : public PlaceHolder
    {
    public:
        T held_;

    public:
        Holder(const T& v): held_(v) {}

    public:
        const std::type_info & type(void) const { return typeid(T); }
        PlaceHolder* clone(void) { return nx::alloc<Holder>(nx::ref(held_)); }
    };

protected:
    PlaceHolder* content_;

public:
    any(void)
        : content_(nx::nulptr)
    {}

    template <typename T>
    any(const T& v)
        : content_(nx::alloc<Holder<T> >(nx::ref(v)))
    {}

    any(const any& other)
        : content_(nx::clone(other.content_))
    {}

    ~any()
    { nx::free(content_); }

public:
    void swap(any& rhs)
    {
        nx::swap(content_, rhs.content_);
    }

    any& operator=(any rhs)
    {
        rhs.swap(*this);
        return (*this);
    }

public:
    const std::type_info & type() const
    {
        return (content_ ? content_->type() : typeid(void));
    }
    
    bool empty() const
    {
        return (!content_);
    }
    
    void clear()
    {
        any().swap(*this);
    }

private:
    template <typename T> friend T any_cast(any&);
    template <typename T> friend T any_cast(const any&);
};

/*
    cast any to a exact type
*/

template <typename T>
T any_cast(any& operand)
{
    nx_assert(operand.type() == typeid(T));
    return static_cast<any::Holder<T>*>(operand.content_)->held_;
}

template <typename T>
T any_cast(const any& operand)
{
    return any_cast<T>(const_cast<any&>(operand));
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
