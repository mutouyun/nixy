/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

// nx...
#include "general/general.h"
#include "preprocessor/preprocessor.h"

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
template <bool, int> struct Static_Assert_Failure;
template <int Line_> struct Static_Assert_Failure<true, Line_> {};
#define nx_assert_static(...) \
    enum \
    { \
        NX_PP_JOIN(static_assert_failure_test_, __LINE__) = \
            sizeof(nx::Static_Assert_Failure<!!(__VA_ARGS__), __LINE__>) \
    }
#endif

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
