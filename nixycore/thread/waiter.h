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

class waiter : nx::NonCopyable
{
    mutable mutex lock_;
    condition     cond_;

    enum
    {
        Resting,
        Excited,
        AutoRet
    } signaled_;

public:
    waiter(void)
        : cond_(lock_)
        , signaled_(Resting)
    {}

    bool is_signaled(void) const
    {
        nx_lock_scope(lock_);
        return (signaled_ != Resting);
    }

    void reset(void)
    {
        nx_lock_scope(lock_);
        signaled_ = Resting;
    }

public:
    bool wait(int tm_ms = -1)
    {
        nx_lock_scope(lock_);
        while (signaled_ == Resting)
        {
            bool ret = cond_.wait(tm_ms);
            if (!ret) return false;
        }
        if (signaled_ == AutoRet) signaled_ = Resting;
        return true;
    }

    void notify(void)
    {
        nx_lock_scope(lock_);
        signaled_ = AutoRet;
        cond_.broadcast();
    }

    void broadcast(void)
    {
        nx_lock_scope(lock_);
        signaled_ = Excited;
        cond_.broadcast();
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
