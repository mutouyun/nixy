/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/pattern/singleton.h"
#include "nixycore/finalizer/scopeguard.h"
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
    NX_CONCEPT(lock_t, lock_t);

    template <typename T>
    struct lock_fr
    {
        T& lc_;
        lock_fr(T& lc) : lc_(lc) { lc_.lock(); }
        void operator() (void) { lc_.unlock(); }
    };
}

template <typename T>
inline typename enable_if<private_lock_scope::has_lock_t<T>::value,
functor<void()> >::type_t make_destructor(T& r)
{
    return private_lock_scope::lock_fr<T>(r);
}

#ifdef NX_SINGLE_THREAD

#define nx_lock_scope(...)
#define nx_lock_sole(Lock_)
#define nx_lock_type(Lock_, ...)

#else /*NX_SINGLE_THREAD*/

#define nx_lock_scope(...) \
    nx_guard_scope(__VA_ARGS__)

#define nx_lock_sole(Lock_) \
    nx_lock_scope(nx::singleton<Lock_>())

#define nx_lock_type(Lock_, ...) \
    nx_lock_scope(nx::singleton<__VA_ARGS__, Lock_>())

#endif/*NX_SINGLE_THREAD*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
