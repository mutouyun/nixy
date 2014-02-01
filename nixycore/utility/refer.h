/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "utility/addressof.h"

#include "general/general.h"
#include "typemanip/typemanip.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <typename T>
class refer
{
public:
    typedef typename rm_const
                <typename rm_reference
                    <T
                    >::type_t
                >::type_t type_t;
    typedef typename rm_reference
                <T
                >::type_t value_t;

protected:
    value_t* content_;

public:
    refer(value_t& r)
        : content_(nx::addressof(r))
    {}

    operator value_t& () const { return *content_; }

    value_t&      operator* (void)       { return *content_; }
    const type_t& operator* (void) const { return *content_; }
    value_t*      operator& (void)       { return content_; }
    const type_t* operator& (void) const { return content_; }
    value_t*      operator->(void)       { return content_; }
    const type_t* operator->(void) const { return content_; }
};

/*
    For get a reference of a variable
*/

template <typename T>
inline refer<T>        ref(T& r)              { return refer<T>(r); }
template <typename T>
inline refer<const T>  ref(const T& r)        { return refer<const T>(r); }
template <typename T>
inline refer<T>&       ref(refer<T>& r)       { return r; }
template <typename T>
inline const refer<T>& ref(const refer<T>& r) { return r; }

template <typename T>
inline T&                               unref(T& r)              { return r; }
template <typename T>
inline const T&                         unref(const T& r)        { return r; }
template <typename T>
inline typename refer<T>::value_t&      unref(refer<T>& r)       { return (*r); }
template <typename T>
inline const typename refer<T>::type_t& unref(const refer<T>& r) { return (*r); }

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
