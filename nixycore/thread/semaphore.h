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

class semaphore : nx::NonCopyable
{
protected:
    mutable mutex lock_;
    condition     cond_;
    long          counter_;

public:
    semaphore(long init_count = 0)
        : cond_(lock_)
        , counter_(init_count)
    {}

    long count(void) const
    {
        nx_lock_scope(lock_);
        return counter_;
    }

public:
    bool wait(int tm_ms = -1)
    {
        nx_lock_scope(lock_);
        while (counter_ < 1)
        {
            bool ret = cond_.wait(tm_ms);
            if (!ret) return false;
        }
        -- counter_;
        return true;
    }

    void post(long count = 1)
    {
        nx_lock_scope(lock_);
        counter_ += count;
        cond_.broadcast();
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
