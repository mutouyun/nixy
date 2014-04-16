/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/thread/lock_guard.h"
#include "nixycore/thread/mutex.h"
#include "nixycore/thread/thread_pool.h"

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
    task status enum
*/

enum task_status_t
{
    task_Deferred,
    task_Ready,
    task_TimeOut
};

/*
    private definitions
*/

namespace private_task
{
    /*
        task data storage
    */

    template <typename T>
    struct data_base
    {
        mutable mutex lock_;
        condition     done_;
        task_status_t status_;

        functor<T()> task_;

        data_base(void)
            : done_(lock_)
            , status_(task_Deferred)
        {}
    };

    template <typename T>
    struct data : data_base<T>
    {
        typename rm_rvalue<T>::type_t result_;

        static void onProcess(pointer<data>& d)
        {
            d->result_ = d->task_(); // no need lock, because result can be access only after wait
            nx_lock_scope(d->lock_);
            d->status_ = task_Ready;
            d->done_.notify();
        }
    };

    template <>
    struct data<void> : data_base<void>
    {
        static void onProcess(pointer<data>& d)
        {
            d->task_();
            nx_lock_scope(d->lock_);
            d->status_ = task_Ready;
            d->done_.notify();
        }
    };

    /*
        package the preparations for the task
    */

    template <typename T>
    class prepare
    {
    public:
        pointer<data<T> > data_;

        template <typename F>
        prepare(nx_fref(F, f),
                typename nx::enable_if<!is_sametype<F, prepare>::value, int>::type_t = 0)
            : data_(nx::alloc<data<T> >())
        {
            nx_assert(data_);
            nx_verify(data_->task_ = nx_forward(F, f));
            start();
        }

        prepare(const prepare& pp)
            : data_(nx::move(pp.data_)) // always get the ownership of the data
        {
            nx_assert(data_);
        }

    private:
        void start(void)
        {
            nx_assert(data_);
            singleton<thread_pool>(0, (size_t)~0)
                .put(bind(&data<T>::onProcess, data_));
        }
    };
}

/*
    async task
*/

template <typename T>
class task
{
public:
    typedef T type_t;

private:
    typedef private_task::data<type_t> data_t;

    pointer<data_t> data_;

    task& operator=(const task&); // = deleted

public:
    task(const private_task::prepare<type_t>& pp)
        : data_(nx::move(pp.data_))
    {
        nx_assert(data_);
    }

    /*
        gcc need this, or will get a compile error
        when using like this: task<T> xx = async(F)
    */
    task(const task&) { nx_assert(false); } // = deleted

public:
    bool wait(int tm_ms = -1)
    {
        nx_assert(data_);
        nx_lock_scope(data_->lock_);
        while (data_->status_ != task_Ready) // used to avoid spurious wakeups
        {
            bool ret = data_->done_.wait(tm_ms);
            if (!ret)
            {
                data_->status_ = task_TimeOut;
                return false;
            }
        }
        return true;
    }

    task_status_t status(void) const
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
};

/*
    async call, will return a task object for controlling asynchronous call
*/

#ifdef NX_SP_CXX11_TEMPLATES
template <typename F, typename... P>
inline private_task::prepare<typename function_traits<F>::result_t>
    async(nx_fref(F, f), nx_fref(P, ... par))
{
    return private_task::prepare<typename function_traits<F>::result_t>
        (bind(nx_forward(F, f), nx_forward(P, par)...));
}
template <typename R, typename F, typename... P>
inline private_task::prepare<R>
    async(nx_fref(F, f), nx_fref(P, ... par))
{
    return private_task::prepare<R>
        (bind<R>(nx_forward(F, f), nx_forward(P, par)...));
}
#else /*NX_SP_CXX11_TEMPLATES*/
template <typename F>
inline private_task::prepare<typename function_traits<F>::result_t> async(nx_fref(F, f))
{
    return private_task::prepare<typename function_traits<F>::result_t>(nx_forward(F, f));
}

template <typename R, typename F>
inline private_task::prepare<R> async(nx_fref(F, f))
{
    return private_task::prepare<R>(nx_forward(F, f));
}

#define NX_ASYNC_(n) \
template <typename F, NX_PP_TYPE_1(n, typename P)> \
inline private_task::prepare<typename function_traits<F>::result_t> \
    async(nx_fref(F, f), NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
{ \
    return private_task::prepare<typename function_traits<F>::result_t> \
        (bind(nx_forward(F, f), NX_PP_FORWARD(n, P, par))); \
} \
template <typename R, typename F, NX_PP_TYPE_1(n, typename P)> \
inline private_task::prepare<R> \
    async(nx_fref(F, f), NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
{ \
    return private_task::prepare<R> \
        (bind<R>(nx_forward(F, f), NX_PP_FORWARD(n, P, par))); \
}
NX_PP_MULT_MAX(NX_ASYNC_)
#undef NX_ASYNC_
#endif/*NX_SP_CXX11_TEMPLATES*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
