/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/thread/waiter.h"

#include "nixycore/bugfix/assert.h"

#include "nixycore/general/general.h"
#include "nixycore/utility/utility.h"
#include "nixycore/memory/memory.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    Future data storage
*/

namespace private_future
{
    struct storage_base
    {
        waiter waiter_;

        bool wait(int tm_ms = -1)
        {
            return waiter_.wait(tm_ms);
        }

        void notify(void)
        {
            waiter_.broadcast();
        }
    };
}

template <typename T, bool = nx::is_reference<T>::value>
class future_storage : public private_future::storage_base
{
    typedef typename rm_rvalue<T>::type_t value_t;
    value_t content_;

public:
    future_storage(void) : content_() {}

    value_t& get(void)
    {
        wait();
        return content_;
    }

    void set(const value_t& val)
    {
        content_ = val;
        notify();
    }

    void set(nx_rref(value_t) val)
    {
        content_ = nx::move(val);
        notify();
    }
};

template <typename T>
class future_storage<T, true> : public private_future::storage_base
{
    typedef typename rm_reference<T>::type_t* value_t;
    value_t content_;

public:
    future_storage(void) : content_(nx::nulptr) {}

    T get(void)
    {
        wait();
        return (*content_);
    }

    void set(T val)
    {
        content_ = &val;
        notify();
    }
};

template <>
class future_storage<void, false> : public private_future::storage_base
{
public:
    void get(void)
    {
        wait();
    }

    void set(void)
    {
        notify();
    }
};

/*
    A future is an object that can retrieve a value
    from some provider object or function,
    properly synchronizing this access if in different threads.
*/

template <typename T>
class future : nx::noncopyable
{
public:
    typedef T type_t;

private:
    typedef future_storage<type_t> storage_t;
    pointer<storage_t> data_ptr_;

public:
    future(void) {}

    future(const pointer<storage_t>& data_ptr)
        : data_ptr_(data_ptr)
    {
        nx_assert(data_ptr_);
    }

    future(nx_rref(future, true) rhs)
    {
        swap(nx::moved(rhs));
    }

    void swap(future& rhs)
    {
        data_ptr_.swap(rhs.data_ptr_);
    }

    future& operator=(future rhs)
    {
        rhs.swap(*this);
        return (*this);
    }

public:
    bool wait(int tm_ms = -1)
    {
        return data_ptr_ ? data_ptr_->wait(tm_ms) : false;
    }

    bool is_valid() const
    {
        return !!data_ptr_;
    }

    type_t get(void)
    {
        nx_assert(data_ptr_);
        return data_ptr_->get();
    }

    nx_rval(future<T>, true) share(void)
    {
        return future<T>(data_ptr_);
    }
};

/*
    Special swap algorithm
*/

template <typename T>
inline void swap(future<T>& x, future<T>& y)
{
    x.swap(y);
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
