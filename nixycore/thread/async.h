/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/thread/thread_pool.h"
#include "nixycore/thread/future.h"
#include "nixycore/thread/promise.h"
#include "nixycore/thread/task.h"

#include "nixycore/pattern/singleton.h"
#include "nixycore/delegate/function_traits.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/preprocessor/preprocessor.h"
#include "nixycore/utility/utility.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    Async proc
*/

namespace private_async
{
    template <typename F>
    class detail
    {
        typedef task<F> task_t;
        typedef typename task_t::result_t r_t;

        task_t task_;

        thread_pool& pool_instance(void)
        {
            return nx::singleton<thread_pool>(0, (size_t)~0);
        }

    public:
        template <typename FuncT>
        detail(nx_fref(FuncT) f)
            : task_(nx_forward(FuncT, f))
        {}

#   ifdef NX_SP_CXX11_TEMPLATES
        template <typename... P>
        nx_rval(future<r_t>, true) start(nx_fref(P)... par)
        {
            future<r_t> futr = task_.get_future();
            pool_instance().put(nx_pass(nx::move(task_)), nx_forward(P, par)...);
            return nx::move(futr);
        }
#   else /*NX_SP_CXX11_TEMPLATES*/
        nx_rval(future<r_t>, true) start(void)
        {
            future<r_t> futr = task_.get_future();
            pool_instance().put(nx_pass(nx::move(task_)));
            return nx::move(futr);
        }

#   define NX_ASYNC_DETAIL_DEFINE_(n) \
        template <NX_PP_TYPE_1(n, typename P)> \
        nx_rval(future<r_t>, true) start(NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
        { \
            future<r_t> futr = task_.get_future(); \
            pool_instance().put(nx_pass(nx::move(task_)), NX_PP_FORWARD(n, P, par)); \
            return nx::move(futr); \
        }
        NX_PP_MULT_MAX(NX_ASYNC_DETAIL_DEFINE_)
#   undef NX_ASYNC_DETAIL_DEFINE_
#   endif/*NX_SP_CXX11_TEMPLATES*/
    };
}

/*
    Calls fn (with args as arguments) at some point,
    returning without waiting for the execution of fn to complete.
*/

#ifdef NX_SP_CXX11_TEMPLATES
template <typename F, typename... P>
inline nx_rval(future<typename result_of<F(P...)>::type_t>, true)
    async(nx_fref(F) f, nx_fref(P)... par)
{
    typedef typename result_of<F(P...)>::type_t r_t;
    return private_async::detail<r_t(P...)>(nx_forward(F, f)).start(nx_forward(P, par)...);
}
template <typename R, typename F, typename... P>
inline nx_rval(future<R>, true) async(nx_fref(F) f, nx_fref(P)... par)
{
    return private_async::detail<R(P...)>(nx_forward(F, f)).start(nx_forward(P, par)...);
}
#else /*NX_SP_CXX11_TEMPLATES*/
template <typename F>
inline nx_rval(future<typename result_of<F()>::type_t>, true)
    async(nx_fref(F) f)
{
    typedef typename result_of<F()>::type_t r_t;
    return private_async::detail<r_t()>(nx_forward(F, f)).start();
}
template <typename R, typename F>
inline nx_rval(future<R>, true) async(nx_fref(F) f)
{
    return private_async::detail<R()>(nx_forward(F, f)).start();
}

#define NX_ASYNC_DEFINE_(n) \
template <typename F, NX_PP_TYPE_1(n, typename P)> \
inline nx_rval(future<typename result_of<F(NX_PP_TYPE_1(n, P))>::type_t>, true) \
    async(nx_fref(F) f, NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
{ \
    typedef typename result_of<F(NX_PP_TYPE_1(n, P))>::type_t r_t; \
    return private_async::detail<r_t(NX_PP_TYPE_1(n, P))>(nx_forward(F, f)).start(NX_PP_FORWARD(n, P, par)); \
} \
template <typename R, typename F, NX_PP_TYPE_1(n, typename P)> \
inline nx_rval(future<R>, true) async(nx_fref(F) f, NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
{ \
    return private_async::detail<R(NX_PP_TYPE_1(n, P))>(nx_forward(F, f)).start(NX_PP_FORWARD(n, P, par)); \
}
NX_PP_MULT_MAX(NX_ASYNC_DEFINE_)
#undef NX_ASYNC_DEFINE_
#endif/*NX_SP_CXX11_TEMPLATES*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
