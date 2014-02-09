/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/typemanip/typedefs.h"

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    Standard random policy
*/

struct by_rand_std
{
    typedef int rand_t;

    NX_STATIC_PROPERTY(rand_t, MAX, RAND_MAX);

    void srand(nx::uint sd)
    {
        std::srand(sd);
    }

    rand_t rand(void)
    {
        return std::rand();
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
