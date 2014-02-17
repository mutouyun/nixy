/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/typemanip/typedefs.h"
#include "nixycore/typemanip/typetools.h"
#include "nixycore/typemanip/typelist.h"
#include "nixycore/typemanip/typeconcept.h"
#include "nixycore/typemanip/typequalifier.h"
#include "nixycore/typemanip/typebehavior.h"

#include "nixycore/general/general.h"
#include "nixycore/preprocessor/preprocessor.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    detect void
*/

namespace private_is_void
{
    template <typename T>
    struct detail
        : type_if<false>
    {};

    template <>
    struct detail<void>
        : type_if<true>
    {};
}

template <typename T>
struct is_void
    : private_is_void::detail<typename rm_cv<T>::type_t>
{};

/*
    detect integral
*/

typedef types<bool , char  , uchar, wchar,
              short, ushort, int  , uint , 
              long , ulong , llong, ullong>::type_t integral_types_t;

template <typename T>
struct is_integral
    : types_exist<integral_types_t, typename rm_cv<T>::type_t>
{};

/*
    detect floating point
*/

typedef nx::types<float, double, ldouble>::type_t float_types_t;

template <typename T>
struct is_float
    : types_exist<float_types_t, typename rm_cv<T>::type_t>
{};

/*
    detect numeric
*/

template <typename T>
struct is_numeric
    : type_if<is_integral<T>::value ||
              is_float   <T>::value>
{};

/*
    detect signed
*/

typedef types<char, short, int, long, llong>::type_t signed_types_t;

template <typename T>
struct is_signed
    : types_exist<signed_types_t, typename rm_cv<T>::type_t>
{};

/*
    detect unsigned
*/

typedef types<uchar, ushort, uint, ulong, ullong>::type_t unsigned_types_t;

template <typename T>
struct is_unsigned
    : types_exist<unsigned_types_t, typename rm_cv<T>::type_t>
{};

/*
    detect union
*/

template <typename T>
struct is_union
    : type_if<__is_union(T)>
{};

/*
    detect class
*/

template <typename T>
struct is_class
    : type_if<__is_class(T)>
{};

/*
    detect function type/pointer
*/

namespace private_is_function
{
    template <typename T>
    struct detail
    {
        template <typename U>
        static nx::yes_t check(U*);
        template <typename U>
        static nx::not_t check(...);
        NX_STATIC_VALUE( bool, (!is_class<T>::value) && nx_rightof(check<T>(*(T*)0)) );
    };

    template <>
    struct detail<nx::null_t>
        : type_if<false>
    {};

    template <>
    struct detail<void>
        : type_if<false>
    {};
}

template <typename T>
struct is_function
    : private_is_function::detail<typename rm_pointer
                                 <typename rm_reference
                                 <typename rm_cv
                                 <T
    >::type_t
    >::type_t
    >::type_t
    >
{};

/*
    detect member function pointer
*/

namespace private_is_mem_function
{
    template <typename T>             struct detail                           : type_if<false> {};
    template <typename T, typename C> struct detail<T C::*>                   : is_function<T> {};
    template <typename R, typename C> struct detail<R(C::*)() const>          : type_if<true>  {};
    template <typename R, typename C> struct detail<R(C::*)() volatile>       : type_if<true>  {};
    template <typename R, typename C> struct detail<R(C::*)() const volatile> : type_if<true>  {};
#define NX_IS_MEM_FUNCTION_(n) \
    template <typename R, typename C, NX_PP_TYPE_1(n, typename P)> struct detail<R(C::*)(NX_PP_TYPE_1(n, P)) const> \
        : type_if<true> {}; \
    template <typename R, typename C, NX_PP_TYPE_1(n, typename P)> struct detail<R(C::*)(NX_PP_TYPE_1(n, P)) volatile> \
        : type_if<true> {}; \
    template <typename R, typename C, NX_PP_TYPE_1(n, typename P)> struct detail<R(C::*)(NX_PP_TYPE_1(n, P)) const volatile> \
        : type_if<true> {};
    NX_PP_MULT_MAX(NX_IS_MEM_FUNCTION_)
#undef NX_IS_MEM_FUNCTION_
}

template <typename T>
struct is_mem_function
    : private_is_mem_function::detail<typename rm_cv<T>::type_t>
{};

/*
    detect member object pointer
*/

namespace private_is_mem_object_pointer
{
    template <typename T>
    struct detail
        : type_if<false>
    {};

    template <typename T, typename C>
    struct detail<T C::*>
        : type_if<!is_function<T>::value>
    {};
}

template <typename T>
struct is_mem_object_pointer
    : private_is_mem_object_pointer::detail<typename rm_cv<T>::type_t>
{};

/*
    detect member pointer
*/

template <typename T>
struct is_mem_pointer
    : type_if<is_mem_object_pointer<T>::value ||
              is_mem_function      <T>::value>
{};

/*
    detect enum
*/

template <typename T>
struct is_enum
    : type_if<!is_numeric    <T>::value &&
              !is_void       <T>::value &&
              !is_array      <T>::value &&
              !is_pointer    <T>::value &&
              !is_reference  <T>::value &&
              !is_mem_pointer<T>::value &&
              !is_union      <T>::value &&
              !is_class      <T>::value>
{};

/*
    detect fundamental type
*/

template <typename T>
struct is_fundamental
    : type_if<is_numeric<T>::value ||
              is_void   <T>::value ||
              is_enum   <T>::value ||
              is_pointer<T>::value>
{};

/*
    detect POD
*/

#include "nixycore/al/typemanip/private_is_pod.hxx"

namespace private_is_pod
{
    template <>
    struct detail<nx::null_t>
        : type_if<false>
    {};
}

template <typename T>
struct is_pod
    : private_is_pod::detail<typename rm_extents
                            <T
    >::type_t
    >
{};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
