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

#ifdef NX_SP_CXX11_TYPE_TRAITS
#include <type_traits>
#endif
#ifdef NX_SP_CXX11_CHAR_TYPE
#include <uchar.h>
#endif

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    detect void
*/

#ifdef NX_SP_CXX11_TYPE_TRAITS
template <typename T>
struct is_void
    : type_if<std::is_void<T>::value>
{};
#else /*NX_SP_CXX11_TYPE_TRAITS*/
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
#endif/*NX_SP_CXX11_TYPE_TRAITS*/

/*
    detect character
*/

#ifdef NX_SP_CXX11_CHAR_TYPE
typedef types<char, uchar, wchar, char16_t, char32_t>::type_t character_types_t;
#else
typedef types<char, uchar, wchar>::type_t character_types_t;
#endif

template <typename T>
struct is_character
    : types_exist<character_types_t, typename rm_cv<T>::type_t>
{};

/*
    detect integral
*/

#ifdef NX_SP_CXX11_TYPE_TRAITS
template <typename T>
struct is_integral
    : type_if<std::is_integral<T>::value>
{};
#else /*NX_SP_CXX11_TYPE_TRAITS*/
typedef types<bool , char  , uchar, wchar,
              short, ushort, int  , uint , 
              long , ulong , llong, ullong>::type_t integral_types_t;

template <typename T>
struct is_integral
    : types_exist<integral_types_t, typename rm_cv<T>::type_t>
{};
#endif/*NX_SP_CXX11_TYPE_TRAITS*/

/*
    detect floating point
*/

#ifdef NX_SP_CXX11_TYPE_TRAITS
template <typename T>
struct is_float
    : type_if<std::is_floating_point<T>::value>
{};
#else /*NX_SP_CXX11_TYPE_TRAITS*/
typedef nx::types<float, double, ldouble>::type_t float_types_t;

template <typename T>
struct is_float
    : types_exist<float_types_t, typename rm_cv<T>::type_t>
{};
#endif/*NX_SP_CXX11_TYPE_TRAITS*/

/*
    detect numeric
*/

#ifdef NX_SP_CXX11_TYPE_TRAITS
template <typename T>
struct is_numeric
    : type_if<std::is_arithmetic<T>::value>
{};
#else /*NX_SP_CXX11_TYPE_TRAITS*/
template <typename T>
struct is_numeric
    : type_if<is_integral<T>::value ||
              is_float   <T>::value>
{};
#endif/*NX_SP_CXX11_TYPE_TRAITS*/

/*
    detect signed
*/

#ifdef NX_SP_CXX11_TYPE_TRAITS
template <typename T>
struct is_signed
    : type_if<std::is_signed<T>::value>
{};
#else /*NX_SP_CXX11_TYPE_TRAITS*/
typedef types<char, short, int, long, llong>::type_t signed_types_t;

template <typename T>
struct is_signed
    : types_exist<signed_types_t, typename rm_cv<T>::type_t>
{};
#endif/*NX_SP_CXX11_TYPE_TRAITS*/

/*
    detect unsigned
*/

#ifdef NX_SP_CXX11_TYPE_TRAITS
template <typename T>
struct is_unsigned
    : type_if<std::is_unsigned<T>::value>
{};
#else /*NX_SP_CXX11_TYPE_TRAITS*/
typedef types<uchar, ushort, uint, ulong, ullong>::type_t unsigned_types_t;

template <typename T>
struct is_unsigned
    : types_exist<unsigned_types_t, typename rm_cv<T>::type_t>
{};
#endif/*NX_SP_CXX11_TYPE_TRAITS*/

/*
    detect union
*/

#ifdef NX_SP_CXX11_TYPE_TRAITS
template <typename T>
struct is_union
    : type_if<std::is_union<T>::value>
{};
#else /*NX_SP_CXX11_TYPE_TRAITS*/
template <typename T>
struct is_union
    : type_if<__is_union(T)>
{};
#endif/*NX_SP_CXX11_TYPE_TRAITS*/

/*
    detect class
*/

#ifdef NX_SP_CXX11_TYPE_TRAITS
template <typename T>
struct is_class
    : type_if<std::is_class<T>::value>
{};
#else /*NX_SP_CXX11_TYPE_TRAITS*/
template <typename T>
struct is_class
    : type_if<__is_class(T)>
{};
#endif/*NX_SP_CXX11_TYPE_TRAITS*/

/*
    detect abstract class
*/

#ifdef NX_SP_CXX11_TYPE_TRAITS
template <typename T>
struct is_abstract
    : type_if<std::is_abstract<T>::value>
{};
#else /*NX_SP_CXX11_TYPE_TRAITS*/
namespace private_is_abstract
{
    template <typename T>
    struct detail
    {
        template <typename U>
        static nx::not_t check(U(*)[1]);
        template <typename U>
        static nx::yes_t check(...);
        NX_STATIC_VALUE( bool, nx_judge(check<T>(0)) );
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
struct is_abstract
    : private_is_abstract::detail<typename rm_cv
                                 <T
    >::type_t
    >
{};
#endif/*NX_SP_CXX11_TYPE_TRAITS*/

/*
    detect virtual destructor
*/

#ifdef NX_SP_CXX11_TYPE_TRAITS
template <typename T>
struct has_virtual_destructor
    : type_if<std::has_virtual_destructor<T>::value>
{};
#else /*NX_SP_CXX11_TYPE_TRAITS*/
template <typename T>
struct has_virtual_destructor
    : type_if<__has_virtual_destructor(T)>
{};
#endif/*NX_SP_CXX11_TYPE_TRAITS*/

/*
    detect function type/pointer
*/

#ifdef NX_SP_CXX11_TYPE_TRAITS
template <typename T>
struct is_function
    : type_if<std::is_function<typename rm_pointer
                              <typename rm_reference
                              <T
    >::type_t
    >::type_t
    >::value || std::is_member_function_pointer<T>::value
    >
{};
#else /*NX_SP_CXX11_TYPE_TRAITS*/
namespace private_is_function
{
    template <typename T>
    struct detail
    {
        template <typename U>
        static nx::yes_t check(U*);
        template <typename U>
        static nx::not_t check(...);
        NX_STATIC_VALUE( bool, (!is_class<T>::value) && nx_judge(check<T>(*(T*)0)) );
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
#endif/*NX_SP_CXX11_TYPE_TRAITS*/

/*
    detect member function pointer
*/

#ifdef NX_SP_CXX11_TYPE_TRAITS
template <typename T>
struct is_member_function
    : type_if<std::is_member_function_pointer<T>::value>
{};
#else /*NX_SP_CXX11_TYPE_TRAITS*/
namespace private_is_member_function
{
    template <typename T>             struct detail         : type_if<false> {};
    template <typename T, typename C> struct detail<T C::*> : is_function<T> {};
#ifdef NX_SP_CXX11_TEMPLATES
    template <typename R, typename C, typename... P> struct detail<R(C::*)(P...) const>
        : type_if<true> {};
    template <typename R, typename C, typename... P> struct detail<R(C::*)(P...) volatile>
        : type_if<true> {};
    template <typename R, typename C, typename... P> struct detail<R(C::*)(P...) const volatile>
        : type_if<true> {};
#else /*NX_SP_CXX11_TEMPLATES*/
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
#endif/*NX_SP_CXX11_TEMPLATES*/
}

template <typename T>
struct is_member_function
    : private_is_member_function::detail<typename rm_cv<T>::type_t>
{};
#endif/*NX_SP_CXX11_TYPE_TRAITS*/

/*
    detect member object pointer
*/

#ifdef NX_SP_CXX11_TYPE_TRAITS
template <typename T>
struct is_member_object_pointer
    : type_if<std::is_member_object_pointer<T>::value>
{};
#else /*NX_SP_CXX11_TYPE_TRAITS*/
namespace private_is_member_object_pointer
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
struct is_member_object_pointer
    : private_is_member_object_pointer::detail<typename rm_cv<T>::type_t>
{};
#endif/*NX_SP_CXX11_TYPE_TRAITS*/

/*
    detect member pointer
*/

#ifdef NX_SP_CXX11_TYPE_TRAITS
template <typename T>
struct is_member_pointer
    : type_if<std::is_member_pointer<T>::value>
{};
#else /*NX_SP_CXX11_TYPE_TRAITS*/
template <typename T>
struct is_member_pointer
    : type_if<is_member_object_pointer<T>::value ||
              is_member_function      <T>::value>
{};
#endif/*NX_SP_CXX11_TYPE_TRAITS*/

/*
    detect enum
*/

#ifdef NX_SP_CXX11_TYPE_TRAITS
template <typename T>
struct is_enum
    : type_if<std::is_enum<T>::value>
{};
#else /*NX_SP_CXX11_TYPE_TRAITS*/
template <typename T>
struct is_enum
    : type_if<!is_numeric       <T>::value &&
              !is_void          <T>::value &&
              !is_array         <T>::value &&
              !is_pointer       <T>::value &&
              !is_reference     <T>::value &&
              !is_member_pointer<T>::value &&
              !is_union         <T>::value &&
              !is_class         <T>::value>
{};
#endif/*NX_SP_CXX11_TYPE_TRAITS*/

/*
    detect fundamental type
*/

#ifdef NX_SP_CXX11_TYPE_TRAITS
template <typename T>
struct is_fundamental
    : type_if<std::is_fundamental<T>::value>
{};
#else /*NX_SP_CXX11_TYPE_TRAITS*/
template <typename T>
struct is_fundamental
    : type_if<is_numeric <T>::value ||
              is_void    <T>::value ||
              is_sametype<T, nx::nulptr_t>::value>
{};
#endif/*NX_SP_CXX11_TYPE_TRAITS*/

/*
    detect scalar type
*/

#ifdef NX_SP_CXX11_TYPE_TRAITS
template <typename T>
struct is_scalar
    : type_if<std::is_scalar<T>::value>
{};
#else /*NX_SP_CXX11_TYPE_TRAITS*/
template <typename T>
struct is_scalar
    : type_if<is_numeric <T>::value ||
              is_enum    <T>::value ||
              is_pointer <T>::value || /* nx::is_pointer includes the member pointers */
              is_sametype<T, nx::nulptr_t>::value>
{};
#endif/*NX_SP_CXX11_TYPE_TRAITS*/

/*
    detect POD
*/

#ifdef NX_SP_CXX11_TYPE_TRAITS
template <typename T>
struct is_pod
    : type_if<std::is_pod<T>::value>
{};

template <>
struct is_pod<nx::null_t>
    : type_if<false>
{};
#else /*NX_SP_CXX11_TYPE_TRAITS*/
namespace private_is_pod
{
    template <typename T>
    struct detail
#ifdef NX_CC_MSVC
        : type_if<is_void<T>::value || is_scalar<T>::value ||
                  __has_trivial_constructor(T) && __is_pod(T)>
#elif defined(NX_CC_GNUC)
        : type_if<__is_pod(T)>
#else
        : type_if<is_void<T>::value || is_scalar<T>::value>
#endif
    {};

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
#endif/*NX_SP_CXX11_TYPE_TRAITS*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
