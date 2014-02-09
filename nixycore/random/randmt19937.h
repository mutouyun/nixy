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
    Mersenne Twister 19937 random policy
*/

struct by_rand_mt19937
{
    typedef nx::uint rand_t;

    static const int    word_size       = sizeof(rand_t) * CHAR_BIT;
    static const int    state_size      = 624;
    static const int    shift_size      = 397;
    static const int    mask_bits       = 31;
    static const rand_t parameter_a     = 0x9908b0df;
    static const int    output_u        = 11;
    static const int    output_s        = 7;
    static const rand_t output_b        = 0x9d2c5680;
    static const int    output_t        = 15;
    static const rand_t output_c        = 0xefc60000;
    static const int    output_l        = 18;
    static const rand_t init_multiplier = 0x6c078965;

    static const rand_t W_MSK = ~((~rand_t(0) << (word_size - 1)) << 1);
    static const rand_t H_MSK = (W_MSK << mask_bits) & W_MSK;
    static const rand_t L_MSK = ~H_MSK & W_MSK;

    NX_STATIC_PROPERTY(rand_t, MAX, W_MSK);

protected:
    int index_;
    rand_t MT_[state_size * 2];

    void refill_lower()
    {
        // compute values for the lower half of the history array
        int i = 0;
        for(; i < state_size - shift_size; ++i)
        {
            // fill in lower region
            rand_t tmp = (MT_[i + state_size] & H_MSK) | (MT_[i + state_size + 1] & L_MSK);
            MT_[i] = (tmp >> 1) ^ (tmp & 1 ? parameter_a : 0) ^ MT_[i + state_size + shift_size];
        }
        for(; i < state_size - 1; ++i)
        {
            // fill in upper region (avoids modulus operation)
            rand_t tmp = (MT_[i + state_size] & H_MSK) | (MT_[i + state_size + 1] & L_MSK);
            MT_[i] = (tmp >> 1) ^ (tmp & 1 ? parameter_a : 0) ^ MT_[i - state_size + shift_size];
        }
        rand_t tmp = (MT_[i + state_size] & H_MSK) | (MT_[0] & L_MSK);
        MT_[i] = (tmp >> 1) ^ (tmp & 1 ? parameter_a : 0) ^ MT_[shift_size - 1];
        index_ = 0;
    }

    void refill_upper()
    {
        // compute values for the upper half of the history array
        for(int i = state_size; i < state_size * 2; ++i)
        {
            // fill in values
            rand_t tmp = (MT_[i - state_size] & H_MSK) | (MT_[i - state_size + 1] & L_MSK);
            MT_[i] = (tmp >> 1) ^ (tmp & 1 ? parameter_a : 0) ^ MT_[i - state_size + shift_size];
        }
    }

public:
    void srand(nx::uint sd)
    {
        rand_t prev = MT_[0] = sd & W_MSK;
        for(int i = 1; i < state_size; ++i)
            prev = MT_[i] = (i + init_multiplier * (prev ^ (prev >> (word_size - 2)))) & W_MSK;
        index_ = state_size;
    }

    rand_t rand(void)
    {
        if (index_ == state_size)
            refill_upper();
        else
        if (index_ >= state_size * 2)
            refill_lower();
        rand_t y = MT_[index_++] & W_MSK;
        y ^= (y >> output_u) & W_MSK;
        y ^= (y << output_s) & output_b;
        y ^= (y << output_t) & output_c;
        y ^= (y & W_MSK) >> output_l;
        return y;
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
