/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#include "time/tickcount.h"

#include <pthread.h>

//////////////////////////////////////////////////////////////////////////
NX_BEG namespace private_condition {
//////////////////////////////////////////////////////////////////////////

class detail
{
    pthread_cond_t cond_;
    mutex&         cond_mutex_;

public:
    detail(mutex& mx)
        : cond_mutex_(mx)
    {
        pthread_condattr_t attr;
        if (pthread_condattr_init(&attr) != 0) return;
        pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
        pthread_cond_init(&cond_, &attr);
        pthread_condattr_destroy(&attr);
    }

    ~detail()
    {
        broadcast();
        pthread_cond_destroy(&cond_);
    }

public:
    bool wait(int tm_ms = -1)
    {
        int err;
        if (tm_ms < 0)
        {
            err = pthread_cond_wait(&cond_, &(*cond_mutex_));
        }
        else
        {
            ulong tc = tickcount() + tm_ms;
            struct timespec tv;
            tv.tv_sec  = (tc / 1000);
            tv.tv_nsec = (tc % 1000) * 1000000;
            // returns true if signal comes, false otherwise.
            err = pthread_cond_timedwait(&cond_, &(*cond_mutex_), &tv);
        }
        return (err == 0);
    }

    void notify(void)
    {
        pthread_cond_signal(&cond_);
    }

    void broadcast(void)
    {
        pthread_cond_broadcast(&cond_);
    }
};

//////////////////////////////////////////////////////////////////////////
} NX_END
//////////////////////////////////////////////////////////////////////////
