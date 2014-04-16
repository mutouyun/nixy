/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/delegate/function_traits.h"

#include "nixycore/general/general.h"
#include "nixycore/preprocessor/preprocessor.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/utility/utility.h"

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

    template <typename F, typename R = nx::null_t>
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
                        , bool = nx::is_pointer        <typename nx::rm_reference<F>::type_t>::value
                        , bool = nx::is_member_function<typename nx::rm_reference<F>::type_t>::value>
    class fr;

    template <typename F>
    class fr<F, void, false, false> // for class type functor
    {
        F f_;

    public:
        typedef void result_t;

        fr(const typename nx::rm_const<F>::type_t& f) : f_(f) {}

#ifdef NX_SP_CXX11_TEMPLATES
        template <typename... P>
        result_t operator()(nx_fref(P, ... par))
        {
            /*return*/ nx_extract(F, f_)(nx_forward(P, par)...);
        }
#else /*NX_SP_CXX11_TEMPLATES*/
        result_t operator()(void)
        {
            /*return*/ nx_extract(F, f_)();
        }

#   define NX_BIND_FR_(n) \
        template <NX_PP_TYPE_1(n, typename P)> \
        result_t operator()(NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
        { \
            /*return*/ nx_extract(F, f_)(NX_PP_FORWARD(n, P, par)); \
        }
        NX_PP_MULT_MAX(NX_BIND_FR_)
#   undef NX_BIND_FR_
#endif/*NX_SP_CXX11_TEMPLATES*/
    };

    template <typename F, typename R>
    class fr<F, R, false, false> // for class type functor
    {
        F f_;

    public:
        typedef typename check_result<F, R>::type_t result_t;

        fr(const typename nx::rm_const<F>::type_t& f) : f_(f) {}

#ifdef NX_SP_CXX11_TEMPLATES
        template <typename... P>
        result_t operator()(nx_fref(P, ... par))
        {
            return nx_extract(F, f_)(nx_forward(P, par)...);
        }
#else /*NX_SP_CXX11_TEMPLATES*/
        result_t operator()(void)
        {
            return nx_extract(F, f_)();
        }

#   define NX_BIND_FR_(n) \
        template <NX_PP_TYPE_1(n, typename P)> \
        result_t operator()(NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
        { \
            return nx_extract(F, f_)(NX_PP_FORWARD(n, P, par)); \
        }
        NX_PP_MULT_MAX(NX_BIND_FR_)
#   undef NX_BIND_FR_
#endif/*NX_SP_CXX11_TEMPLATES*/
    };

    template <typename F>
    class fr<F, void, true, false>
    {
        F f_;

    public:
        typedef void result_t;

        fr(F f) : f_(f) {}

#ifdef NX_SP_CXX11_TEMPLATES
        template <typename... P>
        result_t operator()(nx_fref(P, ... par))
        {
            /*return*/ (*f_)(nx_forward(P, par)...);
        }
#else /*NX_SP_CXX11_TEMPLATES*/
        result_t operator()(void)
        {
            /*return*/ (*f_)();
        }

#   define NX_BIND_FR_(n) \
        template <NX_PP_TYPE_1(n, typename P)> \
        result_t operator()(NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
        { \
            /*return*/ (*f_)(NX_PP_FORWARD(n, P, par)); \
        }
        NX_PP_MULT_MAX(NX_BIND_FR_)
#   undef NX_BIND_FR_
#endif/*NX_SP_CXX11_TEMPLATES*/
    };

    template <typename F, typename R>
    class fr<F, R, true, false>
    {
        F f_;

    public:
        typedef typename check_result<F, R>::type_t result_t;

        fr(F f) : f_(f) {}

#ifdef NX_SP_CXX11_TEMPLATES
        template <typename... P>
        result_t operator()(nx_fref(P, ... par))
        {
            return (*f_)(nx_forward(P, par)...);
        }
#else /*NX_SP_CXX11_TEMPLATES*/
        result_t operator()(void)
        {
            return (*f_)();
        }

#   define NX_BIND_FR_(n) \
        template <NX_PP_TYPE_1(n, typename P)> \
        result_t operator()(NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
        { \
            return (*f_)(NX_PP_FORWARD(n, P, par)); \
        }
        NX_PP_MULT_MAX(NX_BIND_FR_)
#   undef NX_BIND_FR_
#endif/*NX_SP_CXX11_TEMPLATES*/
    };

    template <typename F>
    class fr<F, void, true, true>
    {
        F f_;

    public:
        typedef void result_t;

        fr(F f) : f_(f) {}

#ifdef NX_SP_CXX11_TEMPLATES
        template <typename P1, typename... P>
        result_t operator()(nx_fref(P1, par1), nx_fref(P, ... par))
        {
            /*return*/ (nx_extract(P1, par1)->*f_)(nx_forward(P, par)...);
        }
#else /*NX_SP_CXX11_TEMPLATES*/
#   define NX_BIND_FR_(n) \
        template <NX_PP_TYPE_1(n, typename P)> \
        result_t operator()(NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
        { \
            /*return*/ (nx_extract(P1, par1)->*f_)(NX_PP_B1(NX_PP_FORWARD(n, P, par))); \
        }
        NX_PP_MULT_MAX(NX_BIND_FR_)
#   undef NX_BIND_FR_
#endif/*NX_SP_CXX11_TEMPLATES*/
    };

    template <typename F, typename R>
    class fr<F, R, true, true>
    {
        F f_;

    public:
        typedef typename check_result<F, R>::type_t result_t;

        fr(F f) : f_(f) {}

#ifdef NX_SP_CXX11_TEMPLATES
        template <typename P1, typename... P>
        result_t operator()(nx_fref(P1, par1), nx_fref(P, ... par))
        {
            return (nx_extract(P1, par1)->*f_)(nx_forward(P, par)...);
        }
#else /*NX_SP_CXX11_TEMPLATES*/
#   define NX_BIND_FR_(n) \
        template <NX_PP_TYPE_1(n, typename P)> \
        result_t operator()(NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
        { \
            return (nx_extract(P1, par1)->*f_)(NX_PP_B1(NX_PP_FORWARD(n, P, par))); \
        }
        NX_PP_MULT_MAX(NX_BIND_FR_)
#   undef NX_BIND_FR_
#endif/*NX_SP_CXX11_TEMPLATES*/
    };

    /*
        Storage for saving function parameters
    */

    template <typename T>
    struct storage
    {
        mutable T t_;

        storage(const T& t) : t_(t) {}

#ifdef NX_SP_CXX11_TEMPLATES
        template <typename... P>
        storage(nx_fref(P, ... par))
            : t_(nx_forward(P, par)...)
        {}
#else /*NX_SP_CXX11_TEMPLATES*/
#define NX_BIND_STORAGE_(n) \
        template <NX_PP_TYPE_1(n, typename P)> \
        storage(NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
            : t_(NX_PP_FORWARD(n, P, par)) \
        {}
        NX_PP_MULT_MAX(NX_BIND_STORAGE_)
#undef NX_BIND_STORAGE_
#endif/*NX_SP_CXX11_TEMPLATES*/

        template <typename V>
        V& operator[](V& v) const
        {
            return v;
        }

        template <int N>
        typename nx::types_at<T, N - 1>::type_t& operator[](arg<N>) const
        {
            return t_.template at<N - 1>();
        }
    };

    template <>
    struct storage<nx::tuple<> >
    {
        storage(void) {}

        template <typename V>
        V& operator[](V& v) const
        {
            return v;
        }
    };

    /*
        List for binding parameters
    */
#ifdef NX_SP_CXX11_TEMPLATES
    template <size_t... N>
    struct indexes
    {
        typedef indexes<N..., sizeof...(N)> next_indexes;
    };

    template <typename... P>
    struct indexes_maker;

    template <typename P1, typename... P>
    struct indexes_maker<P1, P...>
    {
        typedef typename indexes_maker<P...>::type_t::next_indexes type_t;
    };

    template <>
    struct indexes_maker<>
    {
        typedef indexes<> type_t;
    };
#endif/*NX_SP_CXX11_TEMPLATES*/

    template <typename T = nx::tuple<> >
    class list;

    template <>
    class list<nx::tuple<> > : public storage<nx::tuple<> >
    {
        typedef storage<nx::tuple<> > base_t;

    public:
        list(const list&) : base_t() {}
#ifdef NX_SP_CXX11_TEMPLATES
        list(nx::none_t) : base_t() {}
#else /*NX_SP_CXX11_TEMPLATES*/
        list(void) : base_t() {}
#endif/*NX_SP_CXX11_TEMPLATES*/

    public:
#ifdef NX_SP_CXX11_TEMPLATES
        typedef indexes_maker<>::type_t indexes_t;
        template <typename R, typename F, typename L, size_t... N>
        R operator()(type_wrap<R>, F& f, L& /*l*/, indexes<N...>) const
#else /*NX_SP_CXX11_TEMPLATES*/
        template <typename R, typename F, typename L>
        R operator()(type_wrap<R>, F& f, L& /*l*/) const
#endif/*NX_SP_CXX11_TEMPLATES*/
        {
            return f();
        }
    };

#ifdef NX_SP_CXX11_TEMPLATES
    template <typename... P>
    class list<nx::tuple<P...> > : public storage<nx::tuple<P...> >
    {
        typedef storage<nx::tuple<P...> > base_t;

    public:
        list(const list& l) : base_t(l.t_) {}

        template <typename... P_>
        list(nx::none_t, nx_fref(P_, ... par))
            : base_t(nx_forward(P_, par)...)
        {}

    public:
        typedef typename indexes_maker<P...>::type_t indexes_t;
        template <typename R, typename F, typename L, size_t... N>
        R operator()(type_wrap<R>, F& f, L& l, indexes<N...>) const
        {
            return f(l[base_t::t_.template at<N>()]...);
        }
    };
#else /*NX_SP_CXX11_TEMPLATES*/
#define NX_BIND_LIST_AT_(n, ...) , l[base_t::t_.template at<n - 1>()]
#define NX_BIND_LIST_(n) \
    template <NX_PP_TYPE_1(n, typename P)> \
    class list<nx::tuple<NX_PP_TYPE_1(n, P)> > : public storage<nx::tuple<NX_PP_TYPE_1(n, P)> > \
    { \
        typedef storage<nx::tuple<NX_PP_TYPE_1(n, P)> > base_t; \
    public: \
        list(const list& l) : base_t(l.t_) {} \
        list(NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
            : base_t(NX_PP_FORWARD(n, P, par)) \
        {} \
    public: \
        template <typename R, typename F, typename L> \
        R operator()(type_wrap<R>, F& f, L& l) const \
        { \
            return f(NX_PP_B1(NX_PP_REPEAT(n, NX_BIND_LIST_AT_))); \
        } \
    };
    NX_PP_MULT_MAX(NX_BIND_LIST_)
#undef NX_BIND_LIST_
#undef NX_BIND_LIST_AT_
#endif/*NX_SP_CXX11_TEMPLATES*/

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
        detail(const F& f, const L& l)
            : f_(f), l_(l)
        {}

#ifdef NX_SP_CXX11_TEMPLATES
        template <typename... P>
        result_t operator()(nx_fref(P, ... par))
        {
            list<nx::tuple<P...> > l(nx::none, nx_forward(P, par)...);
            return l_(type_wrap<result_t>(), f_, l, typename L::indexes_t());
        }
#else /*NX_SP_CXX11_TEMPLATES*/
        result_t operator()(void)
        {
            list<> l;
            return l_(type_wrap<result_t>(), f_, l);
        }

#   define NX_BIND_DETAIL_(n) \
        template <NX_PP_TYPE_1(n, typename P)> \
        result_t operator()(NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
        { \
            list<nx::tuple<NX_PP_TYPE_1(n, P)> > l(NX_PP_FORWARD(n, P, par)); \
            return l_(type_wrap<result_t>(), f_, l); \
        }
        NX_PP_MULT_MAX(NX_BIND_DETAIL_)
#   undef NX_BIND_DETAIL_
#endif/*NX_SP_CXX11_TEMPLATES*/
    };
}

/*
    Bind function to a functor

    It will make a simple functor,
    for using the function with the number of parameters by default
*/

template <typename R, typename F>
inline private_bind::fr<F, R> bind(nx_fref(F, f))
{
    return private_bind::fr<F, R>(nx_forward(F, f));
}

template <typename F>
inline private_bind::fr<F> bind(nx_fref(F, f))
{
    return private_bind::fr<F>(nx_forward(F, f));
}

/*
    Bind function and parameters to a functor
*/

#ifdef NX_SP_CXX11_TEMPLATES
template <typename F, typename P1, typename... P>
inline private_bind::detail<F, private_bind::list<nx::tuple<P1, P...> > >
    bind(nx_fref(F, f), nx_fref(P1, par1), nx_fref(P, ... par))
{
    typedef private_bind::list<nx::tuple<P1, P...> > l_t;
    return private_bind::detail<F, l_t>(nx_forward(F, f), l_t(nx::none, nx_forward(P1, par1), nx_forward(P, par)...));
}
template <typename R, typename F, typename P1, typename... P>
inline private_bind::detail<F, private_bind::list<nx::tuple<P1, P...> >, R>
    bind(nx_fref(F, f), nx_fref(P1, par1), nx_fref(P, ... par))
{
    typedef private_bind::list<nx::tuple<P1, P...> > l_t;
    return private_bind::detail<F, l_t, R>(nx_forward(F, f), l_t(nx::none, nx_forward(P1, par1), nx_forward(P, par)...));
}
#else /*NX_SP_CXX11_TEMPLATES*/
#define NX_BIND_(n) \
template <typename F, NX_PP_TYPE_1(n, typename P)> \
inline private_bind::detail<F, private_bind::list<nx::tuple<NX_PP_TYPE_1(n, P)> > > \
    bind(nx_fref(F, f), NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
{ \
    typedef private_bind::list<nx::tuple<NX_PP_TYPE_1(n, P)> > l_t; \
    return private_bind::detail<F, l_t>(nx_forward(F, f), l_t(NX_PP_FORWARD(n, P, par))); \
} \
template <typename R, typename F, NX_PP_TYPE_1(n, typename P)> \
inline private_bind::detail<F, private_bind::list<nx::tuple<NX_PP_TYPE_1(n, P)> >, R> \
    bind(nx_fref(F, f), NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
{ \
    typedef private_bind::list<nx::tuple<NX_PP_TYPE_1(n, P)> > l_t; \
    return private_bind::detail<F, l_t, R>(nx_forward(F, f), l_t(NX_PP_FORWARD(n, P, par))); \
}
NX_PP_MULT_MAX(NX_BIND_)
#undef NX_BIND_
#endif/*NX_SP_CXX11_TEMPLATES*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
