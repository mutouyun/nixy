/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "thread/lockguard.h"
#include "thread/mutex.h"
#include "thread/condition.h"

#include "general/general.h"
#include "utility/utility.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

class waiter : nx::NonCopyable
{
    mutable mutex lock_;
    condition     cond_;

    enum
    {
        RESTING, 
        EXCITED, 
        AUTORET
    } signaled_;

public:
    waiter(void)
        : cond_(lock_)
        , signaled_(RESTING)
    {}

    bool is_signaled(void) const
    {
        nx_lock_scope(lock_);
        return (signaled_ != RESTING);
    }

    void reset(void)
    {
        nx_lock_scope(lock_);
        signaled_ = RESTING;
    }

public:
    bool wait(int tm_ms = -1)
    {
        nx_lock_scope(lock_);
        while (signaled_ == RESTING)
        {
            bool ret = cond_.wait(tm_ms);
            if (!ret) return false;
        }
        if (signaled_ == AUTORET) signaled_ = RESTING;
        return true;
    }

    void notify(void)
    {
        nx_lock_scope(lock_);
        signaled_ = AUTORET;
        cond_.broadcast();
    }

    void broadcast(void)
    {
        nx_lock_scope(lock_);
        signaled_ = EXCITED;
        cond_.broadcast();
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
