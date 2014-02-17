/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/typemanip/typedetect.h"

#include "nixycore/general/general.h"

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
class rvalue<T, false>
{
    typedef typename rm_const<
            typename rm_reference<T>::type_t
                                   >::type_t value_t;

    value_t content_;

public:
    rvalue(const rvalue& rv)
        : content_(rv.content_)
    {}
    rvalue(const value_t& rv)
        : content_(rv)
    {}

    operator value_t() const { return content_; }
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
inline typename enable_if<is_class<T>::value,
T&>::type_t unmove(const rvalue<T>& rv)
{
    return const_cast<T&>(static_cast<const T&>(rv));
}

template <typename T>
inline typename enable_if<!is_class<T>::value,
T>::type_t unmove(const rvalue<T>& rv)
{
    return rv;
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
