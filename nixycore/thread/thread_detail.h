/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#pragma once

#include "nixycore/thread/blocking_queue.h"
#include "nixycore/thread/thread_ops.h"

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
    handle_t handle_;
    id_t     id_;

    struct data
    {
        task_t proc_;
        blocking_queue<task_t>* task_queue_;
    };

    data* thr_dat_;

    void init(void)
    {
        thread_ops::clear_hd(handle_);
        thread_ops::clear_id(id_);
        thr_dat_ = nx::nulptr;
    }

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

    template <typename F>
    void create_start(nx_fref(F) f)
    {
        if (thr_dat_) thread_ops::join(handle_);
        nx_verify(thr_dat_ = nx::alloc<data>());
        thr_dat_->proc_ = nx_forward(F, f);
        if (thr_dat_->proc_)
            thr_dat_->task_queue_ = nx::nulptr;
        else
            nx_verify(thr_dat_->task_queue_ = nx::alloc<blocking_queue<task_t> >());
        handle_ = thread_ops::create(onThreadProc, thr_dat_, &id_);
    }

public:
    thread(void)
    {
        init();
    }

    thread(nx_rref(thread) r)
    {
        init();
        swap(nx::moved(r));
    }

#ifdef NX_SP_CXX11_TEMPLATES
    template <typename F, typename... P>
    explicit thread(nx_fref(F) f, nx_fref(P)... par)
    {
        init();
        start(nx_forward(F, f), nx_forward(P, par)...);
    }
#else /*NX_SP_CXX11_TEMPLATES*/
    template <typename F>
    explicit thread(nx_fref(F) f)
    {
        init();
        start(nx_forward(F, f));
    }

#define NX_THREAD_CONSTRUCT_(n) \
    template <typename F, NX_PP_TYPE_1(n, typename P)> \
    explicit thread(nx_fref(F) f, NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
    { \
        init(); \
        start(nx_forward(F, f), NX_PP_FORWARD(n, P, par)); \
    }
    NX_PP_MULT_MAX(NX_THREAD_CONSTRUCT_)
#undef NX_THREAD_CONSTRUCT_
#endif/*NX_SP_CXX11_TEMPLATES*/

    ~thread(void) { join(); }

public:
#ifdef NX_SP_CXX11_TEMPLATES
    template <typename F, typename... P>
    void start(nx_fref(F) f, nx_fref(P)... par)
    {
        create_start(bind<void>(nx_forward(F, f), nx_forward(P, par)...));
    }
#else /*NX_SP_CXX11_TEMPLATES*/
    template <typename F>
    void start(nx_fref(F) f)
    {
        create_start(bind<void>(nx_forward(F, f)));
    }

#define NX_THREAD_CONSTRUCT_(n) \
    template <typename F, NX_PP_TYPE_1(n, typename P)> \
    void start(nx_fref(F) f, NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
    { \
        create_start(bind<void>(nx_forward(F, f), NX_PP_FORWARD(n, P, par))); \
    }
    NX_PP_MULT_MAX(NX_THREAD_CONSTRUCT_)
#undef NX_THREAD_CONSTRUCT_
#endif/*NX_SP_CXX11_TEMPLATES*/

    void start(void)
    {
        create_start(nx::none);
    }

    handle_t handle(void) const
    {
        return handle_;
    }

    id_t id(void) const
    {
        return id_;
    }

    bool joinable(void) const
    {
        return !!(thr_dat_);
    }

    void join(void)
    {
        if (joinable()) join(handle_);
        init();
    }

    void detach(void)
    {
        if (joinable()) detach(handle_);
        init();
    }

    template <typename F>
    void post(nx_fref(F) f)
    {
        if (thr_dat_ && thr_dat_->task_queue_)
            thr_dat_->task_queue_->put(nx_forward(F, f));
    }

#ifdef NX_SP_CXX11_TEMPLATES
    template <typename F, typename... P>
    void put(nx_fref(F) f, nx_fref(P)... par)
    {
        put(bind<void>(nx_forward(F, f), nx_forward(P, par)...));
    }
#else /*NX_SP_CXX11_TEMPLATES*/
#define NX_THREAD_CONSTRUCT_(n) \
    template <typename F, NX_PP_TYPE_1(n, typename P)> \
    void post(nx_fref(F) f, NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
    { \
        post(bind<void>(nx_forward(F, f), NX_PP_FORWARD(n, P, par))); \
    }
    NX_PP_MULT_MAX(NX_THREAD_CONSTRUCT_)
#undef NX_THREAD_CONSTRUCT_
#endif/*NX_SP_CXX11_TEMPLATES*/

    void swap(thread& r)
    {
        nx::swap(handle_ , r.handle_);
        nx::swap(id_     , r.id_);
        nx::swap(thr_dat_, r.thr_dat_);
    }
};

/*
    Special swap algorithm
*/

inline void swap(thread& x, thread& y)
{
    x.swap(y);
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
