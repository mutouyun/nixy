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

#ifdef NX_SP_CXX11_ATOMIC
#include <atomic> // std::atomic ...
#endif

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

class spin_lock : nx::noncopyable
{
protected:
#ifdef NX_SP_CXX11_ATOMIC
    std::atomic_flag lc_;
#else
    nx::atomic<bool> lc_;
#endif

public:
    typedef spin_lock lock_t;
    typedef lock_t handle_t;

#ifdef NX_SP_CXX11_ATOMIC
#   if defined(NX_CC_MSVC) && (NX_CC_MSVC <= 1800)
    /*
        <MSVC 2013> Atomic flag cannot be initialized in a constructor initializer.
        See: http://connect.microsoft.com/VisualStudio/feedback/details/800243/
    */
    spin_lock(void)
    {
        lc_.clear(std::memory_order_relaxed);
    }
#   else
    spin_lock(void)
        : lc_ ATOMIC_FLAG_INIT
    {}
#   endif
    ~spin_lock(void)
    {
        nx_assert(try_lock());
    }
#else
    ~spin_lock(void)
    {
        nx_assert(!lc_.load(memory_order::relaxed));
    }
#endif

    handle_t&       operator*(void)       { return (*this); }
    const handle_t& operator*(void) const { return (*this); }

public:
#ifdef NX_SP_CXX11_ATOMIC
    bool try_lock(void)
    {
        return !lc_.test_and_set(std::memory_order_acquire);
    }

    void lock(void)
    {
        for(unsigned k = 0; lc_.test_and_set(std::memory_order_acquire); ++k)
            thread_ops::pause(k);
    }

    void unlock(void)
    {
        lc_.clear(std::memory_order_release);
    }
#else
    bool try_lock(void)
    {
        return !lc_.exchange(true, memory_order::acquire);
    }

    void lock(void)
    {
        for(unsigned k = 0; lc_.exchange(true, memory_order::acquire); ++k)
            thread_ops::pause(k);
    }

    void unlock(void)
    {
        lc_.store(false, memory_order::release);
    }
#endif
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
