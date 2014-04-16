/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/thread/lock_guard.h"
#include "nixycore/thread/mutex.h"
#include "nixycore/thread/condition.h"

#include "nixycore/container/deque.h"

#include "nixycore/bugfix/assert.h"

#include "nixycore/general/general.h"
#include "nixycore/utility/utility.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <typename T>
class blocking_queue : noncopyable
{
    mutable mutex lock_;
    condition     task_coming_;
    condition     until_empty_;
    nx::deque<T>  queue_;

public:
    typedef T type_t;

    blocking_queue(void)
        : task_coming_(lock_)
        , until_empty_(lock_)
    {}

    void put(const type_t& v)
    {
        nx_lock_scope(lock_);
        queue_.push_back(v);
        task_coming_.notify();
    }

    nx_rval(type_t) take(void)
    {
        nx_lock_scope(lock_);
        while (queue_.empty()) // used to avoid spurious wakeups
            task_coming_.wait();
        nx_assert(!queue_.empty());
        type_t ret(nx::move(queue_.front()));
        queue_.pop_front();
        if (queue_.empty())
            until_empty_.notify();
        return nx::move(ret);
    }

    bool wait_empty(int tm_ms = -1)
    {
        nx_lock_scope(lock_);
        while (!queue_.empty()) // used to avoid spurious wakeups
        {
            bool ret = until_empty_.wait(tm_ms);
            if (!ret) return false;
        }
        return true;
    }

    size_t size(void) const
    {
        nx_lock_scope(lock_);
        return queue_.size();
    }

    bool is_empty(void) const
    {
        nx_lock_scope(lock_);
        return queue_.empty();
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
