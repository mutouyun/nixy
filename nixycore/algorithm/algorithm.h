/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "nixycore/algorithm/container.h"
#include "nixycore/algorithm/foreach.h"
#include "nixycore/algorithm/assign.h"
#include "nixycore/algorithm/series.h"

//////////////////////////////////////////////////////////////////////////

#include "nixycore/typemanip/typetools.h"
#include "nixycore/typemanip/typedetect.h"

#include "nixycore/general/general.h"

#include <algorithm> // std algorithms
#include <math.h>    // fabs...

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

using std::swap;
using std::copy;
using std::fill;
using std::fill_n;
using std::sort;
using std::equal;
using std::find;

/*
    using std::lexicographical_compare for compare
*/

template <typename IterT1, typename IterT2>
inline bool compare(IterT1 first1, IterT1 last1, IterT2 first2, IterT2 last2)
{
    return std::lexicographical_compare(first1, last1, first2, last2);
}

template <typename IterT1, typename IterT2, typename CompT>
inline bool compare(IterT1 first1, IterT1 last1, IterT2 first2, IterT2 last2, CompT func)
{
    return std::lexicographical_compare(first1, last1, first2, last2, func);
}

//////////////////////////////////////////////////////////////////////////

/*
    Swap two array
*/

#if defined(NX_CC_MSVC) && (NX_CC_MSVC <= 1500)
template <typename T, size_t N>
inline void swap(T(&x)[N], T(&y)[N])
{
    nx_foreach(i, N)
        nx::swap(x[i], y[i]);
}
#endif

/*
    Copy container/variable
*/

namespace private_copy
{
    template <typename T, typename U>
    inline typename enable_if<is_container<T>::value && 
                              is_container<U>::value, 
    T&>::type_t detail(T& src, U& dst)
    {
        nx::copy(nx::begin(src), nx::end(src), nx::begin(dst));
        return dst;
    }

    template <typename T, typename U>
    inline typename enable_if<!is_container<T>::value || 
                              !is_container<U>::value, 
    T&>::type_t detail(T& src, U& dst)
    {
        return dst = src;
    }
}

template <typename T, typename U>
inline T& copy(T& src, U& dst)
{
    return private_copy::detail(src, dst);
}

/*
    Fill range
*/

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

/*
    Sort
*/

template <typename T>
inline void sort(T& dst)
{
    nx::sort(nx::begin(dst), nx::end(dst));
}

template <typename T, typename CompT>
inline void sort(T& dst, CompT func)
{
    nx::sort(nx::begin(dst), nx::end(dst), func);
}

/*
    Compare
*/

namespace private_equal
{
    inline bool is_equal(float x, float y)
    {
        return (fabs(x - y) < 0.000001f);
    }

    inline bool is_equal(double x, double y)
    {
        return (fabs(x - y) < (double)0.000001);
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
        return (memcmp(&x, &y, sizeof(T)) == 0);
    }

    template <typename T, typename U, size_t N1, size_t N2>
    inline typename enable_if<is_pod<T>::value && 
                              is_pod<U>::value, 
    bool>::type_t is_equal(const T(& x)[N1], const U(& y)[N2])
    {
        return ((N1 == N2) ? (memcmp(x, y, sizeof(x)) == 0) : false);
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

template <typename T, typename U>
inline bool compare(T& x, U& y)
{
    return nx::compare(nx::begin(x), nx::end(x), nx::begin(y), nx::end(y));
}

template <typename T, typename U, typename CompT>
inline bool compare(T& x, U& y, CompT func)
{
    return nx::compare(nx::begin(x), nx::end(x), nx::begin(y), nx::end(y), func);
}

/*
    Find
*/

template <typename T, typename V>
inline typename container_traits<T>::ite_t find(T& x, const V& v)
{
    return nx::find(nx::begin(x), nx::end(x), v);
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
