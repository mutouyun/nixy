/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "delegate/functor.h"
#include "delegate/bind.h"

#include "general/general.h"
#include "typemanip/typemanip.h"
#include "utility/utility.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    Execute guard function when the enclosing scope exits
*/

class scope_guard : nx::NonCopyable
{
    nx::functor<void()> destructor_;

public:
    explicit scope_guard(const nx::functor<void()>& destructor)
        : destructor_(destructor)
    {}

    ~scope_guard()
    { if (destructor_) destructor_(); }

    void dismiss()
    { destructor_ = nx::nulptr; }

    const nx::functor<void()>& get(void) const
    { return destructor_; }

    void swap(scope_guard& rhs)
    { destructor_.swap(rhs.destructor_); }
};

/*
    -->
    void func(void)
    {
        int* p = new int(2);
        nx_guard_scope(p, new_dest);
        ...
    }
*/

template <typename T>
inline typename enable_if<is_function<T>::value,
functor<void()> >::type_t make_destructor(T r)
{
    return r;
}

template <typename T, typename F>
inline functor<void()> make_destructor(T r, F dest_fr)
{
    return bind<void>(dest_fr, r);
}

#define NX_GUARD_SCOPE_NAM_(nn) guard_##nn##__
#define NX_GUARD_SCOPE_(nn, call) \
    nx::scope_guard NX_UNUSED NX_GUARD_SCOPE_NAM_(nn)(call)

#define nx_guard_scope(...) NX_GUARD_SCOPE_(__LINE__, nx::make_destructor(__VA_ARGS__))

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
