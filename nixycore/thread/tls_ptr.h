/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/utility/utility.h"

//////////////////////////////////////////////////////////////////////////

#if defined(NX_OS_WIN)
#   include "detail/tls_ptr_win.hxx"
#elif defined(NX_OS_LINUX)
#   include "detail/tls_ptr_linux.hxx"
#endif

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <typename T>
class tls_ptr
    : public private_tls_ptr::detail<T>
    , public safe_bool<tls_ptr<T> >
    , nx::noncopyable
{
    typedef private_tls_ptr::detail<T> base_t;

public:
    tls_ptr(void (*destructor)(void*) = nx::nulptr)
        : base_t(destructor)
    {}

    bool checkSafeBool(void) const
    { return !!base_t::get(); }

    tls_ptr& operator=(T* ptr)
    {
        base_t::set(ptr);
        return (*this);
    }

    operator T*(void) const
    { return base_t::get(); }

    T* operator->(void) const { return  base_t::get(); }
    T& operator* (void) const { return *base_t::get(); }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
