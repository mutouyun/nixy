/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/thread/threadops.h"
#include "nixycore/thread/blockingqueue.h"
#include "nixycore/thread/mutex.h"

#include "nixycore/delegate/functor.h"
#include "nixycore/delegate/bind.h"

#include "nixycore/bugfix/assert.h"

#include "nixycore/general/general.h"
#include "nixycore/preprocessor/preprocessor.h"
#include "nixycore/utility/utility.h"
#include "nixycore/algorithm/algorithm.h"
#include "nixycore/memory/memory.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

class thread : noncopyable
{
public:
    typedef functor<void()> task_t;

private:
    mutable mutex lock_;

    thread_ops::handle_t handle_;
    thread_ops::id_t     id_;

    struct data
    {
        task_t proc_;
        blocking_queue<task_t>* task_queue_;
    };

    data* thr_dat_;

    static NX_THREAD_PROC(onThreadProc, args)
    {
        data* thr_dat = static_cast<data*>(args);
        nx_assert(thr_dat);
        nx_guard_scope(thr_dat);
        if (thr_dat->task_queue_)
        {
            nx_guard_scope(thr_dat->task_queue_);
            task_t task;
            while (task = thr_dat->task_queue_->take()) // wait for a new task
                task();
        }
        else thr_dat->proc_();
        return 0;
    }

public:
    thread(void)
        : handle_(0), id_(0), thr_dat_(nx::nulptr)
    {}

    thread(const rvalue<thread>& r)
        : handle_(0), id_(0), thr_dat_(nx::nulptr)
    {
        swap(nx::unmove(r));
    }

    template <typename F>
    thread(const F& f)
        : handle_(0), id_(0), thr_dat_(nx::nulptr)
    {
        start(f);
    }

#define NX_THREAD_CONSTRUCT_(n) \
    template <typename F, NX_PP_TYPE_1(n, typename P)> \
    thread(const F& f, NX_PP_TYPE_2(n, P, par)) \
        : handle_(0), id_(0), thr_dat_(nx::nulptr) \
    { \
        start(f, NX_PP_TYPE_1(n, par)); \
    }
    NX_PP_MULT_MAX(NX_THREAD_CONSTRUCT_)
#undef NX_THREAD_CONSTRUCT_

    ~thread() { join(); }

public:
    template <typename F>
    void start(const F& f)
    {
        nx_lock_scope(lock_);
        if (thr_dat_) thread_ops::join(handle_);
        nx_verify(thr_dat_ = nx::alloc<data>());
        thr_dat_->proc_ = f;
        if (!(thr_dat_->proc_))
            nx_verify(thr_dat_->task_queue_ = nx::alloc<blocking_queue<task_t> >());
        else
            thr_dat_->task_queue_ = nx::nulptr;
        handle_ = thread_ops::create(onThreadProc, thr_dat_, &id_);
    }

#define NX_THREAD_CONSTRUCT_(n) \
    template <typename F, NX_PP_TYPE_1(n, typename P)> \
    void start(const F& f, NX_PP_TYPE_2(n, P, par)) \
    { \
        start(bind<void>(f, NX_PP_TYPE_1(n, par))); \
    }
    NX_PP_MULT_MAX(NX_THREAD_CONSTRUCT_)
#undef NX_THREAD_CONSTRUCT_

    void start(void)
    {
        start(nx::none);
    }

    thread_ops::handle_t handle(void) const
    {
        nx_lock_scope(lock_);
        return handle_;
    }

    thread_ops::id_t id(void) const
    {
        nx_lock_scope(lock_);
        return id_;
    }

    void join(void)
    {
        nx_lock_scope(lock_);
        if (thr_dat_)
            thread_ops::join(handle_);
        handle_ = 0;
        id_ = 0;
        thr_dat_ = nx::nulptr;
    }

    void detach(void)
    {
        nx_lock_scope(lock_);
        if (thr_dat_)
            thread_ops::detach(handle_);
        handle_ = 0;
        id_ = 0;
        thr_dat_ = nx::nulptr;
    }

    void post(const task_t& t)
    {
        nx_lock_scope(lock_);
        if (thr_dat_ && thr_dat_->task_queue_)
            thr_dat_->task_queue_->put(t);
    }

#define NX_THREAD_CONSTRUCT_(n) \
    template <typename F, NX_PP_TYPE_1(n, typename P)> \
    void post(const F& f, NX_PP_TYPE_2(n, P, par)) \
    { \
        post(bind<void>(f, NX_PP_TYPE_1(n, par))); \
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
