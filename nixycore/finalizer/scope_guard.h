/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#pragma once

#include "nixycore/delegate/functor.h"

#include "nixycore/bugfix/noexcept.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/utility/utility.h"
#include "nixycore/algorithm/algorithm.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    Execute guard function when the enclosing scope exits
*/

template <typename F>
class scope_guard : noncopyable
{
    F destructor_;
    mutable bool dismiss_;

public:
    explicit scope_guard(const F& destructor)
        : destructor_(destructor)
        , dismiss_(false)
    {}

    explicit scope_guard(nx_rref(F) destructor)
        : destructor_(nx::move(destructor))
        , dismiss_(false)
    {}

    scope_guard(nx_rref(scope_guard, true) rhs)
        : destructor_(nx::move(rhs.destructor_))
        , dismiss_(true) // dismiss rhs
    {
        nx::swap(dismiss_, nx::moved(rhs).dismiss_);
    }

    ~scope_guard(void)
    {
        if (!dismiss_) try
        {
            destructor_();
        }
        /*
            In the realm of exceptions,
            it is fundamental that you can do nothing
            if your "undo/recover" action fails.
        */
        catch(...)
        {
            // Do nothing
        }
    }

    void dismiss() const nx_noexcept
    {
        dismiss_ = true;
    }

    nx_rval(functor<void()>, true) get(void) const
    {
        functor<void()> tmp(destructor_);
        return nx::move(tmp);
    }

    void swap(scope_guard& rhs)
    {
        nx::swap(destructor_, rhs.destructor_);
        nx::swap(dismiss_,    rhs.dismiss_);
    }
};

/*
    -->
    void func(void)
    {
        int* p = new int(2);
        nx_guard_scope(nx::bind(new_dest, p));
        ...
    }
*/

template <typename F>
inline nx_rval(scope_guard<typename nx::decay<F>::type_t>, true)
    make_scope_guard(nx_fref(F) f)
{
    return scope_guard<typename nx::decay<F>::type_t>(nx_forward(F, f));
}

#define NX_GUARD_SCOPE_NAM_(nn) scope_guard_##nn##__
#define NX_GUARD_SCOPE_(nn, call) \
    nx_auto(NX_UNUSED NX_GUARD_SCOPE_NAM_(nn), call)

#define nx_guard_scope(...) NX_GUARD_SCOPE_(__LINE__, nx::make_scope_guard(__VA_ARGS__))

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
