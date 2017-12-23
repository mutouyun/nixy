/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#pragma once

#include "nixycore/typemanip/typedefs.h"
#include "nixycore/typemanip/typetools.h"
#include "nixycore/typemanip/typequalifier.h"

#include "nixycore/general/general.h"
#include "nixycore/preprocessor/preprocessor.h"

#ifdef NX_SP_CXX11_TYPE_TRAITS
#include <type_traits>
#endif

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    reference
*/

#ifdef NX_SP_CXX11_TYPE_TRAITS

/* lvalue_reference */

template <typename T>
struct is_lvalue_reference
    : type_if<std::is_lvalue_reference<T>::value>
{};

namespace private_rm_lvalue_reference
{
    template <typename T, bool = is_lvalue_reference<T>::value>
    struct detail
    {
        typedef T type_t;
    };

    template <typename T>
    struct detail<T, true>
    {
        typedef typename std::remove_reference<T>::type type_t;
    };
}

template <typename T>
struct rm_lvalue_reference
    : private_rm_lvalue_reference::detail<T>
{};

namespace private_cp_lvalue_reference
{
    template <typename T, typename R, bool = is_lvalue_reference<T>::value>
    struct detail
    {
        typedef R type_t;
    };

    template <typename T, typename R>
    struct detail<T, R, true>
    {
        typedef typename std::add_lvalue_reference<R>::type type_t;
    };
}

template <typename T, typename R>
struct cp_lvalue_reference
    : private_cp_lvalue_reference::detail<T, R>
{};

/* rvalue_reference */

template <typename T>
struct is_rvalue_reference
    : type_if<std::is_rvalue_reference<T>::value>
{};

namespace private_rm_rvalue_reference
{
    template <typename T, bool = is_rvalue_reference<T>::value>
    struct detail
    {
        typedef T type_t;
    };

    template <typename T>
    struct detail<T, true>
    {
        typedef typename std::remove_reference<T>::type type_t;
    };
}

template <typename T>
struct rm_rvalue_reference
    : private_rm_rvalue_reference::detail<T>
{};

namespace private_cp_rvalue_reference
{
    template <typename T, typename R, bool = is_rvalue_reference<T>::value>
    struct detail
    {
        typedef R type_t;
    };

    template <typename T, typename R>
    struct detail<T, R, true>
    {
        typedef typename std::add_rvalue_reference<R>::type type_t;
    };
}

template <typename T, typename R>
struct cp_rvalue_reference
    : private_cp_rvalue_reference::detail<T, R>
{};

/* lvalue or rvalue reference */

template <typename T>
struct is_reference
    : type_if<std::is_reference<T>::value>
{};

template <typename T>
struct rm_reference
{
    typedef typename std::remove_reference<T>::type type_t;
};

namespace private_cp_reference
{
    template <typename T, typename R, bool = is_lvalue_reference<T>::value,
                                      bool = is_rvalue_reference<T>::value>
    struct detail
    {
        typedef R type_t;
    };

    template <typename T, typename R>
    struct detail<T, R, true, false> : cp_lvalue_reference<T, R>
    {};

    template <typename T, typename R>
    struct detail<T, R, false, true> : cp_rvalue_reference<T, R>
    {};
}

template <typename T, typename R>
struct cp_reference
    : private_cp_reference::detail<T, R>
{};

#else /*NX_SP_CXX11_TYPE_TRAITS*/

template <typename T>             struct is_lvalue_reference        : type_if<false> {};
template <typename T>             struct is_lvalue_reference<T&>    : type_if<true>  {};
template <typename T>             struct rm_lvalue_reference        { typedef T type_t; };
template <typename T>             struct rm_lvalue_reference<T&>    { typedef T type_t; };
template <typename T, typename R> struct cp_lvalue_reference        { typedef R  type_t; };
template <typename T, typename R> struct cp_lvalue_reference<T&, R> { typedef R& type_t; };

template <typename T>             struct is_rvalue_reference        : type_if<false> {};
template <typename T>             struct rm_rvalue_reference        { typedef T type_t; };
template <typename T, typename R> struct cp_rvalue_reference        { typedef R type_t; };

template <typename T>
struct is_reference
    : type_if<is_lvalue_reference<T>::value ||
              is_rvalue_reference<T>::value>
{};

namespace private_rm_reference
{
    template <typename T, bool = is_lvalue_reference<T>::value,
                          bool = is_rvalue_reference<T>::value>
    struct detail
    {
        typedef T type_t;
    };

    template <typename T>
    struct detail<T, true, false> : rm_lvalue_reference<T>
    {};

    template <typename T>
    struct detail<T, false, true> : rm_rvalue_reference<T>
    {};
}

template <typename T>
struct rm_reference
    : private_rm_reference::detail<T>
{};

namespace private_cp_reference
{
    template <typename T, typename R,
                          bool = is_lvalue_reference<T>::value,
                          bool = is_rvalue_reference<T>::value>
    struct detail
    {
        typedef R type_t;
    };

    template <typename T, typename R>
    struct detail<T, R, true, false> : cp_lvalue_reference<T, R>
    {};

    template <typename T, typename R>
    struct detail<T, R, false, true> : cp_rvalue_reference<T, R>
    {};
}

template <typename T, typename R>
struct cp_reference
    : private_cp_reference::detail<T, R>
{};

#endif/*NX_SP_CXX11_TYPE_TRAITS*/

/*
    array
*/

#ifdef NX_SP_CXX11_TYPE_TRAITS

template <typename T> struct is_array   : type_if<std::is_array<T>::value> {};
template <typename T> struct rm_array   { typedef typename std::remove_extent     <T>::type type_t; };
template <typename T> struct rm_extents { typedef typename std::remove_all_extents<T>::type type_t; };

#else /*NX_SP_CXX11_TYPE_TRAITS*/

template <typename T>           struct is_array         : type_if<false> {};
template <typename T>           struct is_array<T[]>    : type_if<true>  {};
template <typename T, size_t N> struct is_array<T[N]>   : type_if<true>  {};

template <typename T>           struct rm_array         { typedef T type_t; };
template <typename T>           struct rm_array<T[]>    { typedef T type_t; };
template <typename T, size_t N> struct rm_array<T[N]>   { typedef T type_t; };

template <typename T>           struct rm_extents       { typedef T type_t; };
template <typename T>           struct rm_extents<T[]>  { typedef typename rm_extents<T>::type_t type_t; };
template <typename T, size_t N> struct rm_extents<T[N]> { typedef typename rm_extents<T>::type_t type_t; };

#endif/*NX_SP_CXX11_TYPE_TRAITS*/

template <typename T, typename R>           struct cp_array          { typedef R type_t;    };
template <typename T, typename R>           struct cp_array<T[] , R> { typedef R type_t[];  };
template <typename T, typename R, size_t N> struct cp_array<T[N], R> { typedef R type_t[N]; };

/*
    pointer
*/

namespace private_is_pointer
{
    template <typename T>             struct detail         : type_if<false> {};
    template <typename T>             struct detail<T*>     : type_if<true>  {};
    template <typename T, typename C> struct detail<T C::*> : type_if<true>  {};
#ifdef NX_SP_CXX11_TEMPLATES
    template <typename R, typename C, typename... P> struct detail<R(C::*)(P...) const>
        : type_if<true> {};
    template <typename R, typename C, typename... P> struct detail<R(C::*)(P...) volatile>
        : type_if<true> {};
    template <typename R, typename C, typename... P> struct detail<R(C::*)(P...) const volatile>
        : type_if<true> {};
#else /*NX_SP_CXX11_TEMPLATES*/
    template <typename R, typename C> struct detail<R(C::*)() const>          : type_if<true> {};
    template <typename R, typename C> struct detail<R(C::*)() volatile>       : type_if<true> {};
    template <typename R, typename C> struct detail<R(C::*)() const volatile> : type_if<true> {};
#define NX_IS_POINTER_(n) \
    template <typename R, typename C, NX_PP_TYPE_1(n, typename P)> struct detail<R(C::*)(NX_PP_TYPE_1(n, P)) const> \
        : type_if<true> {}; \
    template <typename R, typename C, NX_PP_TYPE_1(n, typename P)> struct detail<R(C::*)(NX_PP_TYPE_1(n, P)) volatile> \
        : type_if<true> {}; \
    template <typename R, typename C, NX_PP_TYPE_1(n, typename P)> struct detail<R(C::*)(NX_PP_TYPE_1(n, P)) const volatile> \
        : type_if<true> {};
    NX_PP_MULT_MAX(NX_IS_POINTER_)
#undef NX_IS_POINTER_
#endif/*NX_SP_CXX11_TEMPLATES*/
}

template <typename T> struct is_pointer
    : private_is_pointer::detail<typename nx::rm_cv<T>::type_t>
{};

namespace private_rm_pointer
{
    template <typename T>             struct detail         { typedef T type_t; };
    template <typename T>             struct detail<T*>     { typedef T type_t; };
    template <typename T, typename C> struct detail<T C::*> { typedef T type_t; };
#ifdef NX_SP_CXX11_TEMPLATES
    template <typename R, typename C, typename... P> struct detail<R(C::*)(P...) const>
    { typedef R(type_t)(P...); };
    template <typename R, typename C, typename... P> struct detail<R(C::*)(P...) volatile>
    { typedef R(type_t)(P...); };
    template <typename R, typename C, typename... P> struct detail<R(C::*)(P...) const volatile>
    { typedef R(type_t)(P...); };
#else /*NX_SP_CXX11_TEMPLATES*/
    template <typename R, typename C> struct detail<R(C::*)() const>          { typedef R(type_t)(); };
    template <typename R, typename C> struct detail<R(C::*)() volatile>       { typedef R(type_t)(); };
    template <typename R, typename C> struct detail<R(C::*)() const volatile> { typedef R(type_t)(); };
#define NX_RM_POINTER_(n) \
    template <typename R, typename C, NX_PP_TYPE_1(n, typename P)> struct detail<R(C::*)(NX_PP_TYPE_1(n, P)) const> \
    { typedef R(type_t)(NX_PP_TYPE_1(n, P)); }; \
    template <typename R, typename C, NX_PP_TYPE_1(n, typename P)> struct detail<R(C::*)(NX_PP_TYPE_1(n, P)) volatile> \
    { typedef R(type_t)(NX_PP_TYPE_1(n, P)); }; \
    template <typename R, typename C, NX_PP_TYPE_1(n, typename P)> struct detail<R(C::*)(NX_PP_TYPE_1(n, P)) const volatile> \
    { typedef R(type_t)(NX_PP_TYPE_1(n, P)); };
    NX_PP_MULT_MAX(NX_RM_POINTER_)
#undef NX_RM_POINTER_
#endif/*NX_SP_CXX11_TEMPLATES*/
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
