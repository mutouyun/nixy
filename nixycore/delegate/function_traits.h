/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/preprocessor/preprocessor.h"

#ifdef NX_SP_CXX11_RESULT_OF
#ifdef NX_CC_MSVC
/*
    <MSVC> The result_of class is in <functional>
    See: http://msdn.microsoft.com/en-us/library/bb982028(v=vs.120).aspx
*/
#include <functional>
#else /*NX_CC_MSVC*/
#include <type_traits>
#endif/*NX_CC_MSVC*/
#endif

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

namespace private_function_traits
{
    template <typename FuncT, bool = nx::is_function<FuncT>::value>
    struct detail
    /* Circumvent MSVC's ETI errors */
    {
        typedef nx::null_t result_t;
        typedef nx::null_t type_t;
    };

    /*
        Traits for normal function
    */
#ifdef NX_SP_CXX11_TEMPLATES
    template <typename R, typename... P>
    struct detail<R(*)(P...), true>
    {
        typedef R result_t;
        typedef R type_t(P...);
    };
#else /*NX_SP_CXX11_TEMPLATES*/
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
#endif/*NX_SP_CXX11_TEMPLATES*/

    /*
        Traits for member function
    */
#ifdef NX_SP_CXX11_TEMPLATES
#define NX_FUNC_MEM_(...) \
    template <typename R, typename C, typename... P> \
    struct detail<R(C::*)(P...) __VA_ARGS__, true> \
    { \
        typedef R result_t; \
        typedef R type_t(P...); \
    };
    NX_FUNC_MEM_()
    NX_FUNC_MEM_(const)
    NX_FUNC_MEM_(volatile)
    NX_FUNC_MEM_(const volatile)
#undef NX_FUNC_MEM_
#else /*NX_SP_CXX11_TEMPLATES*/
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
#endif/*NX_SP_CXX11_TEMPLATES*/

#if !(defined(NX_OS_WINCE) && (NX_CC_MSVC <= 1400))

    template <typename T>
    struct detail<T*, false> : detail<nx_typeof(&T::operator())>
    {};

#endif

    template <typename T>
    struct adjust
    {
    private:
        typedef typename nx::decay<T>::type_t clear_type;

    public:
        typedef typename nx::select_if<
                nx::is_pointer<clear_type>::value,
                    clear_type, clear_type*
        >::type_t type_t;
    };
}

template <typename T>
struct function_traits
    : private_function_traits::detail<typename private_function_traits::adjust<T>::type_t>
{};

/*
    Obtains the result type of a call
*/

#ifdef NX_SP_CXX11_RESULT_OF
template <typename F>
struct result_of
{
    typedef typename std::result_of<F>::type type_t;
};
#else /*NX_SP_CXX11_RESULT_OF*/
template <typename F>
struct result_of;

template <typename F>
struct result_of<F()>
{
    typedef typename function_traits<F>::result_t type_t;
};
#define NX_FUNC_TRAITS_(n) \
template <typename F, NX_PP_TYPE_1(n, typename P)> \
struct result_of<F(NX_PP_TYPE_1(n, P))> \
{ \
    typedef typename function_traits<F>::result_t type_t; \
};
NX_PP_MULT_MAX(NX_FUNC_TRAITS_)
#undef NX_FUNC_TRAITS_
#endif/*NX_SP_CXX11_RESULT_OF*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
