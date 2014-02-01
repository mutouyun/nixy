/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "algorithm/container.h"
#include "algorithm/foreach.h"
#include "algorithm/assign.h"
#include "algorithm/series.h"

//////////////////////////////////////////////////////////////////////////

#include "general/general.h"
#include "typemanip/typemanip.h"

// std algorithms
#include <algorithm>
// std::fabs...
#include <cmath>

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    Swap two variables
*/

NX_CONCEPT(void_swap, void, swap, U&)
NX_CONCEPT(refr_swap, U&  , swap, U&)

template <typename T>
inline typename enable_if<has_void_swap<T>::value || 
                          has_refr_swap<T>::value
>::type_t swap(T& x, T& y)
{
    x.swap(y);
}

template <typename T>
inline typename enable_if<!has_void_swap<T>::value && 
                          !has_refr_swap<T>::value
>::type_t swap(T& x, T& y)
{
    std::swap(x, y);
}

template <typename T, size_t N>
inline void swap(T(&x)[N], T(&y)[N])
{
    nx_foreach(i, N)
        nx::swap(x[i], y[i]);
}

/*
    Copy container/variable
*/

template <typename R, typename Iter_>
inline R copy(R r, Iter_ first, Iter_ last)
{
    return std::copy(first, last, r);
}

namespace private_copy
{
    template <typename T, typename U>
    inline typename enable_if<is_container<T>::value && 
                              is_container<U>::value, 
    T&>::type_t detail(T& dst, U& src)
    {
        nx::copy(nx::begin(dst), nx::begin(src), nx::end(src));
        return dst;
    }

    template <typename T, typename U>
    inline typename enable_if<!is_container<T>::value || 
                              !is_container<U>::value, 
    T&>::type_t detail(T& dst, U& src)
    {
        return dst = src;
    }
}

template <typename T, typename U>
inline T& copy(T& dst, U& src)
{
    return private_copy::detail(dst, src);
}

/*
    Fill range
*/

template <typename Iter_, typename V>
inline void fill(Iter_ first, Iter_ last, const V& v)
{
    std::fill(first, last, v);
}

namespace private_fill
{
    template <typename T, typename V>
    inline typename enable_if<is_container<T>::value
    >::type_t detail(T& dst, const V& v)
    {
        nx::fill(nx::begin(dst), nx::end(dst), v);
    }

    template <typename T, typename V>
    inline typename enable_if<!is_container<T>::value
    >::type_t detail(T& dst, const V& v)
    {
        dst = static_cast<T>(v);
    }
}

template <typename T, typename V>
inline void fill(T& dst, const V& v)
{
    private_fill::detail(dst, v);
}

template <typename T, typename S, typename V>
inline T fill_n(T first, S n, const V& v)
{
    return std::fill_n(first, n, v);
}

/*
    Sort
*/

template <typename Iter_>
inline void sort(Iter_ first, Iter_ last)
{
    std::sort(first, last);
}

template <typename Iter_, typename Comp_>
inline void sort(Iter_ first, Iter_ last, Comp_ func)
{
    std::sort(first, last, func);
}

template <typename T>
inline void sort(T& dst)
{
    nx::sort(nx::begin(dst), nx::end(dst));
}

template <typename T, typename Comp_>
inline void sort(T& dst, Comp_ func)
{
    nx::sort(nx::begin(dst), nx::end(dst), func);
}

/*
    Compare
*/

template <typename Iter1_, typename Iter2_>
inline bool equal(Iter1_ first1, Iter1_ last1, Iter2_ first2)
{
    return std::equal(first1, last1, first2);
}

template <typename Iter1_, typename Iter2_, typename Pred_>
inline bool equal(Iter1_ first1, Iter1_ last1, Iter2_ first2, Pred_ func)
{
    return std::equal(first1, last1, first2, func);
}

namespace private_equal
{
    inline bool is_equal(float x, float y)
    {
        return (std::fabs(x - y) < 0.000001f);
    }

    inline bool is_equal(double x, double y)
    {
        return (std::fabs(x - y) < (double)0.000001);
    }

    template <typename T, typename U>
    inline bool is_equal(const T& x, const U& y)
    {
        return (x == y);
    }

    template <typename T>
    inline typename enable_if<is_union<T>::value, 
    bool>::type_t is_equal(const T& x, const T& y)
    {
        return (std::memcmp(&x, &y, sizeof(T)) == 0);
    }

    template <typename T, typename U, size_t N1, size_t N2>
    inline typename enable_if<is_pod<T>::value && 
                              is_pod<U>::value, 
    bool>::type_t is_equal(const T(& x)[N1], const U(& y)[N2])
    {
        return ((N1 == N2) ? (std::memcmp(x, y, sizeof(x)) == 0) : false);
    }

    template <typename T, typename U, size_t N1, size_t N2>
    inline typename enable_if<!is_pod<T>::value || 
                              !is_pod<U>::value, 
    bool>::type_t is_equal(const T(& x)[N1], const U(& y)[N2])
    {
        if (N1 != N2) return false;
        bool ret = true;
        for(size_t i = 0; i < N1; ++i)
        {
            if (!is_equal(x[i], y[i]))
            {
                ret = false;
                break;
            }
        }
        return ret;
    }

    template <typename T, typename U>
    inline typename enable_if<is_container<T>::value && 
                              is_container<U>::value, 
    bool>::type_t detail(T& x, U& y)
    {
        return nx::equal(nx::begin(x), nx::end(x), nx::begin(y));
    }

    template <typename T, typename U>
    inline typename enable_if<!is_container<T>::value || 
                              !is_container<U>::value, 
    bool>::type_t detail(T& x, U& y)
    {
        return is_equal(x, y);
    }
}

template <typename T, typename U>
inline bool equal(T& x, U& y)
{
    return private_equal::detail(x, y);
}

template <typename Iter1_, typename Iter2_>
inline bool compare(Iter1_ first1, Iter1_ last1, Iter2_ first2, Iter2_ last2)
{
    return std::lexicographical_compare(first1, last1, first2, last2);
}

template <typename Iter1_, typename Iter2_, typename Comp_>
inline bool compare(Iter1_ first1, Iter1_ last1, Iter2_ first2, Iter2_ last2, Comp_ func)
{
    return std::lexicographical_compare(first1, last1, first2, last2, func);
}

template <typename T, typename U>
inline bool compare(T& x, U& y)
{
    return nx::compare(nx::begin(x), nx::end(x), nx::begin(y), nx::end(y));
}

template <typename T, typename U, typename Comp_>
inline bool compare(T& x, U& y, Comp_ func)
{
    return nx::compare(nx::begin(x), nx::end(x), nx::begin(y), nx::end(y), func);
}

/*
    Find
*/

template <typename Iter_, typename V>
inline Iter_ find(Iter_ first, Iter_ last, const V& v)
{
    return std::find(first, last, v);
}

template <typename T, typename V>
inline typename container_traits<T>::ite_t find(T& x, const V& v)
{
    return nx::find(nx::begin(x), nx::end(x), v);
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
