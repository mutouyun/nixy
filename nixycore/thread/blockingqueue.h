/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "thread/lockguard.h"
#include "thread/mutex.h"
#include "thread/condition.h"

#include "container/deque.h"

#include "bugfix/assert.h"

#include "general/general.h"
#include "utility/utility.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <typename Type_, class Alloc_ = NX_DEFAULT_ALLOC>
class blocking_queue : NonCopyable
{
    mutable mutex        lock_;
    condition            until_empty_;
    deque<Type_, Alloc_> queue_;

public:
    typedef Type_ type_t;

    blocking_queue(void)
        : until_empty_(lock_)
    {}

    void put(const type_t& v)
    {
        nx_lock_scope(lock_);
        queue_.push_back(v);
        until_empty_.notify();
    }

    rvalue<type_t> take(void)
    {
        nx_lock_scope(lock_);
        while (queue_.empty()) // used to avoid spurious wakeups
            until_empty_.wait();
        nx_assert(!queue_.empty());
        type_t ret(nx::move(queue_.front()));
        queue_.pop_front();
        return ret;
    }

    size_t size(void) const
    {
        nx_lock_scope(lock_);
        return queue_.size();
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
