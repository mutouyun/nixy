/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "utility/refer.h"

#include "general/general.h"
#include "typemanip/typemanip.h"
#include "algorithm/algorithm.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    rvalue
    -->
    MyClass::MyClass(const nx::rvalue<MyClass>& rv);
    MyClass& MyClass::operator=(const nx::rvalue<MyClass>& rv);
*/

template <typename T, bool = is_class<T>::value>
class rvalue : public nx::traits<T>::type_t
{
    typedef typename nx::traits<T>::type_t base_t;
    typedef base_t value_t;

public:
    rvalue(const rvalue& rv)
        : base_t(rv)
    {}
    rvalue(const value_t& rv)
        : base_t(static_cast<rvalue&>(const_cast<value_t&>(rv)))
    {}
};

template <typename T>
class rvalue<T, false> : public refer<T>
{
    typedef refer<T> base_t;
    typedef typename base_t::value_t value_t;

public:
    rvalue(const rvalue& rv)
        : base_t(rv)
    {}
    rvalue(const value_t& rv)
        : base_t(const_cast<value_t&>(rv))
    {}
};

/*
    Move
*/

template <typename T>
inline typename enable_if<is_class<T>::value, 
const nx::rvalue<T>&>::type_t move(const T& rv)
{
    return static_cast<const nx::rvalue<T>&>(rv);
}

template <typename T>
inline typename enable_if<!is_class<T>::value, 
const T&>::type_t move(const T& rv)
{
    return rv;
}

template <typename T>
inline T& unmove(const rvalue<T>& rv)
{
    return const_cast<T&>(static_cast<const T&>(rv));
}

/*
    Special swap algorithm
*/

template <typename T>
inline void swap(T& x, const nx::rvalue<T>& y)
{
    nx::swap(x, unmove(y));
}

template <typename T>
inline void swap(const nx::rvalue<T>& x, T& y)
{
    nx::swap(unmove(x), y);
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
