/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "thread/atomic.h"
#include "thread/spinlock.h"
#include "thread/mutex.h"

#include "general/general.h"
#include "utility/utility.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <class Model_>
struct thread_model : Model_
{
    template <typename T>
    struct atomic { typedef nx::atomic<T, typename Model_::interlocked_t> type_t; };
};

/*
    none_lock, for single-thread model
*/

class none_lock : nx::NonCopyable
{
public:
    typedef none_lock lock_t;
    typedef lock_t handle_t;
    handle_t&       operator*(void)       { return (*this); }
    const handle_t& operator*(void) const { return (*this); }

public:
    bool trylock(void) { return true; }
    void lock   (void) {}
    void unlock (void) {}
};

/*
    single-thread model
*/

struct single_thread_model
{
    typedef by_interlocked_st interlocked_t;
    typedef none_lock         lock_t;
    typedef none_lock         mutex_t;
};

typedef thread_model<single_thread_model> by_thread_single;

/*
    multi-thread model
*/

struct multi_thread_model
{
    typedef by_interlocked_mt interlocked_t;
    typedef spin_lock         lock_t;
    typedef mutex             mutex_t;
};

typedef thread_model<multi_thread_model> by_thread_multi;

/*
    default thread model
*/

#ifndef NX_DEFAULT_THREAD_MODEL
#   ifdef NX_SINGLE_THREAD
#       define NX_DEFAULT_THREAD_MODEL  nx::by_thread_single
#   else
#       define NX_DEFAULT_THREAD_MODEL  nx::by_thread_multi
#   endif
#endif

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
