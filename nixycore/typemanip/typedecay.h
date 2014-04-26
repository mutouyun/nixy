/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/typemanip/typetraits.h"

#include "nixycore/utility/rvalue.h"

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

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
