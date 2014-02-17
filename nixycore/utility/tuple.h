/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/bugfix/assert.h"

#include "nixycore/typemanip/typedefs.h"
#include "nixycore/typemanip/typelist.h"
#include "nixycore/typemanip/typebehavior.h"

#include "nixycore/general/general.h"
#include "nixycore/preprocessor/preprocessor.h"
#include "nixycore/algorithm/algorithm.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

namespace private_tuple
{
    template <typename T, typename U = nx::null_t>
    struct detail : TypeList<T, U>
    {
        typedef TypeList<T, U> base_t;

        typename base_t::head_t head_;
        typename base_t::tail_t tail_;

        detail(void) {}

        template <NX_PP_TYPE_MAX_1(typename P)>
        detail(NX_PP_TYPE_MAX_2(P, & par))
            : head_(par1)
            , tail_(NX_PP_B1(NX_PP_TYPE_MAX_1(par)), nx::none)
        {}

        template <typename T_, typename U_>
        detail(const detail<T_, U_>& r)
            : head_(r.head_)
            , tail_(r.tail_)
        {}

        template <typename T_, typename U_>
        detail& operator=(const detail<T_, U_>& r)
        {
            head_ = r.head_;
            tail_ = r.tail_;
            return (*this);
        }

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

        template <NX_PP_TYPE_MAX_1(typename P)>
        detail(P1& par1, NX_PP_B1(NX_PP_TYPE_MAX_1(P, &)))
            : head_(par1)
        {}

        template <typename T_>
        detail(const detail<T_, nx::null_t>& r)
            : head_(r.head_)
        {}

        template <typename T_>
        detail& operator=(const detail<T_, nx::null_t>& r)
        {
            head_ = r.head_;
            return (*this);
        }

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

template <NX_PP_TYPE_MAX_1(typename T, = nx::null_t)>
class tuple : public types<private_tuple::detail<T1>, NX_PP_B1(NX_PP_TYPE_MAX_1(T))>::type_t
{
public:
    typedef typename types<private_tuple::detail<T1>, NX_PP_B1(NX_PP_TYPE_MAX_1(T))>::type_t base_t;

public:
    tuple(void) {}

#define NX_TUPLE_(n) \
    tuple(NX_PP_TYPE_2(n, typename traits<T, >::param_t par)) \
        : base_t(NX_PP_TYPE_1(n, par), NX_PP_CLONE(NX_PP_REM(n), nx::none, ,)) \
    {}
    NX_PP_MULT(NX_PP_DEC(NX_PP_MAX), NX_TUPLE_)
#undef NX_TUPLE_

    tuple(NX_PP_TYPE_MAX_2(typename traits<T, >::param_t par))
        : base_t(NX_PP_TYPE_MAX_1(par))
    {}

    template<NX_PP_TYPE_MAX_1(typename U)>
    tuple(const tuple<NX_PP_TYPE_MAX_1(U)>& r)
        : base_t(r)
    {}

    template<class U1, class U2>
    tuple& operator=(const private_tuple::detail<U1, U2>& r)
    {
        base_t::operator=(r);
        return (*this);
    }

    template <class U1, class U2>
    tuple& operator=(const std::pair<U1, U2>& r)
    {
        nx_assert_static(nx::types_len<tuple>::value == 2);
        this->head_       = r.first;
        this->tail_.head_ = r.second;
        return (*this);
    }

    template <class U1, class U2>
    void swap(private_tuple::detail<U1, U2>& r)
    {
        base_t::swap(r);
    }

    size_t length(void)
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

template <NX_PP_TYPE_MAX_1(typename T), NX_PP_TYPE_MAX_1(typename U)>
inline void swap(tuple<NX_PP_TYPE_MAX_1(T)>& x, tuple<NX_PP_TYPE_MAX_1(U)>& y)
{
    x.swap(y);
}

/*
    Special list algorithms
*/

#define NX_TYPENAME_(n) , typename
NX_SPECIAL_TYPES(tuple, typename NX_PP_MULT(NX_PP_DEC(NX_PP_MAX), NX_TYPENAME_))
#undef NX_TYPENAME_

/*
    Tie variables to be a tuple
*/

#define NX_TIE_(n) \
template <NX_PP_TYPE_1(n, typename T)> \
inline tuple<NX_PP_TYPE_1(n, T, &)> tie(NX_PP_TYPE_2(n, T, & par)) \
{ \
    return tuple<NX_PP_TYPE_1(n, T, &)>(NX_PP_TYPE_1(n, par)); \
}
NX_PP_MULT_MAX(NX_TIE_)
#undef NX_TIE_

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
