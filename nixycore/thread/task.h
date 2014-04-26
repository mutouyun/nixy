/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/thread/future.h"
#include "nixycore/thread/promise.h"

#include "nixycore/delegate/functor.h"

#include "nixycore/general/general.h"
#include "nixycore/preprocessor/preprocessor.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/utility/utility.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    A task wraps a callable element
    and allows its result to be retrieved asynchronously.
*/

template <typename F>
class task;

#ifdef NX_SP_CXX11_TEMPLATES
template <typename R, typename... P>
class task<R(P...)> : public nx::noncopyable
{
    functor<R(P...)> func_;
    promise<R>       prom_;

public:
    typedef R result_t;

    task(void) {}

    task(nx_rref(task, true) rhs)
    {
        swap(nx::moved(rhs));
    }

    template <typename FuncT>
    task(nx_fref(FuncT, f),
         typename enable_if_diff<FuncT, task, int>::type_t = 0)
        : func_(nx_forward(FuncT, f))
    {}

    void swap(task& rhs)
    {
        func_.swap(rhs.func_);
        prom_.swap(rhs.prom_);
    }

    task& operator=(task rhs)
    {
        rhs.swap(*this);
        return (*this);
    }

public:
    bool is_valid(void) const
    {
        return !!func_;
    }

    nx_rval(future<R>, true) get_future(void)
    {
        return prom_.get_future();
    }

    template <typename... P_>
    void operator()(nx_fref(P_, ... par))
    {
        nx_assert(func_);
        prom_.set_value(func_(nx_forward(P_, par)...));
    }
};
#else /*NX_SP_CXX11_TEMPLATES*/
template <typename R>
class task<R()> : public nx::noncopyable
{
    functor<R()> func_;
    promise<R>   prom_;

public:
    typedef R result_t;

    task(void) {}

    task(nx_rref(task, true) rhs)
    {
        swap(nx::moved(rhs));
    }

    template <typename FuncT>
    task(nx_fref(FuncT, f),
         typename enable_if_diff<FuncT, task, int>::type_t = 0)
        : func_(nx_forward(FuncT, f))
    {}

    void swap(task& rhs)
    {
        func_.swap(rhs.func_);
        prom_.swap(rhs.prom_);
    }

    task& operator=(task rhs)
    {
        rhs.swap(*this);
        return (*this);
    }

public:
    bool is_valid(void) const
    {
        return !!func_;
    }

    nx_rval(future<R>, true) get_future(void)
    {
        return prom_.get_future();
    }

    void operator()(void)
    {
        nx_assert(func_);
        prom_.set_value(func_());
    }
};

#define NX_TASK_DEFINE_(n) \
template <typename R, NX_PP_TYPE_1(n, typename P)> \
class task<R(NX_PP_TYPE_1(n, P))> : public nx::noncopyable \
{ \
    functor<R(NX_PP_TYPE_1(n, P))> func_; \
    promise<R> prom_; \
public: \
    typedef R result_t; \
    task(void) {} \
    task(nx_rref(task, true) rhs) \
    { \
        swap(nx::moved(rhs)); \
    } \
    template <typename FuncT> \
    task(nx_fref(FuncT, f), \
         typename enable_if_diff<FuncT, task, int>::type_t = 0) \
        : func_(nx_forward(FuncT, f)) \
    {} \
    void swap(task& rhs) \
    { \
        func_.swap(rhs.func_); \
        prom_.swap(rhs.prom_); \
    } \
    task& operator=(task rhs) \
    { \
        rhs.swap(*this); \
        return (*this); \
    } \
public: \
    bool is_valid(void) const \
    { \
        return !!func_; \
    } \
    nx_rval(future<R>, true) get_future(void) \
    { \
        return prom_.get_future(); \
    } \
    template <NX_PP_TYPE_1(n, typename P_)> \
    void operator()(NX_PP_TYPE_2(n, P_, NX_PP_FREF(par))) \
    { \
        nx_assert(func_); \
        prom_.set_value(func_(NX_PP_FORWARD(n, P_, par))); \
    } \
};
NX_PP_MULT_MAX(NX_TASK_DEFINE_)
#undef NX_TASK_DEFINE_
#endif/*NX_SP_CXX11_TEMPLATES*/

/*
    Special swap algorithm
*/

template <typename F>
inline void swap(task<F>& x, task<F>& y)
{
    x.swap(y);
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
