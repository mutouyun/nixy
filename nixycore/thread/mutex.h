/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/general/general.h"
#include "nixycore/utility/utility.h"

#ifdef NX_SP_CXX11_MUTEX
#include <mutex>
#endif

//////////////////////////////////////////////////////////////////////////

#ifndef NX_SP_CXX11_MUTEX
#if defined(NX_OS_WIN)
#   include "detail/mutex_win.hxx"
#elif defined(NX_OS_LINUX)
#   include "detail/mutex_linux.hxx"
#endif
#endif/*NX_SP_CXX11_MUTEX*/

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

#ifdef NX_SP_CXX11_MUTEX
namespace private_mutex
{
    class detail : private std::mutex
    {
        typedef std::mutex base_t;

    public:
        typedef base_t::native_handle_type handle_t;
        handle_t operator*(void) { return base_t::native_handle(); }

    public:
        bool try_lock(void)
        {
            return base_t::try_lock();
        }

        void lock(void)
        {
            base_t::lock();
        }

        void unlock(void)
        {
            base_t::unlock();
        }
    };
}
#endif/*NX_SP_CXX11_MUTEX*/

class mutex
    : public private_mutex::detail, nx::noncopyable
{
public:
    typedef mutex lock_t;
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
