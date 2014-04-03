/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/typemanip/typedefs.h"
#include "nixycore/typemanip/typetools.h"

#include "nixycore/general/general.h"
#include "nixycore/preprocessor/preprocessor.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    Check has member type
*/

#define NX_HAS_MEMBERTYPE_(name, InnerType) \
    template <class T> \
    struct has_##name \
    { \
        template <class U> \
        static nx::yes_t check(typename U::InnerType*); \
        template <class U> \
        static nx::not_t check(...); \
        NX_STATIC_VALUE( bool, nx_rightof(check<T>(nx::nulptr)) ); \
    };

/*
    Check has member function
*/

#define NX_HAS_MEMBERFUNC_(name, R, InnerFunc, qf, ...) \
    template <class T> \
    struct has_##name \
    { \
        template <class U, NX_PP_VA(R (U::*)(__VA_ARGS__) qf)> \
        struct FuncTest; \
        template <class U> \
        static nx::yes_t check(FuncTest<U, &U::InnerFunc>*); \
        template <class U> \
        static nx::not_t check(...); \
        NX_STATIC_VALUE( bool, nx_rightof(check<T>(nx::nulptr)) ); \
    };

/*
    Define a concept for member type or function
*/

#define NX_CONCEPT(...) \
    NX_PP_JOIN(NX_, NX_PP_LESS(2, NX_PP_COUNT(__VA_ARGS__), HAS_MEMBERFUNC_, HAS_MEMBERTYPE_))(__VA_ARGS__)

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
