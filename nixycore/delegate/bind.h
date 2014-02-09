/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/delegate/functiontraits.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/utility/utility.h"
#include "nixycore/preprocessor/preprocessor.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    The placeholders for binding args
*/

template <int N> struct arg {};

#define NX_BIND_ARG_(n, ...) static arg<n> NX_UNUSED _##n;
NX_PP_REPEAT_MAX(NX_BIND_ARG_)
#undef NX_BIND_ARG_

/*
    Bind detail
*/

namespace private_bind
{
    /*
        Check result type
    */

    template <typename F, typename R>
    struct check_result
    {
        typedef R type_t;
    };

    template <typename F>
    struct check_result<F, nx::null_t>
    {
        typedef typename function_traits<F>::result_t type_t;
    };

    /*
        Simple functor for saving function pointer
    */

    template <typename F, typename R = nx::null_t
                        , bool = nx::is_pointer<F>::value
                        , bool = nx::is_mem_function<F>::value>
    class fr;

    template <typename F>
    class fr<F, void, false, false>
    {
        F f_;

    public:
        typedef void result_t;

        fr(const typename nx::rm_const<F>::type_t& f) : f_(f) {}

        result_t operator()(void)
        {
            f_();
        }

#   define NX_BIND_FR_(n) \
        template <NX_PP_TYPE_1(n, typename P)> \
        result_t operator()(NX_PP_TYPE_2(n, P, p)) \
        { \
            f_(NX_PP_TYPE_1(n, p)); \
        }
        NX_PP_MULT_MAX(NX_BIND_FR_)
#   undef NX_BIND_FR_
    };

    template <typename F, typename R>
    class fr<F, R, false, false>
    {
        F f_;

    public:
        typedef typename check_result<F, R>::type_t result_t;

        fr(const typename nx::rm_const<F>::type_t& f) : f_(f) {}

        result_t operator()(void)
        {
            return f_();
        }

#   define NX_BIND_FR_(n) \
        template <NX_PP_TYPE_1(n, typename P)> \
        result_t operator()(NX_PP_TYPE_2(n, P, p)) \
        { \
            return f_(NX_PP_TYPE_1(n, p)); \
        }
        NX_PP_MULT_MAX(NX_BIND_FR_)
#   undef NX_BIND_FR_
    };

    template <typename F>
    class fr<F, void, true, false>
    {
        F f_;

    public:
        typedef void result_t;

        fr(F f) : f_(f) {}

        result_t operator()(void)
        {
            (*f_)();
        }

#   define NX_BIND_FR_(n) \
        template <NX_PP_TYPE_1(n, typename P)> \
        result_t operator()(NX_PP_TYPE_2(n, P, p)) \
        { \
            (*f_)(NX_PP_TYPE_1(n, p)); \
        }
        NX_PP_MULT_MAX(NX_BIND_FR_)
#   undef NX_BIND_FR_
    };

    template <typename F, typename R>
    class fr<F, R, true, false>
    {
        F f_;

    public:
        typedef typename check_result<F, R>::type_t result_t;

        fr(F f) : f_(f) {}

        result_t operator()(void)
        {
            return (*f_)();
        }

#   define NX_BIND_FR_(n) \
        template <NX_PP_TYPE_1(n, typename P)> \
        result_t operator()(NX_PP_TYPE_2(n, P, p)) \
        { \
            return (*f_)(NX_PP_TYPE_1(n, p)); \
        }
        NX_PP_MULT_MAX(NX_BIND_FR_)
#   undef NX_BIND_FR_
    };

    template <typename F>
    class fr<F, void, true, true>
    {
        F f_;

    public:
        typedef void result_t;

        fr(F f) : f_(f) {}

#   define NX_BIND_FR_(n) \
        template <NX_PP_TYPE_1(n, typename P)> \
        result_t operator()(NX_PP_TYPE_2(n, P, p)) \
        { \
            (nx::unref(p1)->*f_)(NX_PP_B1(NX_PP_TYPE_1(n, p))); \
        }
        NX_PP_MULT_MAX(NX_BIND_FR_)
#   undef NX_BIND_FR_
    };

    template <typename F, typename R>
    class fr<F, R, true, true>
    {
        F f_;

    public:
        typedef typename check_result<F, R>::type_t result_t;

        fr(F f) : f_(f) {}

#   define NX_BIND_FR_(n) \
        template <NX_PP_TYPE_1(n, typename P)> \
        result_t operator()(NX_PP_TYPE_2(n, P, p)) \
        { \
            return (nx::unref(p1)->*f_)(NX_PP_B1(NX_PP_TYPE_1(n, p))); \
        }
        NX_PP_MULT_MAX(NX_BIND_FR_)
#   undef NX_BIND_FR_
    };

    template <typename F, typename P>
    class fr_mem : public fr<F>
    {
        P p_;

    public:
        fr_mem(F f, P p) : fr<F>(f), p_(p) {}

        typename fr<F>::result_t operator()(void)
        {
            return fr<F>::operator()(p_);
        }

#   define NX_BIND_FR_(n) \
        template <NX_PP_TYPE_1(n, typename P)> \
        typename fr<F>::result_t operator()(NX_PP_TYPE_2(n, P, p)) \
        { \
            return fr<F>::operator()(p_, NX_PP_TYPE_1(n, p)); \
        }
        NX_PP_MULT(NX_PP_DEC(NX_PP_MAX), NX_BIND_FR_)
#   undef NX_BIND_FR_
    };

    /*
        Storage for saving function parameters
    */

    template <typename T>
    struct storage
    {
        T t_;

        template <typename U>
        storage(const U& u) : t_(u) {}

        template <typename V>
        V& operator[](V& v)
        {
            return v;
        }

        template <int N>
        typename nx::types_at<T, N - 1>::type_t& operator[](arg<N>)
        {
            return t_.template at<N - 1>();
        }
    };

    template <>
    struct storage<nx::null_t>
    {
        storage(void) {}

        template <typename V>
        V& operator[](V& v)
        {
            return v;
        }
    };

    /*
        List for binding parameters
    */

    template <typename T = nx::null_t>
    class list;

    template <>
    class list<nx::null_t> : public storage<nx::null_t>
    {
        typedef storage<nx::null_t> base_t;
    public:
        list(void) : base_t() {}
    public:
        template <typename R, typename F, typename L>
        R operator()(type_wrap<R>, F& f, L& /*l*/)
        {
            return f();
        }
    };

#define NX_BIND_LIST_AT_(n, ...) , l[base_t::t_.template at<n - 1>()]
#define NX_BIND_LIST_(n) \
    template <NX_PP_TYPE_1(n, typename P)> \
    class list<nx::tuple<NX_PP_TYPE_1(n, P)> > : public storage<nx::tuple<NX_PP_TYPE_1(n, P)> > \
    { \
        typedef storage<nx::tuple<NX_PP_TYPE_1(n, P)> > base_t; \
    public: \
        list(const list& l) : base_t(l.t_) {} \
        template <typename U> \
        list(const U& u) : base_t(u) {} \
    public: \
        template <typename R, typename F, typename L> \
        R operator()(type_wrap<R>, F& f, L& l) \
        { \
            return f(NX_PP_B1(NX_PP_REPEAT(n, NX_BIND_LIST_AT_))); \
        } \
    };
    NX_PP_MULT_MAX(NX_BIND_LIST_)
#undef NX_BIND_LIST_
#undef NX_BIND_LIST_AT_

    /*
        Make calls be forwarded to fr and list
    */

    template <typename F, typename L, typename R = nx::null_t>
    class detail
    {
        typedef typename fr<F, R>::result_t result_t;

        fr<F, R> f_;
        L        l_;

    public:
        detail(F f, const L& l) : f_(f), l_(l) {}

        result_t operator()(void)
        {
            list<> l;
            return l_(type_wrap<result_t>(), f_, l);
        }

#   define NX_BIND_DETAIL_(n) \
        template <NX_PP_TYPE_1(n, typename P)> \
        result_t operator()(NX_PP_TYPE_2(n, P, p)) \
        { \
            list<nx::tuple<NX_PP_TYPE_1(n, P)> > l(nx::tie(NX_PP_TYPE_1(n, p))); \
            return l_(type_wrap<result_t>(), f_, l); \
        }
        NX_PP_MULT_MAX(NX_BIND_DETAIL_)
#   undef NX_BIND_DETAIL_
    };
}

/*
    Bind function to a functor

    It will make a simple functor,
    for using the function with the number of parameters by default
*/

template <typename R, typename F>
inline private_bind::fr<F, R> bind(F f)
{
    return private_bind::fr<F, R>(f);
}

template <typename F>
inline private_bind::fr<F> bind(F f)
{
    return private_bind::fr<F>(f);
}

template <typename T, typename C, typename P>
inline private_bind::fr_mem<T C::*, P> bind(T C::* f, P p)
{
    return private_bind::fr_mem<T C::*, P>(f, p);
}

/*
    Bind function and parameters to a functor
*/

#define NX_BIND_(n) \
template <typename F, NX_PP_TYPE_1(n, typename P)> \
inline private_bind::detail<F, private_bind::list<nx::tuple<NX_PP_TYPE_1(n, P)> > > bind(F f, NX_PP_TYPE_2(n, P, p)) \
{ \
    typedef private_bind::list<nx::tuple<NX_PP_TYPE_1(n, P)> > l_t; \
    return private_bind::detail<F, l_t>(f, nx::tie(NX_PP_TYPE_1(n, p))); \
}
NX_PP_MULT_MAX(NX_BIND_)
#undef NX_BIND_

#define NX_BIND_(n) \
template <typename R, typename F, NX_PP_TYPE_1(n, typename P)> \
inline private_bind::detail<F, private_bind::list<nx::tuple<NX_PP_TYPE_1(n, P)> >, R> bind(F f, NX_PP_TYPE_2(n, P, p)) \
{ \
    typedef private_bind::list<nx::tuple<NX_PP_TYPE_1(n, P)> > l_t; \
    return private_bind::detail<F, l_t, R>(f, nx::tie(NX_PP_TYPE_1(n, p))); \
}
NX_PP_MULT_MAX(NX_BIND_)
#undef NX_BIND_

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
