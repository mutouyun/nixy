/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

// nx...
#include "nixycore/general/general.h"
#include "nixycore/preprocessor/preprocessor.h"

// assert
#include <assert.h>

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

// assert && verify

#ifdef	NDEBUG
#   define nx_assert(...)
#else
#   define nx_assert(...)   assert((__VA_ARGS__))
#endif/*NDEBUG*/

#ifdef	NDEBUG
#   define nx_verify(...)   (__VA_ARGS__)
#else
#   define nx_verify(...)   nx_assert(__VA_ARGS__)
#endif/*NDEBUG*/

// Static Assert

#ifndef nx_assert_static
template <bool, int> struct static_assert_failure;
template <int Line_> struct static_assert_failure<true, Line_> {};
#define nx_assert_static(...) \
    enum \
    { \
        NX_PP_JOIN(StaticAssertFailureTest_, __LINE__) = \
            sizeof(nx::static_assert_failure<!!(__VA_ARGS__), __LINE__>) \
    }
#endif

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
