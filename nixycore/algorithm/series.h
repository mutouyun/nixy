/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/algorithm/foreach.h"

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <typename T>
struct series_base
{
    typedef T type_t;

    type_t val_;

    series_base(void) : val_(0) {}

    type_t value(void) const { return val_; }

    bool operator==(const series_base& y) const
    {
        return val_ == y.val_;
    }
};

namespace use // Series Policy
{
    /*
        Accumulation (N * (n - 1))

        -->
        0 : 0
        n : N + f(n - 1)
    */

    template <typename T, size_t N>
    struct iter_acc : series_base<T>
    {
        typedef series_base<T> base_t;

        void operator()(int n)
        {
            if (n == 0) return;
            if (n > 0)
                base_t::val_ += (N * n);
            else
                base_t::val_ -= (N * (-n));
        }
    };

    /*
        (N ^ (n - 1))

        -->
        0 : 0
        1 : 1
        n : N * f(n - 1)
    */

    template <typename T, size_t N>
    struct iter_powerof : series_base<T>
    {
        typedef series_base<T> base_t;

        void operator()(int n)
        {
            if (n == 0) return;
            if (n > 0)
            {
                if (base_t::val_ == 0)
                {
                    base_t::val_ = 1;
                    --n;
                }
                nx_foreach(NX_UNUSED i, n)
                {
                    base_t::val_ *= N;
                }
            }
            else
            {
                nx_foreach(NX_UNUSED i, n)
                {
                    // when val == 1, val / N == 0
                    base_t::val_ /= N;
                }
            }
        }
    };

    /*
        Fibonacci

        -->
        0 : 0
        1 : N
        n : f(n - 1) + f(n - 2)
    */

    template <typename T, size_t N = 1>
    struct iter_fibonacci : series_base<T>
    {
        typedef series_base<T> base_t;

        typename base_t::type_t prv_;
        iter_fibonacci(void) : prv_(0) {}

        void operator()(int n)
        {
            if (n == 0) return;
            if (n > 0) nx_foreach(NX_UNUSED i, n)
            {
                if (base_t::val_ == 0)
                {
                    base_t::val_ = N;
                    prv_ = 0;
                }
                else
                {
                    T tmp = base_t::val_;
                    base_t::val_ += prv_;
                    prv_ = tmp;
                }
            }
            else nx_foreach(NX_UNUSED i, n)
            {
                T tmp = prv_;
                prv_ = (base_t::val_ -= prv_);
                base_t::val_ = tmp;
            }
        }
    };
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
