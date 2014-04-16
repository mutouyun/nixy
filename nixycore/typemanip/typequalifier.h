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

#ifdef NX_SP_CXX11_TYPE_TRAITS
#include <type_traits>
#endif

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

#ifdef NX_SP_CXX11_TYPE_TRAITS

template <typename T> struct is_const    : type_if<std::is_const   <T>::value> {};
template <typename T> struct is_volatile : type_if<std::is_volatile<T>::value> {};

template <typename T> struct rm_const    { typedef typename std::remove_const   <T>::type type_t; };
template <typename T> struct rm_volatile { typedef typename std::remove_volatile<T>::type type_t; };
template <typename T> struct rm_cv       { typedef typename std::remove_cv      <T>::type type_t; };

#else /*NX_SP_CXX11_TYPE_TRAITS*/

/*
    detect type qualifiers
*/

#define NX_QUALIFIER_DEF(q) \
template <typename T>           struct NX_PP_JOIN(is_, q)         : type_if<false> {}; \
template <typename T>           struct NX_PP_JOIN(is_, q)<q T>    : type_if<true>  {}; \
template <typename T>           struct NX_PP_JOIN(is_, q)<q T&>   : type_if<true>  {}; \
template <typename T>           struct NX_PP_JOIN(is_, q)<q T[]>  : type_if<true>  {}; \
template <typename T, size_t N> struct NX_PP_JOIN(is_, q)<q T[N]> : type_if<true>  {}; \
template <typename T>           struct NX_PP_JOIN(is_, q)<T* q>   : type_if<true>  {};

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

#endif/*NX_SP_CXX11_TYPE_TRAITS*/

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
