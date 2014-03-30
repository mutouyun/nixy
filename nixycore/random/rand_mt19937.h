/*
    The Nixy Library
    Code covered by the MIT License

    Modified from http://en.wikipedia.org/wiki/Mersenne_twister
    Modified by : mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/typemanip/typedefs.h"
#include "nixycore/bugfix/assert.h"

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG namespace use {
//////////////////////////////////////////////////////////////////////////

/*
    Mersenne Twister 19937 random policy
*/

struct rand_mt19937
{
    typedef nx::uint32 rand_t;

    NX_STATIC_PROPERTY(rand_t, MAX, (rand_t)~0);

    NX_STATIC_PROPERTY(int   , WORD_SIZE      , sizeof(rand_t) * CHAR_BIT);        // word size (in number of bits)
    NX_STATIC_PROPERTY(int   , MASK_BITS      , WORD_SIZE - 1);                    // separation point of one word, or the number of bits of the lower bitmask
    NX_STATIC_PROPERTY(rand_t, F_MASK         , (rand_t)~0);                       // 0xffffffff
    NX_STATIC_PROPERTY(rand_t, H_MASK         , (F_MASK << MASK_BITS) & F_MASK);   // 0x80000000
    NX_STATIC_PROPERTY(rand_t, L_MASK         , ~H_MASK & F_MASK);                 // 0x7fffffff

    NX_STATIC_PROPERTY(int   , STATE_SIZE     , 624);                              // degree of recurrence
    NX_STATIC_PROPERTY(int   , MID_SIZE       , 397);                              // middle word, or the number of parallel sequences
    NX_STATIC_PROPERTY(int   , MOD_SIZE       , STATE_SIZE - MID_SIZE);

    NX_STATIC_PROPERTY(rand_t, INIT_MULTIPLIER, 0x6c078965);
    NX_STATIC_PROPERTY(rand_t, PARAMETER_A    , 0x9908b0df);                       // coefficients of the rational normal form twist matrix
    NX_STATIC_PROPERTY(rand_t, OUTPUT_B       , 0x9d2c5680);                       // TGFSR(R) tempering bitmasks
    NX_STATIC_PROPERTY(rand_t, OUTPUT_C       , 0xefc60000);
    NX_STATIC_PROPERTY(rand_t, OUTPUT_S       , 7);                                // TGFSR(R) tempering bit shifts
    NX_STATIC_PROPERTY(rand_t, OUTPUT_T       , 15);
    NX_STATIC_PROPERTY(rand_t, OUTPUT_U       , 11);                               // additional Mersenne Twister tempering bit shifts
    NX_STATIC_PROPERTY(rand_t, OUTPUT_L       , 18);

private:
    rand_t MT_[STATE_SIZE]; // Create a length 624 array to store the state of the generator
    int index_;

    /*
        Generate an array of 624 untempered numbers
    */
    void generate_numbers(void)
    {
        /* Fill in lower region */
        int i = 0;
        for(; i < STATE_SIZE - MID_SIZE; ++i)
        {
            rand_t y = (MT_[i]     & H_MASK) |  // bit 31 (32nd bit) of MT_[i]
                       (MT_[i + 1] & L_MASK);   // bits 0-30 (first 31 bits) of MT_[...]
            MT_[i] = MT_[i + MID_SIZE] ^ (y >> 1) ^ (y & 1 /* y is odd */ ? PARAMETER_A : 0);
        }
        /* Fill in upper region (avoids mod) */
        int n = 0;
        for(; i < STATE_SIZE - 1; ++i, ++n)
        {
            rand_t y = (MT_[i] & H_MASK) |      // bit 31 (32nd bit) of MT_[i]
                       (MT_[i + 1] & L_MASK);   // bits 0-30 (first 31 bits) of MT_[...]
            MT_[i] = MT_[n] ^ (y >> 1) ^ (y & 1 /* y is odd */ ? PARAMETER_A : 0);
        }
        /* The last round (avoids mod), now i == 623, n == 397 - 1 */
        {
            rand_t y = (MT_[i] & H_MASK) |      // bit 31 (32nd bit) of MT_[i]
                       (MT_[0] & L_MASK);       // bits 0-30 (first 31 bits) of MT_[...]
            MT_[i] = MT_[n] ^ (y >> 1) ^ (y & 1 /* y is odd */ ? PARAMETER_A : 0);
        }
    }

public:
    /*
        Initialize the generator from a seed
    */
    void srand(nx::uint sd)
    {
        rand_t prev = MT_[0] = sd;
        for(int i = 1; i < STATE_SIZE; ++i) // loop over each other element
            prev = MT_[i] = ( (i + INIT_MULTIPLIER * (prev ^ (prev >> 30))) & F_MASK );
        index_ = 0;
    }

    /*
        Extract a tempered pseudorandom number based on the index-th value,
        calling generate_numbers() every 624 numbers
    */
    rand_t rand(void)
    {
        nx_assert(index_ < STATE_SIZE);
        if (index_ == 0) generate_numbers();

        rand_t y = MT_[index_++];
        y ^= (y >> OUTPUT_U);
        y ^= (y << OUTPUT_S) & OUTPUT_B;
        y ^= (y << OUTPUT_T) & OUTPUT_C;
        y ^= (y >> OUTPUT_L);

        if (index_ >= STATE_SIZE) index_ = 0;
        return y;
    }
};

//////////////////////////////////////////////////////////////////////////
} NX_END
//////////////////////////////////////////////////////////////////////////
