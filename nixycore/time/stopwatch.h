/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/time/tickcount.h"

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

namespace use
{
    struct clock_std // Standard clock policy
    {
        typedef ulong clock_t;
        clock_t clock(void) { return tickcount(); }
        static double second(const clock_t& cl) { return double(cl) / double(1000); }
    };
}

//////////////////////////////////////////////////////////////////////////

#ifndef NX_DEFAULT_CLOCK
#define NX_DEFAULT_CLOCK nx::use::clock_std
#endif

template <typename ModelT = NX_DEFAULT_CLOCK>
class stopwatch : public ModelT
{
public:
    typedef typename ModelT::clock_t clock_t;

protected:
    clock_t start_time_;
    clock_t pause_elap_;
    bool is_stopped_;

public:
    stopwatch(bool start_watch = false)
        : ModelT()
        , start_time_(0)
        , pause_elap_(0)
        , is_stopped_(true)
    {
        if (start_watch) start();
    }

public:
    bool check(void)
    {
        return (start_time_ <= ModelT::clock());
    }

    double value(void)
    {
        return ModelT::second(elapsed());
    }

    clock_t elapsed(void)
    {
        if (isStopped())
            return 0;
        else if (isPaused())
            return pause_elap_;
        else
        {
            clock_t now = ModelT::clock();
            if (start_time_ > now)
            {
                stop();
                return 0;
            }
            return now - start_time_;
        }
    }

    void start(void)
    {
        elapsed(); // if (start_time_ > now), stopwatch will restart
        start_time_ = ModelT::clock();
        start_time_ -= pause_elap_;
        pause_elap_ = 0;
        is_stopped_ = false;
    }

    void pause(void)
    {
        pause_elap_ = elapsed();
    }

    void stop(void)
    {
        start_time_ = 0;
        pause_elap_ = 0;
        is_stopped_ = true;
    }

    bool isStopped(void)
    {
        return is_stopped_;
    }

    bool isPaused(void)
    {
        return (pause_elap_ != 0);
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
