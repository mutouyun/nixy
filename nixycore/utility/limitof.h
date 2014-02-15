/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/typemanip/typedefs.h"
#include "nixycore/typemanip/typedetect.h"

#include "nixycore/general/general.h"

// numeric limits
#include <limits.h>

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <typename T, bool IsUnsigned_ = is_unsigned<T>::value
                    , bool IsSigned_   = is_signed<T>::value>
struct limit_of;

template <typename T>
struct limit_of<T, true, false>
{
    NX_STATIC_PROPERTY(T, lower, 0);
    NX_STATIC_PROPERTY(T, upper, (T)~0);
};

template <typename T>
struct limit_of<T, false, true>
{
    NX_STATIC_PROPERTY(T, lower, ((T)1) << ((sizeof(T) * CHAR_BIT) - 1) );
    NX_STATIC_PROPERTY(T, upper, ~lower);
};

template <>
struct limit_of<bool, false, false>
{
    NX_STATIC_PROPERTY(bool, lower, false);
    NX_STATIC_PROPERTY(bool, upper, true);
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
