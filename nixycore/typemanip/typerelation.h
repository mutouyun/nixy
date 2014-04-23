/*
    The Nixy Library
    Code covered by the MIT License

    Modified from The Loki Library
    Modified by : mutouyun (http://darkc.at)

    Copyright (c) 2001 by Andrei Alexandrescu
*/

#pragma once

#include "nixycore/typemanip/typedefs.h"
#include "nixycore/typemanip/typetools.h"
#include "nixycore/typemanip/typebehavior.h"

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    Check the same type
*/

template <typename T, typename U>
struct is_same
    : type_if<false>
{};

template <typename T>
struct is_same<T, T>
    : type_if<true>
{};

/*
    Check the conversion of two classes
*/

template <typename T, typename U>
struct is_convertible
{
private:
    static yes_t check(U);
    static not_t check(...);

public:
    NX_STATIC_VALUE( bool, nx_judge( check(T()) ) );
};

template <typename T>
struct is_convertible<T, T> : 
    type_if<true>
{};

/*
    Check the Super-Sub inheritance of two classes
*/

namespace private_is_supersub
{
    template <typename T, typename U>
    struct detail : 
        type_if<is_convertible<const U*, const T*>::value &&
               !is_same<const T*, const void*>::value &&
               !is_same<const T , const U    >::value>
    {};

    template <>
    struct detail<void, void> : 
        type_if<false>
    {};

    template <typename T>
    struct detail<T, void> : 
        type_if<false>
    {};

    template <typename U>
    struct detail<void, U> : 
        type_if<false>
    {};
}

template <typename T, typename U>
struct is_supersub
    : private_is_supersub::detail
    <typename rm_reference<typename rm_cv<T>::type_t>::type_t,
     typename rm_reference<typename rm_cv<U>::type_t>::type_t>
{};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
