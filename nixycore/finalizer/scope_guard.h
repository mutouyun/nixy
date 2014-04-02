/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/delegate/functor.h"
#include "nixycore/delegate/bind.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/utility/utility.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    Execute guard function when the enclosing scope exits
*/

class scope_guard : noncopyable
{
    nx::functor<void()> destructor_;

public:
    explicit scope_guard(const nx::functor<void()>& destructor)
        : destructor_(destructor)
    {}

    ~scope_guard(void)
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
inline typename nx::enable_if<!nx::is_numeric<F>::value,
functor<void()> >::type_t make_destructor(T r, F dest_fr)
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
