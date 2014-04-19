/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/thread/blocking_queue.h"
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
#   include "detail/thread_ops_define.hxx"

public:
    typedef functor<void()> task_t;

private:
    mutable mutex lock_;

    handle_t handle_;
    id_t     id_;

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
        nx_memory_scope(thr_dat);
        if (thr_dat->task_queue_)
        {
            nx_memory_scope(thr_dat->task_queue_);
            task_t task;
            while (!!(task = thr_dat->task_queue_->take())) // wait for a new task
                task();
        }
        else thr_dat->proc_();
        return 0;
    }

public:
    thread(void)
        : handle_(0), id_(0), thr_dat_(nx::nulptr)
    {}

    thread(nx_rref(thread) r)
        : handle_(0), id_(0), thr_dat_(nx::nulptr)
    {
        swap(nx::moved(r));
    }

#ifdef NX_SP_CXX11_TEMPLATES
    template <typename F, typename... P>
    thread(nx_fref(F, f), nx_fref(P, ... par))
        : handle_(0), id_(0), thr_dat_(nx::nulptr)
    {
        start(nx_forward(F, f), nx_forward(P, par)...);
    }
#else /*NX_SP_CXX11_TEMPLATES*/
    template <typename F>
    thread(nx_fref(F, f))
        : handle_(0), id_(0), thr_dat_(nx::nulptr)
    {
        start(nx_forward(F, f));
    }

#define NX_THREAD_CONSTRUCT_(n) \
    template <typename F, NX_PP_TYPE_1(n, typename P)> \
    thread(nx_fref(F, f), NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
        : handle_(0), id_(0), thr_dat_(nx::nulptr) \
    { \
        start(nx_forward(F, f), NX_PP_FORWARD(n, P, par)); \
    }
    NX_PP_MULT_MAX(NX_THREAD_CONSTRUCT_)
#undef NX_THREAD_CONSTRUCT_
#endif/*NX_SP_CXX11_TEMPLATES*/

    ~thread(void) { join(); }

public:
    template <typename F>
    void start(nx_fref(F, f))
    {
        nx_lock_scope(lock_);
        if (thr_dat_) thread_ops::join(handle_);
        nx_verify(thr_dat_ = nx::alloc<data>());
        thr_dat_->proc_ = nx_forward(F, f);
        if (!(thr_dat_->proc_))
            nx_verify(thr_dat_->task_queue_ = nx::alloc<blocking_queue<task_t> >());
        else
            thr_dat_->task_queue_ = nx::nulptr;
        handle_ = thread_ops::create(onThreadProc, thr_dat_, &id_);
    }

#ifdef NX_SP_CXX11_TEMPLATES
    template <typename F, typename... P>
    void start(nx_fref(F, f), nx_fref(P, ... par))
    {
        start(bind<void>(nx_forward(F, f), nx_forward(P, par)...));
    }
#else /*NX_SP_CXX11_TEMPLATES*/
#define NX_THREAD_CONSTRUCT_(n) \
    template <typename F, NX_PP_TYPE_1(n, typename P)> \
    void start(nx_fref(F, f), NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
    { \
        start(bind<void>(nx_forward(F, f), NX_PP_FORWARD(n, P, par))); \
    }
    NX_PP_MULT_MAX(NX_THREAD_CONSTRUCT_)
#undef NX_THREAD_CONSTRUCT_
#endif/*NX_SP_CXX11_TEMPLATES*/

    void start(void)
    {
        start(nx::none);
    }

    handle_t handle(void) const
    {
        nx_lock_scope(lock_);
        return handle_;
    }

    id_t id(void) const
    {
        nx_lock_scope(lock_);
        return id_;
    }

    void join(void)
    {
        nx_lock_scope(lock_);
        if (thr_dat_) join(handle_);
        handle_ = 0;
        id_ = 0;
        thr_dat_ = nx::nulptr;
    }

    void detach(void)
    {
        nx_lock_scope(lock_);
        if (thr_dat_) detach(handle_);
        handle_ = 0;
        id_ = 0;
        thr_dat_ = nx::nulptr;
    }

    template <typename F>
    void post(nx_fref(F, f))
    {
        nx_lock_scope(lock_);
        if (thr_dat_ && thr_dat_->task_queue_)
            thr_dat_->task_queue_->put(nx_forward(F, f));
    }

#ifdef NX_SP_CXX11_TEMPLATES
    template <typename F, typename... P>
    void put(nx_fref(F, f), nx_fref(P, ... par))
    {
        put(bind<void>(nx_forward(F, f), nx_forward(P, par)...));
    }
#else /*NX_SP_CXX11_TEMPLATES*/
#define NX_THREAD_CONSTRUCT_(n) \
    template <typename F, NX_PP_TYPE_1(n, typename P)> \
    void post(nx_fref(F, f), NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
    { \
        post(bind<void>(nx_forward(F, f), NX_PP_FORWARD(n, P, par))); \
    }
    NX_PP_MULT_MAX(NX_THREAD_CONSTRUCT_)
#undef NX_THREAD_CONSTRUCT_
#endif/*NX_SP_CXX11_TEMPLATES*/

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
