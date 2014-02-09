/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/random/randstd.h"
#include "nixycore/random/randmt19937.h"

#include "nixycore/time/tickcount.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

#ifndef NX_DEFAULT_RAND
#define NX_DEFAULT_RAND nx::by_rand_mt19937
#endif

template <typename Model_ = NX_DEFAULT_RAND>
class random : public Model_
{
protected:
    int lower_, upper_;

    double roll(void)
    {
        return (Model_::rand() + 0.5) / (Model_::MAX + 1.0);
    }

public:
    random(int lower = 0, int upper = Model_::MAX)
        : lower_(0), upper_(0)
    {
        range(lower, upper);
        srand();
    }

    void range(int lower = 0, int upper = Model_::MAX)
    {
        lower_ = lower;
        upper_ = upper;
    }

    void srand(nx::uint seed = 0)
    {
        Model_::srand(seed ? seed : static_cast<uint>(tickcount()));
    }

    template <typename T>
    T roll(int lower, int upper)
    {
        return static_cast<T>( lower + (upper - lower) * roll() );
    }

    template <typename T>
    T roll(void)
    {
        return roll<T>(lower_, upper_);
    }

    template <typename T>
    T roll(T& r_, int lower, int upper)
    {
        return r_ = roll<T>(lower, upper);
    }

    template <typename T>
    T roll(T& r_)
    {
        return r_ = roll<T>();
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
