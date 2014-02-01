/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "general/general.h"
#include "typemanip/typemanip.h"
#include "utility/utility.h"

//////////////////////////////////////////////////////////////////////////

#include "al/thread/tlsptr.hxx"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <typename T>
class tls_ptr
    : public private_tls_ptr::detail<T>
    , public SafeBool<tls_ptr<T> >
    , nx::NonCopyable
{
    typedef private_tls_ptr::detail<T> base_t;

public:
    tls_ptr(void (*destructor)(void*) = nx::nulptr)
        : base_t(destructor) {}

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
