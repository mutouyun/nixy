/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
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

    template <typename F, typename R = nx::null_t, bool = nx::is_pointer        <F>::value ||
                                                          nx::is_function       <F>::value
                                                 , bool = nx::is_member_function<F>::value>
    class fr;

    // for class type

    template <typename F, bool = nx::is_copyable<F>::value>
    class fr_class_base
    {
    protected:
        F f_;

    public:
        fr_class_base(const F& f)               : f_(f)      {}
        fr_class_base(const fr_class_base& rhs) : f_(rhs.f_) {}
        fr_class_base(nx_rref(F) f)
            : f_(nx::move(f))
        {}
        fr_class_base(nx_rref(fr_class_base, true) rhs)
            : f_(nx::move(rhs.f_))
        {}
    };

    template <typename F>
    class fr_class_base<F, false>
    {
    protected:
        F f_;

    public:
        fr_class_base(const F& f)
            : f_(nx::move(f))
        {}
        fr_class_base(const fr_class_base& rhs)
            : f_(nx::move(const_cast<fr_class_base&>(rhs).f_))
        {}
        fr_class_base(nx_rref(F) f)
            : f_(nx::move(f))
        {}
        fr_class_base(nx_rref(fr_class_base, true) rhs)
            : f_(nx::move(rhs.f_))
        {}
    };

    template <typename F>
    class fr<F, void, false, false> : public fr_class_base<F>
    {
        typedef fr_class_base<F> base_t;
        using base_t::f_;

    public:
        typedef void result_t;

        fr(const F& f)    : base_t(f)            {}
        fr(const fr& rhs) : base_t(rhs)          {}
        fr(nx_rref(F) f)  : base_t(nx::move(f))  {}
        fr(nx_rref(fr, true) rhs)
            : base_t(nx::move_cast<base_t>(rhs))
        {}

#ifdef NX_SP_CXX11_TEMPLATES
        template <typename... P>
        result_t operator()(nx_fref(P)... par)
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
    class fr<F, R, false, false> : public fr_class_base<F>
    {
        typedef fr_class_base<F> base_t;
        using base_t::f_;

    public:
        typedef typename check_result<F, R>::type_t result_t;

        fr(const F& f)    : base_t(f)            {}
        fr(const fr& rhs) : base_t(rhs)          {}
        fr(nx_rref(F) f)  : base_t(nx::move(f))  {}
        fr(nx_rref(fr, true) rhs)
            : base_t(nx::move_cast<base_t>(rhs))
        {}

#ifdef NX_SP_CXX11_TEMPLATES
        template <typename... P>
        result_t operator()(nx_fref(P)... par)
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

    // for pointer or function type

    template <typename F>
    class fr<F, void, true, false>
    {
        F f_;

    public:
        typedef void result_t;

        fr(F f) : f_(f) {}

#ifdef NX_SP_CXX11_TEMPLATES
        template <typename... P>
        result_t operator()(nx_fref(P)... par)
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
        result_t operator()(nx_fref(P)... par)
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

    // for member function pointer type

    template <typename F>
    class fr<F, void, true, true>
    {
        F f_;

    public:
        typedef void result_t;

        fr(F f) : f_(f) {}

#ifdef NX_SP_CXX11_TEMPLATES
        template <typename P1, typename... P>
        result_t operator()(nx_fref(P1) par1, nx_fref(P)... par)
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
        result_t operator()(nx_fref(P1) par1, nx_fref(P)... par)
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

        storage(const T& t)         : t_(t) {}
        storage(nx_rref(T, true) t) : t_(nx::move(t)) {}
        storage(const storage& rhs) : t_(rhs.t_) {}
        storage(nx_rref(storage, true) rhs) : t_(nx::move(rhs.t_)) {}

#ifdef NX_SP_CXX11_TEMPLATES
        template <typename... P>
        storage(nx_fref(P)... par)
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

    template <typename T>
    class list;

    /* void(P...) is portable to older compilers, just like P... */

    template <>
    class list<void()> : public storage<nx::tuple<> >
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
    class list<void(P...)> : public storage<nx::tuple<typename nx::decay<P>::type_t...> >
    {
        typedef storage<nx::tuple<typename nx::decay<P>::type_t...> > base_t;

    public:
        list(const list& l) : base_t(l.t_) {}
        list(nx_rref(list, true) l) : base_t(nx::move(l.t_)) {}

        template <typename... P_>
        list(nx::none_t, nx_fref(P_)... par)
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
    class list<void(NX_PP_TYPE_1(n, P))> : \
        public storage<nx::tuple<NX_PP_TYPE_1(n, typename nx::decay<P, >::type_t)> > \
    { \
        typedef storage<nx::tuple<NX_PP_TYPE_1(n, typename nx::decay<P, >::type_t)> > base_t; \
    public: \
        list(const list& l) : base_t(l.t_) {} \
        list(nx_rref(list, true) l) : base_t(nx::move(l.t_)) {} \
        template <NX_PP_TYPE_1(n, typename P_)> \
        list(NX_PP_TYPE_2(n, P_, NX_PP_FREF(par))) \
            : base_t(NX_PP_FORWARD(n, P_, par)) \
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
        typedef typename nx::decay<F>::type_t func_t;
        typedef typename fr<func_t, R>::result_t result_t;

        fr<func_t, R> f_;
        L             l_;

    public:
        detail(F f, nx_rref(L, true) l)
            : f_(nx_forward(F, f))
            , l_(nx::move(l))
        {}

        detail(const detail& rhs)
            : f_(rhs.f_)
            , l_(rhs.l_)
        {}

        detail(nx_rref(detail, true) rhs)
            : f_(nx::move(rhs.f_))
            , l_(nx::move(rhs.l_))
        {}

#ifdef NX_SP_CXX11_TEMPLATES
        template <typename... P>
        result_t operator()(nx_fref(P)... par)
        {
            list<void(P...)> l(nx::none, nx_forward(P, par)...);
            return l_(type_wrap<result_t>(), f_, l, typename L::indexes_t());
        }
#else /*NX_SP_CXX11_TEMPLATES*/
        result_t operator()(void)
        {
            list<void()> l;
            return l_(type_wrap<result_t>(), f_, l);
        }

#   define NX_BIND_DETAIL_(n) \
        template <NX_PP_TYPE_1(n, typename P)> \
        result_t operator()(NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
        { \
            list<void(NX_PP_TYPE_1(n, P))> l(NX_PP_FORWARD(n, P, par)); \
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

template <typename F>
inline nx_rval(private_bind::fr<typename nx::decay<F>::type_t>, true) bind(nx_fref(F) f)
{
    return private_bind::fr<typename nx::decay<F>::type_t>(nx_forward(F, f));
}

#ifdef NX_SP_CXX11_RVALUE_REF
template <typename R, typename F>
inline private_bind::fr<typename nx::decay<F>::type_t, R> bind(nx_fref(F) f)
{
    return private_bind::fr<typename nx::decay<F>::type_t, R>(nx_forward(F, f));
}
#else /*NX_SP_CXX11_RVALUE_REF*/
template <typename R, typename F>
inline nx::rvalue<private_bind::fr<typename nx::decay<F>::type_t, R>, true> bind(nx_fref(F) f)
{
    return private_bind::fr<typename nx::decay<F>::type_t, R>(nx_forward(F, f));
}
#endif/*NX_SP_CXX11_RVALUE_REF*/

/*
    Bind function and parameters to a functor
*/

#ifdef NX_SP_CXX11_TEMPLATES
#ifdef NX_SP_CXX11_RVALUE_REF
template <typename F, typename... P>
inline private_bind::detail<F, private_bind::list<void(P...)> >
    bind(nx_fref(F) f, nx_fref(P)... par)
{
    typedef private_bind::list<void(P...)> l_t;
    return private_bind::detail<F, l_t>(nx_forward(F, f), l_t(nx::none, nx_forward(P, par)...));
}
template <typename R, typename F, typename... P>
inline private_bind::detail<F, private_bind::list<void(P...)>, R>
    bind(nx_fref(F) f, nx_fref(P)... par)
{
    typedef private_bind::list<void(P...)> l_t;
    return private_bind::detail<F, l_t, R>(nx_forward(F, f), l_t(nx::none, nx_forward(P, par)...));
}
#else /*NX_SP_CXX11_RVALUE_REF*/
template <typename F, typename... P>
inline nx::rvalue<private_bind::detail<F, private_bind::list<void(P...)> >, true>
    bind(nx_fref(F) f, nx_fref(P)... par)
{
    typedef private_bind::list<void(P...)> l_t;
    return private_bind::detail<F, l_t>(nx_forward(F, f), l_t(nx::none, nx_forward(P, par)...));
}
template <typename R, typename F, typename... P>
inline nx::rvalue<private_bind::detail<F, private_bind::list<void(P...)>, R>, true>
    bind(nx_fref(F) f, nx_fref(P)... par)
{
    typedef private_bind::list<void(P...)> l_t;
    return private_bind::detail<F, l_t, R>(nx_forward(F, f), l_t(nx::none, nx_forward(P, par)...));
}
#endif/*NX_SP_CXX11_RVALUE_REF*/
#else /*NX_SP_CXX11_TEMPLATES*/
#ifdef NX_SP_CXX11_RVALUE_REF
#define NX_BIND_DEFINE_(n) \
template <typename F, NX_PP_TYPE_1(n, typename P)> \
inline private_bind::detail<F, private_bind::list<void(NX_PP_TYPE_1(n, P))> > \
    bind(nx_fref(F) f, NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
{ \
    typedef private_bind::list<void(NX_PP_TYPE_1(n, P))> l_t; \
    return private_bind::detail<F, l_t>(nx_forward(F, f), l_t(NX_PP_FORWARD(n, P, par))); \
} \
template <typename R, typename F, NX_PP_TYPE_1(n, typename P)> \
inline private_bind::detail<F, private_bind::list<void(NX_PP_TYPE_1(n, P))>, R> \
    bind(nx_fref(F) f, NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
{ \
    typedef private_bind::list<void(NX_PP_TYPE_1(n, P))> l_t; \
    return private_bind::detail<F, l_t, R>(nx_forward(F, f), l_t(NX_PP_FORWARD(n, P, par))); \
}
#else /*NX_SP_CXX11_RVALUE_REF*/
#define NX_BIND_DEFINE_(n) \
template <typename F, NX_PP_TYPE_1(n, typename P)> \
inline nx::rvalue<private_bind::detail<F, private_bind::list<void(NX_PP_TYPE_1(n, P))> >, true> \
    bind(nx_fref(F) f, NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
{ \
    typedef private_bind::list<void(NX_PP_TYPE_1(n, P))> l_t; \
    return private_bind::detail<F, l_t>(nx_forward(F, f), l_t(NX_PP_FORWARD(n, P, par))); \
} \
template <typename R, typename F, NX_PP_TYPE_1(n, typename P)> \
inline nx::rvalue<private_bind::detail<F, private_bind::list<void(NX_PP_TYPE_1(n, P))>, R>, true> \
    bind(nx_fref(F) f, NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
{ \
    typedef private_bind::list<void(NX_PP_TYPE_1(n, P))> l_t; \
    return private_bind::detail<F, l_t, R>(nx_forward(F, f), l_t(NX_PP_FORWARD(n, P, par))); \
}
#endif/*NX_SP_CXX11_RVALUE_REF*/
NX_PP_MULT_MAX(NX_BIND_DEFINE_)
#undef NX_BIND_DEFINE_
#endif/*NX_SP_CXX11_TEMPLATES*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
