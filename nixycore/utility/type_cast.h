/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/typemanip/typetools.h"

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <typename T>
inline T implicit_cast(typename type_wrap<T>::type_t input)
{
    return input;
}

template <typename T, typename U>
inline T horrible_cast(const U& input)
{
    union { T out; U inp; } horrible;
    horrible.inp = input;
    return horrible.out;
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
