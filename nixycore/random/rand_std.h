/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#pragma once

#include "nixycore/typemanip/typedefs.h"

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG namespace use {
//////////////////////////////////////////////////////////////////////////

/*
    Standard random policy
*/

struct rand_std
{
    typedef int rand_t;

    NX_STATIC_PROPERTY(rand_t, MAX, RAND_MAX);

    void srand(nx::uint sd)
    {
        ::srand(sd);
    }

    rand_t rand(void)
    {
        return ::rand();
    }
};

//////////////////////////////////////////////////////////////////////////
} NX_END
//////////////////////////////////////////////////////////////////////////
