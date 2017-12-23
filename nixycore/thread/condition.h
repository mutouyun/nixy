/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#pragma once

#include "nixycore/thread/mutex.h"

#include "nixycore/general/general.h"
#include "nixycore/utility/utility.h"

#ifdef NX_SP_CXX11_CONDITION
#include <condition_variable>
#include <chrono> // std::chrono::milliseconds
#endif

//////////////////////////////////////////////////////////////////////////

#ifndef NX_SP_CXX11_CONDITION
#if defined(NX_OS_WIN)
#   include "detail/condition_win.hxx"
#elif defined(NX_OS_LINUX)
#   include "detail/condition_linux.hxx"
#endif
#endif/*NX_SP_CXX11_CONDITION*/

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

#ifdef NX_SP_CXX11_CONDITION
namespace private_condition
{
    class detail : private std::condition_variable_any
    {
        typedef std::condition_variable_any base_t;

        mutex& cond_mutex_;

    public:
        detail(mutex& mx)
            : cond_mutex_(mx)
        {}

        ~detail(void)
        {
            broadcast();
        }

    public:
        bool wait(int tm_ms = -1)
        {
            if (tm_ms < 0)
            {
                base_t::wait(cond_mutex_);
                return true;
            }
            else
            {
                return (base_t::wait_for(cond_mutex_, std::chrono::milliseconds(tm_ms))
                        == std::cv_status::no_timeout);
            }
        }

        void notify(void)
        {
            base_t::notify_one();
        }

        void broadcast(void)
        {
            base_t::notify_all();
        }
    };
}
#endif/*NX_SP_CXX11_CONDITION*/

class condition
    : public private_condition::detail, nx::noncopyable
{
public:
    condition(mutex& mx)
        : private_condition::detail(mx)
    {}
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
