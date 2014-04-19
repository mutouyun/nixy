/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/pattern/singleton.h"
#include "nixycore/finalizer/scope_guard.h"
#include "nixycore/delegate/bind.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/utility/utility.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    nx_lock_scope
*/

namespace private_lock_scope
{
    template <typename T>
    struct lock_fr
    {
        T& lc_;
        lock_fr(T& lc) : lc_(lc) { lc_.lock(); }
        void operator() (void) { lc_.unlock(); }
    };
}

template <typename T>
inline private_lock_scope::lock_fr<T> make_lock_fr(T& r)
{
    return private_lock_scope::lock_fr<T>(r);
}

#ifdef NX_SINGLE_THREAD

#define nx_lock_scope(...)
#define nx_lock_sole(Lock_)
#define nx_lock_type(Lock_, ...)

#else /*NX_SINGLE_THREAD*/

#define nx_lock_scope(...) \
    nx_guard_scope(nx::make_lock_fr(__VA_ARGS__))

#define nx_lock_sole(Lock_) \
    nx_lock_scope(nx::singleton<Lock_>())

#define nx_lock_type(Lock_, ...) \
    nx_lock_scope(nx::singleton<__VA_ARGS__, Lock_>())

#endif/*NX_SINGLE_THREAD*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
