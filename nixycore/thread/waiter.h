/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/thread/lockguard.h"
#include "nixycore/thread/mutex.h"
#include "nixycore/thread/condition.h"

#include "nixycore/general/general.h"
#include "nixycore/utility/utility.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    waiter status enum
*/

enum waiter_status_t
{
    waiter_Resting,
    waiter_Excited,
    waiter_AutoRet
};

/*
    wait for something is happen
*/

class waiter : nx::noncopyable
{
    mutable mutex lock_;
    condition     cond_;

    waiter_status_t signaled_;

public:
    waiter(void)
        : cond_(lock_)
        , signaled_(waiter_Resting)
    {}

    waiter_status_t status(void) const
    {
        nx_lock_scope(lock_);
        return signaled_;
    }

    bool is_signaled(void) const
    {
        nx_lock_scope(lock_);
        return (signaled_ != waiter_Resting);
    }

    void reset(void)
    {
        nx_lock_scope(lock_);
        signaled_ = waiter_Resting;
    }

public:
    bool wait(int tm_ms = -1)
    {
        nx_lock_scope(lock_);
        while (signaled_ == waiter_Resting)
        {
            bool ret = cond_.wait(tm_ms);
            if (!ret) return false;
        }
        if (signaled_ == waiter_AutoRet)
            signaled_ =  waiter_Resting;
        return true;
    }

    void notify(void)
    {
        nx_lock_scope(lock_);
        signaled_ = waiter_AutoRet;
        cond_.broadcast();
    }

    void broadcast(void)
    {
        nx_lock_scope(lock_);
        signaled_ = waiter_Excited;
        cond_.broadcast();
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
