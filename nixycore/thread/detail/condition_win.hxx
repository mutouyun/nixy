/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#if defined(NX_OS_WINCE) || (WINVER < 0x0600)
#include "nixycore/thread/lock_guard.h"
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
    mutex     lock_;
    HANDLE    sema_, handshake_;
    mutex&    cond_mutex_;
    long      counter_;

public:
    detail(mutex& mx)
        : sema_     (CreateSemaphore(NULL, 0, LONG_MAX, NULL))
        , handshake_(CreateSemaphore(NULL, 0, LONG_MAX, NULL))
        , cond_mutex_(mx)
        , counter_(0)
    {}

    ~detail(void)
    {
        broadcast();
        CloseHandle(handshake_);
        CloseHandle(sema_);
    }

public:
    bool wait(int tm_ms = -1)
    {
        {
            nx_lock_scope(lock_);
            ++ counter_;
        }
        cond_mutex_.unlock();
        bool ret_s = (WaitForSingleObject(sema_, (tm_ms < 0) ? INFINITE : tm_ms) == WAIT_OBJECT_0);
        bool ret_h = !!ReleaseSemaphore(handshake_, 1, NULL);
        cond_mutex_.lock();
        return ret_s && ret_h;
    }

    void notify(void)
    {
        nx_lock_scope(lock_);
        if (counter_ > 0)
        {
            ReleaseSemaphore(sema_, 1, NULL);
            -- counter_;
            WaitForSingleObject(handshake_, INFINITE);
        }
    }

    void broadcast(void)
    {
        nx_lock_scope(lock_);
        if (counter_ > 0)
        {
            ReleaseSemaphore(sema_, counter_, NULL);
            do
            {
                -- counter_;
                WaitForSingleObject(handshake_, INFINITE);
            } while (counter_ > 0);
        }
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

    ~detail(void)
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
