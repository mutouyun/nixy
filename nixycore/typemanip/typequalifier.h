/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "typemanip/typedefs.h"

#include "general/general.h"
#include "preprocessor/preprocessor.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    detect type qualifiers
*/

#define NX_QUALIFIER_DEF(q) \
template <typename T>           struct NX_PP_JOIN(is_, q)         : Judge<false> {}; \
template <typename T>           struct NX_PP_JOIN(is_, q)<q T>    : Judge<true>  {}; \
template <typename T>           struct NX_PP_JOIN(is_, q)<q T&>   : Judge<true>  {}; \
template <typename T>           struct NX_PP_JOIN(is_, q)<q T[]>  : Judge<true>  {}; \
template <typename T, size_t N> struct NX_PP_JOIN(is_, q)<q T[N]> : Judge<true>  {}; \
template <typename T>           struct NX_PP_JOIN(is_, q)<T* q>   : Judge<true>  {};

NX_QUALIFIER_DEF(const)
NX_QUALIFIER_DEF(volatile)

#undef NX_QUALIFIER_DEF

/*
    remove type qualifiers
*/

#define NX_QUALIFIER_DEF(q) \
template <typename T>           struct NX_PP_JOIN(rm_, q)         { typedef T  type_t; }; \
template <typename T>           struct NX_PP_JOIN(rm_, q)<q T>    { typedef T  type_t; }; \
template <typename T>           struct NX_PP_JOIN(rm_, q)<q T&>   { typedef T& type_t; }; \
template <typename T>           struct NX_PP_JOIN(rm_, q)<q T[]>  { typedef T  type_t[]; }; \
template <typename T, size_t N> struct NX_PP_JOIN(rm_, q)<q T[N]> { typedef T  type_t[N]; }; \
template <typename T>           struct NX_PP_JOIN(rm_, q)<T* q>   { typedef T* type_t; };

NX_QUALIFIER_DEF(const)
NX_QUALIFIER_DEF(volatile)

#undef NX_QUALIFIER_DEF

template <typename T>
struct rm_cv
{
    typedef typename rm_const<typename rm_volatile<T>::type_t>::type_t type_t;
};

/*
    copy type qualifiers
*/

#define NX_QUALIFIER_DEF(q) \
template <typename T, typename R, bool = nx::NX_PP_JOIN(is_, q)<T>::value> \
struct NX_PP_JOIN(cp_, q)             { typedef R   type_t; }; \
template <typename T, typename R> \
struct NX_PP_JOIN(cp_, q)<T, R, true> { typedef R q type_t; };

NX_QUALIFIER_DEF(const)
NX_QUALIFIER_DEF(volatile)

#undef NX_QUALIFIER_DEF

template <typename T, typename R>
struct cp_cv
{
    typedef typename cp_const<T, typename cp_volatile<T, R>::type_t>::type_t type_t;
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
