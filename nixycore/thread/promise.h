/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#pragma once

#include "nixycore/thread/future.h"

#include "nixycore/general/general.h"
#include "nixycore/utility/utility.h"
#include "nixycore/memory/memory.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

namespace private_promise
{
    template <typename T>
    class base
    {
    public:
        typedef T type_t;

    protected:
        typedef future_storage<type_t> storage_t;
        pointer<storage_t> data_ptr_;

    public:
        base(void)
            : data_ptr_(nx::alloc<storage_t>())
        {
            nx_assert(data_ptr_);
        }

        base(nx::none_t) {}

        void swap(base& rhs)
        {
            data_ptr_.swap(rhs.data_ptr_);
        }

    public:
        nx_rval(future<T>, true) get_future(void)
        {
            return future<T>(data_ptr_);
        }
    };

    template <typename T, bool = nx::is_reference<T>::value>
    class detail : public base<T>
    {
        typedef base<T> base_t;
        using base_t::data_ptr_;

    public:
        typedef typename base_t::type_t type_t;

    public:
#   ifdef NX_SP_CXX11_INHERITING
        using base_t::base;
#   else /*NX_SP_CXX11_INHERITING*/
        detail(void)       : base_t()         {}
        detail(nx::none_t) : base_t(nx::none) {}
#   endif/*NX_SP_CXX11_INHERITING*/

    public:
        void set_value(const type_t& val)
        {
            nx_assert(data_ptr_);
            data_ptr_->set(val);
        }

        void set_value(nx_rref(type_t) val)
        {
            nx_assert(data_ptr_);
            data_ptr_->set(nx::move(val));
        }
    };

    template <typename T>
    class detail<T, true> : public base<T>
    {
        typedef base<T> base_t;
        using base_t::data_ptr_;

    public:
        typedef typename base_t::type_t type_t;

    public:
#   ifdef NX_SP_CXX11_INHERITING
        using base_t::base;
#   else /*NX_SP_CXX11_INHERITING*/
        detail(void)       : base_t()         {}
        detail(nx::none_t) : base_t(nx::none) {}
#   endif/*NX_SP_CXX11_INHERITING*/

    public:
        void set_value(type_t val)
        {
            nx_assert(data_ptr_);
            data_ptr_->set(val);
        }
    };

    template <>
    class detail<void, false> : public base<void>
    {
        typedef base<void> base_t;
        using base_t::data_ptr_;

    public:
        typedef base_t::type_t type_t;

    public:
#   ifdef NX_SP_CXX11_INHERITING
        using base_t::base;
#   else /*NX_SP_CXX11_INHERITING*/
        detail(void)       : base_t()         {}
        detail(nx::none_t) : base_t(nx::none) {}
#   endif/*NX_SP_CXX11_INHERITING*/

    public:
        void set_value(void)
        {
            nx_assert(data_ptr_);
            data_ptr_->set();
        }
    };
}

/*
    A promise is an object that can store a value of type T
    to be retrieved by a future object (possibly in another thread),
    offering a synchronization point.
*/

template <typename T>
class promise : public private_promise::detail<T>, nx::noncopyable
{
    typedef private_promise::detail<T> base_t;

public:
    typedef typename base_t::type_t type_t;

public:
    promise(void)
        : base_t()
    {}

    promise(nx_rref(promise, true) rhs)
        : base_t(nx::none)
    {
        base_t::swap(nx::moved(rhs));
    }

    promise& operator=(promise rhs)
    {
        rhs.swap(*this);
        return (*this);
    }
};

/*
    Special swap algorithm
*/

template <typename T>
inline void swap(promise<T>& x, promise<T>& y)
{
    x.swap(y);
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
