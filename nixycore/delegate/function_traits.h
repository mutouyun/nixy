/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/preprocessor/preprocessor.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

namespace private_function_traits
{
    template <typename Func_, bool = nx::is_function<Func_>::value>
    struct detail
    /* Circumvent MSVC's ETI errors */
    {
        typedef nx::null_t result_t;
        typedef nx::null_t type_t;
    };

    /*
        Traits for normal function
    */
    template <typename R>
    struct detail<R(*)(), true>
    {
        typedef R result_t;
        typedef R type_t();
    };
#define NX_FUNC_TRAITS_(n) \
    template <typename R, NX_PP_TYPE_1(n, typename P)> \
    struct detail<R(*)(NX_PP_TYPE_1(n, P)), true> \
    { \
        typedef R result_t; \
        typedef R type_t(NX_PP_TYPE_1(n, P)); \
    };
    NX_PP_MULT_MAX(NX_FUNC_TRAITS_)
#undef NX_FUNC_TRAITS_

    /*
        Traits for member function
    */
#define NX_FUNC_MEM_(...) \
    template <typename R, typename C> \
    struct detail<NX_PP_VA(R(C::*)() __VA_ARGS__), true> \
    { \
        typedef R result_t; \
        typedef R type_t(); \
    };
    NX_FUNC_MEM_()
    NX_FUNC_MEM_(const)
    NX_FUNC_MEM_(volatile)
    NX_FUNC_MEM_(const volatile)
#undef NX_FUNC_MEM_
#define NX_FUNC_MEM_(n, ...) \
    template <typename R, typename C, NX_PP_TYPE_1(n, typename P)> \
    struct detail<NX_PP_VA(R(C::*)(NX_PP_TYPE_1(n, P)) __VA_ARGS__), true> \
    { \
        typedef R result_t; \
        typedef R type_t(NX_PP_TYPE_1(n, P)); \
    };
#define NX_FUNC_TRAITS_(n) \
    NX_FUNC_MEM_(n) \
    NX_FUNC_MEM_(n, const) \
    NX_FUNC_MEM_(n, volatile) \
    NX_FUNC_MEM_(n, const volatile)
    NX_PP_MULT_MAX(NX_FUNC_TRAITS_)
#undef NX_FUNC_TRAITS_
#undef NX_FUNC_MEM_

#if !(defined(NX_OS_WINCE) && (NX_CC_MSVC <= 1400))

    template <typename T>
    struct detail<T*, false> : detail<nx_typeof(&T::operator())>
    {};

#endif
}

template <typename T>
struct function_traits
    : private_function_traits::detail<typename nx::rm_cv
                                     <typename nx::select_if
                                     <nx::is_pointer<T>::value, T, T*
    >::type_t
    >::type_t
    >
{};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
