/*
    The Nixy Library
    Code covered by the MIT License

    Modified from The Boost Library
    Modified by : mutouyun (http://darkc.at)

    Copyright Kevlin Henney, 2000, 2001, 2002. All rights reserved.
*/

#pragma once

#include "nixycore/bugfix/assert.h"
#include "nixycore/memory/default_alloc.h"
#include "nixycore/pattern/prototype.h"

#include "nixycore/general/general.h"
#include "nixycore/utility/utility.h"
#include "nixycore/algorithm/algorithm.h"

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
        virtual ~PlaceHolder(void) {}

    public:
        virtual const std::type_info& type(void) const = 0;
        virtual PlaceHolder* clone(void) = 0;
        virtual size_t size_of(void) const = 0;
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
        PlaceHolder* clone(void)                { return nx::alloc<Holder>(nx::ref(held_)); }
        size_t size_of(void) const              { return sizeof(Holder); }
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

    any(const rvalue<any>& rhs)
        : content_(nx::nulptr)
    {
        swap(unmove(rhs));
    }

    ~any(void)
    {
        nx::free(content_);
    }

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
    Special swap algorithm
*/

inline void swap(any& x, any& y)
{
    x.swap(y);
}

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
