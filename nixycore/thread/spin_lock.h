/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/thread/atomic.h"
#include "nixycore/thread/thread_ops.h"

#include "nixycore/bugfix/assert.h"

#include "nixycore/general/general.h"
#include "nixycore/utility/utility.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

class spin_lock : nx::noncopyable
{
protected:
    long rc_;
    nx::atomic<thread_ops::id_t> id_;

public:
    typedef spin_lock lock_t;

    spin_lock(void) : rc_(0), id_(0) {}
    ~spin_lock() { nx_assert(rc_ <= 0); }

    typedef lock_t handle_t;
    handle_t&       operator*(void)       { return (*this); }
    const handle_t& operator*(void) const { return (*this); }

public:
    bool trylock(void)
    {
        thread_ops::id_t tid = thread_ops::current_id();
        if (id_ == tid)
        {
            ++rc_;
            return true;
        }
        else if (id_.cas(0, tid))
        {
            rc_ = 1;
            return true;
        }
        return false;
    }

    void lock(void)
    {
        thread_ops::id_t tid = thread_ops::current_id();
        if (id_ == tid)
            ++rc_;
        else
        {
            for (unsigned k = 0; !id_.cas(0, tid); ++k)
                thread_ops::pause(k);
            rc_ = 1;
        }
    }

    void unlock(void)
    {
        if (id_ == thread_ops::current_id())
            if (--rc_ <= 0)
            {
                rc_ = 0;
                id_ = 0;
            }
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
