/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "thread/threadops.h"
#include "thread/blockingqueue.h"

#include "delegate/functor.h"
#include "delegate/bind.h"

#include "bugfix/assert.h"

#include "general/general.h"
#include "preprocessor/preprocessor.h"
#include "utility/utility.h"
#include "algorithm/algorithm.h"
#include "memory/memory.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

class thread : NonCopyable
{
    thread_ops::handle_t handle_;
    thread_ops::id_t     id_;

    typedef functor<void()> task_t;

    struct data
    {
        task_t proc_;
        blocking_queue<task_t> task_queue_;
    };

    data* thr_dat_;
    spin_lock lock_;

    static NX_THREAD_PROC(onThreadProc, args)
    {
        data* thr_dat = static_cast<data*>(args);
        nx_assert(thr_dat);
        nx_guard_scope(thr_dat);
        if (thr_dat->proc_)
            thr_dat->proc_();
        else nx_forever()
        {
            task_t task = thr_dat->task_queue_.take(); // wait for a new task
            if (task) task();
            else break;
        }
        return 0;
    }

    void start(const task_t& t)
    {
        nx_verify(thr_dat_ = nx::alloc<data>());
        thr_dat_->proc_ = t;
        handle_ = thread_ops::create(onThreadProc, thr_dat_, &id_);
    }

    void start(task_t& t)
    {
        nx_verify(thr_dat_ = nx::alloc<data>());
        thr_dat_->proc_.swap(t);
        handle_ = thread_ops::create(onThreadProc, thr_dat_, &id_);
    }

public:
    thread(void)
        : handle_(0), id_(0), thr_dat_(nx::nulptr)
    { start(nx::none); }

    thread(const rvalue<thread>& r)
        : handle_(0), id_(0), thr_dat_(nx::nulptr)
    { swap(nx::unmove(r)); }

    thread(const task_t& t)
        : handle_(0), id_(0), thr_dat_(nx::nulptr)
    { start(t); }

#define NX_THREAD_CONSTRUCT_(n) \
    template <typename F, NX_PP_TYPE_1(n, typename P)> \
    thread(const F& f, NX_PP_TYPE_2(n, P, par)) \
        : handle_(0), id_(0), thr_dat_(nx::nulptr) \
    { start(bind<void>(f, NX_PP_TYPE_1(n, par))); }
    NX_PP_MULT_MAX(NX_THREAD_CONSTRUCT_)
#undef NX_THREAD_CONSTRUCT_

    ~thread() { join(); }

public:
    thread_ops::handle_t handle(void) const
    {
        return handle_;
    }

    thread_ops::id_t id(void) const
    {
        return id_;
    }

    void join(void)
    {
        thread_ops::join(handle_);
    }

    void detach(void)
    {
        thread_ops::detach(handle_);
    }

    void post(const task_t& t)
    {
        if (thr_dat_)
            thr_dat_->task_queue_.put(t);
    }

#define NX_THREAD_CONSTRUCT_(n) \
    template <typename F, NX_PP_TYPE_1(n, typename P)> \
    void post(const F& f, NX_PP_TYPE_2(n, P, par)) \
    { \
        if (thr_dat_) \
            thr_dat_->task_queue_.put(bind<void>(f, NX_PP_TYPE_1(n, par))); \
    }
    NX_PP_MULT_MAX(NX_THREAD_CONSTRUCT_)
#undef NX_THREAD_CONSTRUCT_

private:
    void swap(thread& r)
    {
        nx::swap(handle_ , r.handle_);
        nx::swap(id_     , r.id_);
        nx::swap(thr_dat_, r.thr_dat_);
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
