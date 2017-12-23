/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#pragma once

#include "nixycore/typemanip/typeconcept.h"
#include "nixycore/typemanip/typedetect.h"

#include "nixycore/general/general.h"

// placement new
#include <new>

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

NX_CONCEPT(initialize, initialize, void(C::*)(void))

template <typename T>
inline typename enable_if<!has_initialize<T>::value &&
                          !is_pointer<T>::value &&
                           is_numeric<T>::value
>::type_t initialize(T& d)
{
    d = 0;
}

template <typename T>
inline typename enable_if<!has_initialize<T>::value &&
                          !is_pointer<T>::value &&
                          !is_numeric<T>::value &&
                           is_pod<T>::value
>::type_t initialize(T& d)
{
    memset(&d, 0, sizeof(d));
}

template <typename T>
inline typename enable_if<!has_initialize<T>::value &&
                          !is_pointer<T>::value &&
                          !is_numeric<T>::value &&
                          !is_pod<T>::value
>::type_t initialize(T& d)
{
    ::new (&d) T;
}

template <typename T>
inline typename enable_if<has_initialize<T>::value
>::type_t initialize(T& d)
{
    d.initialize();
}

template <typename T, size_t N>
inline void initialize(T(& d)[N])
{
    for(size_t i = 0; i < N; ++i)
        initialize(d[i]);
}

template <typename T>
inline typename enable_if<is_pointer<T>::value, 
T>::type_t initialize(T d)
{
    initialize(*d);
    return d;
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
