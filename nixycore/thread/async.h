/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/thread/lockguard.h"
#include "nixycore/thread/mutex.h"
#include "nixycore/thread/threadpool.h"

#include "nixycore/pattern/singleton.h"

#include "nixycore/delegate/bind.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/preprocessor/preprocessor.h"
#include "nixycore/utility/utility.h"
#include "nixycore/memory/memory.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    async task
*/

template <typename T>
class task
{
public:
    typedef T type_t;

    enum status_t
    {
        Deferred,
        Ready,
        TimeOut
    };

private:
    struct data_base
    {
        mutable mutex lock_;
        condition     done_;
        status_t      status_;

        functor<type_t()> task_;

        data_base(void)
            : done_(lock_)
            , status_(Deferred)
        {}
    };

    template <typename U, typename = nx::null_t>
    struct data : data_base
    {
        typename rm_rvalue<U>::type_t result_;

        static void onProcess(pointer<data>& d)
        {
            d->result_ = d->task_(); // no need lock
            nx_lock_scope(d->lock_);
            d->status_ = Ready;
            d->done_.notify();
        }
    };

    template <typename Dummy_>
    struct data<void, Dummy_> : data_base
    {
        static void onProcess(pointer<data>& d)
        {
            d->task_();
            nx_lock_scope(d->lock_);
            d->status_ = Ready;
            d->done_.notify();
        }
    };

    typedef data<type_t> data_t;
    pointer<data_t> data_;

    task& operator=(const task&); /* = delete */

public:
    task(const functor<type_t()>& t)
        : data_(nx::alloc<data_t>())
    {
        nx_assert(data_);
        nx_verify(data_->task_ = nx::move(t)); // always get the ownership of the data
    }

    task(const task& r)
    {
        swap(const_cast<task&>(r));
    }

public:
    task& start(void)
    {
        nx_assert(data_);
        singleton<thread_pool>(0, limit_of<size_t>::upper)
            .put(bind(&data_t::onProcess, data_));
        return (*this);
    }

    bool wait(int tm_ms = -1)
    {
        nx_assert(data_);
        nx_lock_scope(data_->lock_);
        while (data_->status_ != Ready) // used to avoid spurious wakeups
        {
            bool ret = data_->done_.wait(tm_ms);
            if (!ret)
            {
                data_->status_ = TimeOut;
                return false;
            }
        }
        return true;
    }

    status_t status(void) const
    {
        nx_assert(data_);
        nx_lock_scope(data_->lock_);
        return data_->status_;
    }

    typename enable_if<!is_sametype<type_t, void>::value,
    type_t>::type_t result(void)
    {
        wait();
        return data_->result_;
    }

    void swap(task& r)
    {
        data_.swap(r.data_);
    }
};

/*
    async call, will return a task object for controlling asynchronous call
*/

template <typename F>
inline rvalue<task<typename function_traits<F>::result_t> > async(F f)
{
    return task<typename function_traits<F>::result_t>(f).start();
}

template <typename R, typename F>
inline rvalue<task<R> > async(F f)
{
    return task<R>(f).start();
}

#define NX_ASYNC_(n) \
template <typename F, NX_PP_TYPE_1(n, typename P)> \
inline rvalue<task<typename function_traits<F>::result_t> > async(F f, NX_PP_TYPE_2(n, P, p)) \
{ \
    return task<typename function_traits<F>::result_t>(bind(f, NX_PP_TYPE_1(n, p))).start(); \
} \
template <typename R, typename F, NX_PP_TYPE_1(n, typename P)> \
inline rvalue<task<R> > async(F f, NX_PP_TYPE_2(n, P, p)) \
{ \
    return task<R>(bind<R>(f, NX_PP_TYPE_1(n, p))).start(); \
}
NX_PP_MULT_MAX(NX_ASYNC_)
#undef NX_ASYNC_

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
