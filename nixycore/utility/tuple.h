/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/utility/rvalue.h"
#include "nixycore/utility/refer.h"

#include "nixycore/bugfix/assert.h"
#include "nixycore/bugfix/noexcept.h"

#include "nixycore/typemanip/typedefs.h"
#include "nixycore/typemanip/typelist.h"
#include "nixycore/typemanip/typebehavior.h"

#include "nixycore/general/general.h"
#include "nixycore/preprocessor/preprocessor.h"
#include "nixycore/algorithm/algorithm.h"

#ifdef NX_SP_CXX11_TUPLE
#include <tuple> // std::tuple
#endif

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

namespace private_tuple
{
#ifdef NX_SP_CXX11_TEMPLATES
    template <typename... TypesT>
    struct types_array {}; // helper for std::get<N>(std::tuple)
#endif/*NX_SP_CXX11_TUPLE*/

    template <typename T = nx::null_t, typename U = nx::null_t>
    struct detail : TypeList<T, U>
    {
        typedef TypeList<T, U> base_t;

        typename base_t::head_t head_;
        typename base_t::tail_t tail_;

        detail(void) {}

#ifdef NX_SP_CXX11_TEMPLATES
#   ifdef NX_SP_CXX11_TUPLE
        template <typename P1, typename... ArgsT, typename T1, typename... TypesT>
        detail(const std::tuple<P1, ArgsT...>& tp, types_array<T1, TypesT...>)
            : head_(std::get<(sizeof...(ArgsT) - sizeof...(TypesT))>(tp))
            , tail_(tp, types_array<TypesT...>())
        {}
#   endif/*NX_SP_CXX11_TUPLE*/

        template <typename P1, typename... ArgsT>
        detail(nx::none_t, nx_fref(P1, par1), nx_fref(ArgsT, ... args))
            : head_(nx_extract(P1, par1))
            , tail_(nx::none, nx_forward(ArgsT, args)...)
        {}
#else /*NX_SP_CXX11_TEMPLATES*/
        template <NX_PP_TYPE_MAX_1(typename P)>
        detail(NX_PP_TYPE_MAX_2(P, NX_PP_FREF(par)))
            : head_(nx_extract(P1, par1))
            , tail_(NX_PP_B1(NX_PP_FORWARD_MAX(P, par)), nx::none)
        {}
#endif/*NX_SP_CXX11_TEMPLATES*/

#ifdef NX_SP_CXX11_RVALUE_REF
        template <typename T_, typename U_>
        detail(detail<T_, U_>&& r)
            : head_(nx::move(r.head_))
            , tail_(nx::move(r.tail_))
        {}
#else /*NX_SP_CXX11_RVALUE_REF*/
        template <typename T_, typename U_>
        detail(const nx::rvalue<detail<T_, U_>, true>& r)
            : head_(r.head_)
            , tail_(nx::move(r.tail_))
        {}
#endif/*NX_SP_CXX11_RVALUE_REF*/

        template <typename T_, typename U_>
        detail(const detail<T_, U_>& r)
            : head_(r.head_)
            , tail_(r.tail_)
        {}

        template <typename T_, typename U_>
        void swap(detail<T_, U_>& r)
        {
            nx::swap(head_, r.head_);
            tail_.swap(r.tail_);
        }
    };

    template <typename T>
    struct detail<T, nx::null_t> : TypeList<T, nx::null_t>
    {
        typedef TypeList<T, nx::null_t> base_t;

        typename base_t::head_t head_;

        detail(void) {}

#ifdef NX_SP_CXX11_TEMPLATES
#   ifdef NX_SP_CXX11_TUPLE
        template <typename P1, typename... ArgsT, typename T1, typename... TypesT>
        detail(const std::tuple<P1, ArgsT...>& tp, types_array<T1, TypesT...>)
            : head_(std::get<(sizeof...(ArgsT) - sizeof...(TypesT))>(tp))
        {}
#   endif/*NX_SP_CXX11_TUPLE*/

        template <typename P1>
        detail(nx::none_t, nx_fref(P1, par1))
            : head_(nx_extract(P1, par1))
        {}
#else /*NX_SP_CXX11_TEMPLATES*/
        template <NX_PP_TYPE_MAX_1(typename P)>
        detail(nx_fref(P1, par1), NX_PP_B1(NX_PP_TYPE_MAX_1(P, NX_PP_FREF())))
            : head_(nx_extract(P1, par1))
        {}
#endif/*NX_SP_CXX11_TEMPLATES*/

        template <typename T_>
        detail(const detail<T_, nx::null_t>& r)
            : head_(r.head_)
        {}

#ifdef NX_SP_CXX11_RVALUE_REF
        template <typename T_>
        detail(detail<T_, nx::null_t>&& r)
            : head_(nx::move(r.head_))
        {}
#else /*NX_SP_CXX11_RVALUE_REF*/
        template <typename T_>
        detail(const nx::rvalue<detail<T_, nx::null_t>, true>& r)
            : head_(r.head_)
        {}
#endif/*NX_SP_CXX11_RVALUE_REF*/

        template <typename T_>
        void swap(detail<T_, nx::null_t>& r)
        {
            nx::swap(head_, r.head_);
        }
    };

    template <>
    struct detail<nx::null_t, nx::null_t>
    {
        typedef nx::null_t base_t;

        detail(void) {}

        void swap(detail& /*r*/) {}
    };
}

/*
    For getting a value from tuple
*/

template <int N>
struct tuple_at
{
    template <class U1, class U2>
    static typename nx::rm_reference<typename nx::types_at<private_tuple::detail<U1, U2>, N>::type_t
    >::type_t& value(private_tuple::detail<U1, U2>& t)
    {
        return tuple_at<N - 1>::value(t.tail_);
    }
};

template <>
struct tuple_at<0>
{
    template <class U1, class U2>
    static typename nx::rm_reference<typename nx::types_at<private_tuple::detail<U1, U2>, 0>::type_t
    >::type_t& value(private_tuple::detail<U1, U2>& t)
    {
        return t.head_;
    }
};

/*
    tuple for values
*/

#ifdef NX_SP_CXX11_TEMPLATES
template <typename... TypesT>
class tuple;

template <>
class tuple<> : public private_tuple::detail<>
{
public: /* public for TypeList operations */
    typedef private_tuple::detail<> base_t;

public:
    tuple(void) {}
#ifdef NX_SP_CXX11_TUPLE
    tuple(const std::tuple<>& /*tp*/) {}
#endif/*NX_SP_CXX11_TUPLE*/

    template<typename... U>
    tuple(const tuple<U...>& /*r*/) {}

    template<typename... U>
    tuple& operator=(tuple<U...> rhs)
    {
        rhs.swap(*this);
        return (*this);
    }

    template<typename... U>
    void swap(tuple<U...>& rhs)
    {
        base_t::swap(rhs);
    } 

    size_t length(void) const
    {
        return 0;
    }
};

template <typename T1, typename... TypesT>
class tuple<T1, TypesT...> : public types<private_tuple::detail<T1>, TypesT...>::type_t
{
public: /* public for TypeList operations */
    typedef typename types<private_tuple::detail<T1>, TypesT...>::type_t base_t;
#else /*NX_SP_CXX11_TEMPLATES*/
template <NX_PP_TYPE_MAX_1(typename T, = nx::null_t)>
class tuple : public types<private_tuple::detail<T1>, NX_PP_B1(NX_PP_TYPE_MAX_1(T))>::type_t
{
public: /* public for TypeList operations */
    typedef typename types<private_tuple::detail<T1>, NX_PP_B1(NX_PP_TYPE_MAX_1(T))>::type_t base_t;
#endif/*NX_SP_CXX11_TEMPLATES*/

public:
    tuple(void) {}

    template <class U1, class U2>
    tuple(const std::pair<U1, U2>& r)
        : base_t(r.first, r.second)
    {}

#ifdef NX_SP_CXX11_TEMPLATES
#ifdef NX_SP_CXX11_TUPLE
    template <typename... P>
    tuple(const std::tuple<P...>& tp)
        : base_t(tp, private_tuple::types_array<P...>())
    {}
#endif/*NX_SP_CXX11_TUPLE*/

    tuple(typename traits<T1>::param_t par1, typename traits<TypesT>::param_t... args)
        : base_t(nx::none, nx_fval(par1), nx_fval(args)...)
    {}

    template<typename... U>
    tuple(const tuple<U...>& r)
        : base_t(r)
    {}

    template<typename... U>
    tuple(nx_rref(tuple<U...>, true) r)
#ifdef NX_SP_CXX11_RVALUE_REF
        : base_t(nx::move(r))
#else /*NX_SP_CXX11_RVALUE_REF*/
        : base_t(nx::move(static_cast<typename tuple<U...>::base_t>(nx::moved(r))))
#endif/*NX_SP_CXX11_RVALUE_REF*/
    {}

    template<typename... U>
    tuple& operator=(tuple<U...> rhs)
    {
        rhs.swap(*this);
        return (*this);
    }

    template<typename... U>
    void swap(tuple<U...>& rhs)
    {
        base_t::swap(rhs);
    }
#else /*NX_SP_CXX11_TEMPLATES*/
#ifdef NX_SP_CXX11_TUPLE

#define NX_TUPLE_PAR_1_(n, par) std::get<n>(par)
#define NX_TUPLE_PAR_2_(n, par) , NX_TUPLE_PAR_1_(n, par)

#define NX_TUPLE_(n) \
    template <NX_PP_TYPE_1(n, typename P)> \
    tuple(const std::tuple<NX_PP_TYPE_1(n, P)>& tp) \
        : base_t(NX_PP_REPEATEX(n, NX_TUPLE_PAR_1_, NX_TUPLE_PAR_2_, tp), NX_PP_CLONE(NX_PP_REM(n), nx::none, , )) \
    {}
    NX_PP_MULT(NX_PP_DEC(NX_PP_MAX), NX_TUPLE_)
#undef NX_TUPLE_

    template <NX_PP_TYPE_MAX_1(typename P)>
    tuple(const std::tuple<NX_PP_TYPE_MAX_1(P)>& tp)
        : base_t(NX_PP_REPEATEX_MAX(NX_TUPLE_PAR_1_, NX_TUPLE_PAR_2_, tp))
    {}

#undef NX_TUPLE_PAR_1_
#undef NX_TUPLE_PAR_2_

#endif/*NX_SP_CXX11_TUPLE*/

#define NX_TUPLE_PAR_1_(n, par) nx_fval(NX_PP_JOIN(par, n))
#define NX_TUPLE_PAR_2_(n, par) , NX_TUPLE_PAR_1_(n, par)

#define NX_TUPLE_(n) \
    tuple(NX_PP_TYPE_2(n, typename traits<T, >::param_t par)) \
        : base_t(NX_PP_REPEATEX(n, NX_TUPLE_PAR_1_, NX_TUPLE_PAR_2_, par), NX_PP_CLONE(NX_PP_REM(n), nx::none, ,)) \
    {}
    NX_PP_MULT(NX_PP_DEC(NX_PP_MAX), NX_TUPLE_)
#undef NX_TUPLE_

    tuple(NX_PP_TYPE_MAX_2(typename traits<T, >::param_t par))
        : base_t(NX_PP_REPEATEX_MAX(NX_TUPLE_PAR_1_, NX_TUPLE_PAR_2_, par))
    {}

#undef NX_TUPLE_PAR_1_
#undef NX_TUPLE_PAR_2_

    template<NX_PP_TYPE_MAX_1(typename U)>
    tuple(const tuple<NX_PP_TYPE_MAX_1(U)>& r)
        : base_t(r)
    {}

    template<NX_PP_TYPE_MAX_1(typename U)>
#ifdef NX_SP_CXX11_RVALUE_REF
    tuple(tuple<NX_PP_TYPE_MAX_1(U)>&& r)
        : base_t(nx::move(r))
#else /*NX_SP_CXX11_RVALUE_REF*/
    tuple(const nx::rvalue<tuple<NX_PP_TYPE_MAX_1(U)>, true>& r)
        : base_t(nx::move(static_cast<typename tuple<NX_PP_TYPE_MAX_1(U)>::base_t>(nx::moved(r))))
#endif/*NX_SP_CXX11_RVALUE_REF*/
    {}

    template <NX_PP_TYPE_MAX_1(typename U)>
    tuple& operator=(tuple<NX_PP_TYPE_MAX_1(U)> rhs)
    {
        rhs.swap(*this);
        return (*this);
    }

    template <NX_PP_TYPE_MAX_1(typename U)>
    void swap(tuple<NX_PP_TYPE_MAX_1(U)>& rhs)
    {
        base_t::swap(rhs);
    }
#endif/*NX_SP_CXX11_TEMPLATES*/

    size_t length(void) const
    {
        return nx::types_len<tuple>::value;
    }

public:
    template <int N>
    typename rm_reference<typename types_at<tuple, N>::type_t>::type_t& at(void)
    {
        return tuple_at<N>::value(*this);
    }
    template <int N>
    typename rm_reference<typename types_at<tuple, N>::type_t>::type_t& at(type_int<N>)
    {
        return at<N>();
    }
    template <typename R>
    typename rm_reference<R>::type_t& at(void)
    {
        return tuple_at<types_find<tuple, R>::value>::value(*this);
    }
    template <typename R>
    typename rm_reference<R>::type_t& at(type_wrap<R>)
    {
        return at<R>();
    }

    typename rm_reference<typename types_at<tuple, 0>::type_t>::type_t& operator*(void)
    {
        return at<0>();
    }
};

/*
    Special swap algorithm
*/

#ifdef NX_SP_CXX11_TEMPLATES
template <typename... T, typename... U>
inline void swap(tuple<T...>& x, tuple<U...>& y)
{
    x.swap(y);
}
#else /*NX_SP_CXX11_TEMPLATES*/
template <NX_PP_TYPE_MAX_1(typename T), NX_PP_TYPE_MAX_1(typename U)>
inline void swap(tuple<NX_PP_TYPE_MAX_1(T)>& x, tuple<NX_PP_TYPE_MAX_1(U)>& y)
{
    x.swap(y);
}
#endif/*NX_SP_CXX11_TEMPLATES*/

/*
    Special TypeList algorithms
*/

NX_SPECIAL_TYPES(tuple)

/*
    Tie variables to be a tuple
*/

#ifdef NX_SP_CXX11_TEMPLATES
template <typename... T>
inline tuple<T&...> tie(T&... args) nx_noexcept
{
    return tuple<T&...>(args...);
}
#else /*NX_SP_CXX11_TEMPLATES*/
#define NX_TIE_(n) \
template <NX_PP_TYPE_1(n, typename T)> \
inline tuple<NX_PP_TYPE_1(n, T, &)> tie(NX_PP_TYPE_2(n, T, & par)) nx_noexcept \
{ \
    return tuple<NX_PP_TYPE_1(n, T, &)>(NX_PP_TYPE_1(n, par)); \
}
NX_PP_MULT_MAX(NX_TIE_)
#undef NX_TIE_
#endif/*NX_SP_CXX11_TEMPLATES*/

/*
    Make a tuple
*/

#ifdef NX_SP_CXX11_TEMPLATES
template <typename... T>
inline tuple<typename unrefwrap<T>::type_t...> make_tuple(nx_fref(T, ... args))
{
    return tuple<typename unrefwrap<T>::type_t...>(nx_forward(T, args)...);
}
#else /*NX_SP_CXX11_TEMPLATES*/
#define NX_TIE_(n) \
template <NX_PP_TYPE_1(n, typename P)> \
inline tuple<NX_PP_TYPE_1(n, typename unrefwrap<P, >::type_t)> \
    make_tuple(NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
{ \
    return tuple<NX_PP_TYPE_1(n, typename unrefwrap<P, >::type_t)> \
        (NX_PP_FORWARD(n, P, par)); \
}
NX_PP_MULT_MAX(NX_TIE_)
#undef NX_TIE_
#endif/*NX_SP_CXX11_TEMPLATES*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
