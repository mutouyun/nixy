/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#if defined(NX_OS_WINCE) || (WINVER < 0x0600)
#include "thread/lockguard.h"
#include "thread/spinlock.h"
#endif

//////////////////////////////////////////////////////////////////////////
NX_BEG namespace private_condition {
//////////////////////////////////////////////////////////////////////////

/*
    wince, winxp, win2003, ...
*/
#if defined(NX_OS_WINCE) || (WINVER < 0x0600)

class detail
{
    HANDLE    sema_;
    mutex&    cond_mutex_;
    spin_lock lock_;
    long      counter_;

public:
    detail(mutex& mx)
        : sema_(CreateSemaphore(NULL, 0, LONG_MAX, NULL))
        , cond_mutex_(mx)
        , counter_(0)
    {}

    ~detail()
    {
        broadcast();
        CloseHandle(sema_);
    }

public:
    bool wait(int tm_ms = -1)
    {
        {
            nx_lock_scope(lock_);
            ++ counter_;
            cond_mutex_.unlock();
        }
        bool ret = (WaitForSingleObject(sema_, (tm_ms < 0) ? INFINITE : tm_ms) == WAIT_OBJECT_0);
        cond_mutex_.lock();
        return ret;
    }

    void notify(void)
    {
        {
            nx_lock_scope(lock_);
            if (counter_ == 0) return;
            -- counter_;
        }
        ReleaseSemaphore(sema_, 1, NULL);
    }

    void broadcast(void)
    {
        long all_count;
        {
            nx_lock_scope(lock_);
            if (counter_ == 0) return;
            all_count = counter_;
            counter_ = 0;
        }
        ReleaseSemaphore(sema_, all_count, NULL);
    }
};

/*
    vista, win7, ...
*/
#else // (WINVER >= 0x0600)

class detail
{
    CONDITION_VARIABLE cond_;
    mutex&             cond_mutex_;

public:
    detail(mutex& mx)
        : cond_mutex_(mx)
    {
        InitializeConditionVariable(&cond_);
    }

    ~detail()
    {
        broadcast();
    }

public:
    bool wait(int tm_ms = -1)
    {
        // returns true if signal comes, false otherwise.
        return !!SleepConditionVariableCS(&cond_, &(*cond_mutex_), (tm_ms < 0) ? INFINITE : tm_ms);
    }

    void notify(void)
    {
        WakeConditionVariable(&cond_);
    }

    void broadcast(void)
    {
        WakeAllConditionVariable(&cond_);
    }
};

#endif /* vista, win7, ... */

//////////////////////////////////////////////////////////////////////////
} NX_END
//////////////////////////////////////////////////////////////////////////
