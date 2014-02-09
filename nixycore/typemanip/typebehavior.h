/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/typemanip/typedefs.h"
#include "nixycore/typemanip/typequalifier.h"

#include "nixycore/general/general.h"
#include "nixycore/preprocessor/preprocessor.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    reference
*/

template <typename T>                       struct is_reference        : Judge<false> {};
template <typename T>                       struct is_reference<T&>    : Judge<true>  {};

template <typename T>                       struct rm_reference        { typedef T type_t; };
template <typename T>                       struct rm_reference<T&>    { typedef T type_t; };

template <typename T, typename R>           struct cp_reference        { typedef R  type_t; };
template <typename T, typename R>           struct cp_reference<T&, R> { typedef R& type_t; };

/*
    array
*/

template <typename T>                       struct is_array            : Judge<false> {};
template <typename T>                       struct is_array<T[]>       : Judge<true>  {};
template <typename T, size_t N>             struct is_array<T[N]>      : Judge<true>  {};

template <typename T>                       struct rm_array            { typedef T type_t; };
template <typename T>                       struct rm_array<T[]>       { typedef T type_t; };
template <typename T, size_t N>             struct rm_array<T[N]>      { typedef T type_t; };

template <typename T, typename R>           struct cp_array            { typedef R type_t;    };
template <typename T, typename R>           struct cp_array<T[] , R>   { typedef R type_t[];  };
template <typename T, typename R, size_t N> struct cp_array<T[N], R>   { typedef R type_t[N]; };

template <typename T>                       struct rm_extents          { typedef T type_t; };
template <typename T>                       struct rm_extents<T[]>     { typedef typename rm_extents<T>::type_t type_t; };
template <typename T, size_t N>             struct rm_extents<T[N]>    { typedef typename rm_extents<T>::type_t type_t; };

/*
    pointer
*/

namespace private_is_pointer
{
    template <typename T>                   struct detail                           : Judge<false> {};
    template <typename T>                   struct detail<T*>                       : Judge<true>  {};
    template <typename T, typename C>       struct detail<T C::*>                   : Judge<true>  {};
    template <typename R, typename C>       struct detail<R(C::*)() const>          : Judge<true>  {};
    template <typename R, typename C>       struct detail<R(C::*)() volatile>       : Judge<true>  {};
    template <typename R, typename C>       struct detail<R(C::*)() const volatile> : Judge<true>  {};
#define NX_IS_POINTER_(n) \
    template <typename R, typename C, NX_PP_TYPE_1(n, typename P)> struct detail<R(C::*)(NX_PP_TYPE_1(n, P)) const> \
        : Judge<true> {}; \
    template <typename R, typename C, NX_PP_TYPE_1(n, typename P)> struct detail<R(C::*)(NX_PP_TYPE_1(n, P)) volatile> \
        : Judge<true> {}; \
    template <typename R, typename C, NX_PP_TYPE_1(n, typename P)> struct detail<R(C::*)(NX_PP_TYPE_1(n, P)) const volatile> \
        : Judge<true> {};
    NX_PP_MULT_MAX(NX_IS_POINTER_)
#undef NX_IS_POINTER_
}

template <typename T> struct is_pointer
    : private_is_pointer::detail<typename nx::rm_cv<T>::type_t> {};

namespace private_rm_pointer
{
    template <typename T>                   struct detail                           { typedef T type_t; };
    template <typename T>                   struct detail<T*>                       { typedef T type_t; };
    template <typename T, typename C>       struct detail<T C::*>                   { typedef T type_t; };
    template <typename R, typename C>       struct detail<R(C::*)() const>          { typedef R(type_t)(); };
    template <typename R, typename C>       struct detail<R(C::*)() volatile>       { typedef R(type_t)(); };
    template <typename R, typename C>       struct detail<R(C::*)() const volatile> { typedef R(type_t)(); };
#define NX_RM_POINTER_(n) \
    template <typename R, typename C, NX_PP_TYPE_1(n, typename P)> struct detail<R(C::*)(NX_PP_TYPE_1(n, P)) const> \
    { typedef R(type_t)(NX_PP_TYPE_1(n, P)); }; \
    template <typename R, typename C, NX_PP_TYPE_1(n, typename P)> struct detail<R(C::*)(NX_PP_TYPE_1(n, P)) volatile> \
    { typedef R(type_t)(NX_PP_TYPE_1(n, P)); }; \
    template <typename R, typename C, NX_PP_TYPE_1(n, typename P)> struct detail<R(C::*)(NX_PP_TYPE_1(n, P)) const volatile> \
    { typedef R(type_t)(NX_PP_TYPE_1(n, P)); };
    NX_PP_MULT_MAX(NX_RM_POINTER_)
#undef NX_RM_POINTER_
}

template <typename T>
struct rm_pointer
{
private:
    typedef typename private_rm_pointer::detail<typename nx::rm_cv<T>::type_t>::type_t nk_type_t;
public:
    typedef typename nx::cp_cv<T, nk_type_t>::type_t type_t;
};

template <typename T, typename R, bool = nx::is_pointer<T>::value>
struct cp_pointer             { typedef R  type_t; };
template <typename T, typename R>
struct cp_pointer<T, R, true> { typedef R* type_t; };

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
