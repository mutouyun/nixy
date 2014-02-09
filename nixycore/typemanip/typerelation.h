/*
    Modified from The Loki Library
    Modified by : 2013 darkcat

    Copyright (c) 2001 by Andrei Alexandrescu

    This code accompanies the book:
    Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design 
        Patterns Applied". Copyright (c) 2001. Addison-Wesley.
    Permission to use, copy, modify, distribute and sell this software for any 
        purpose is hereby granted without fee, provided that the above copyright 
        notice appear in all copies and that both that copyright notice and this 
        permission notice appear in supporting documentation.
    The author or Addison-Welsey Longman make no representations about the 
        suitability of this software for any purpose. It is provided "as is" 
        without express or implied warranty.
*/

#pragma once

#include "nixycore/typemanip/typedefs.h"
#include "nixycore/typemanip/typetools.h"

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    Check the same type
*/

template <typename T, typename U>
struct is_sametype
    : Judge<false>
{};

template <typename T>
struct is_sametype<T, T>
    : Judge<true>
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
    NX_STATIC_VALUE( bool, nx_rightof( check(T()) ) );
};

template <typename T>
struct is_convertible<T, T> : 
    Judge<true>
{};

/*
    Check the Super-Sub inheritance of two classes
*/

template <typename T, typename U>
struct is_supersub : 
    Judge<is_convertible<const U*, const T*>::value && 
         !is_sametype<const T*, const void*>::value && 
         !is_sametype<const T , const U    >::value>
{};

template <>
struct is_supersub<void, void> : 
    Judge<false>
{};

template <typename T>
struct is_supersub<T, void> : 
    Judge<false>
{};

template <typename U>
struct is_supersub<void, U> : 
    Judge<false>
{};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
