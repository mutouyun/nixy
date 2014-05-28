/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/typemanip/typetraits.h"

#include "nixycore/utility/forward.h"
#include "nixycore/utility/noncopyable.h" // for is_copyable

#include "nixycore/general/general.h"

#ifdef NX_SP_CXX11_TYPE_TRAITS
#include <type_traits>
#endif

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    Obtains the decay type of T
*/

#ifdef NX_SP_CXX11_TYPE_TRAITS
template <typename T>
struct decay
{
private:
    typedef typename extract<T>::type_t extract_type;

public:
    typedef typename std::decay<extract_type>::type type_t;
};
#else /*NX_SP_CXX11_TYPE_TRAITS*/
template <typename T>
struct decay
{
private:
    typedef typename extract<T>::type_t extract_type;

public:
    typedef typename select_if<is_array<extract_type>::value,
            typename rm_array<extract_type>::type_t*,
            typename select_if<is_function<extract_type>::value && !is_pointer<extract_type>::value,
                extract_type*, 
                extract_type
            >::type_t
    >::type_t type_t;
};
#endif/*NX_SP_CXX11_TYPE_TRAITS*/

/*
    The type_t is enabled as T is the same type as U.
*/

template <typename T, typename U, typename TypeT>
struct enable_if_same
    : nx::enable_if<nx::is_same<
                        typename nx::decay<T>::type_t,
                        typename nx::decay<U>::type_t
                    >::value, TypeT>
{};

/*
    The type_t is enabled as T is the different type as U.
*/

template <typename T, typename U, typename TypeT>
struct enable_if_diff
    : nx::enable_if<!nx::is_same<
                        typename nx::decay<T>::type_t,
                        typename nx::decay<U>::type_t
                    >::value, TypeT>
{};

/*
    detect copyable
*/

#if defined(NX_SP_CXX11_TYPE_TRAITS) && \
    /*
        <MSVC 2013> std::is_copy_constructible doesn't work correctly.
        See: http://connect.microsoft.com/VisualStudio/feedback/details/799732/
             http://connect.microsoft.com/VisualStudio/feedback/details/800328/
             http://connect.microsoft.com/VisualStudio/feedback/details/802032/
    */ \
    !(defined(NX_CC_MSVC) && (NX_CC_MSVC <= 1800)) && \
    /*
        <GCC> 4.7 and later support -std=c++11 and -std=gnu++11 as well.
        See: http://gcc.gnu.org/projects/cxx0x.html
    */ \
    (!defined(NX_CC_GCC) || NX_CHECK_GNUC(4, 7, 0))
template <typename T>
struct is_copyable
    : type_if<std::is_copy_constructible<T>::value>
{};
#else /*NX_SP_CXX11_TYPE_TRAITS*/
template <typename T>
struct is_copyable
    : type_if<!nx::is_supersub<nx::noncopyable, typename nx::decay<T>::type_t>::value>
{};
#endif/*NX_SP_CXX11_TYPE_TRAITS*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
