/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/finalizer/scope_guard.h"

#include "nixycore/memory/default_alloc.h"
#include "nixycore/thread/thread_model.h"
#include "nixycore/bugfix/assert.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/utility/utility.h"
#include "nixycore/algorithm/algorithm.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <class AllocT = NX_DEFAULT_ALLOC, class ModelT = NX_DEFAULT_THREAD_MODEL>
class ref_base : nx_operator(typename NX_SHIELD(ref_base<AllocT, ModelT>), unequal)
{
public:
    typedef typename ModelT::template atomic<ulong>::type_t ref_t;
    typedef scope_guard<nx::functor<void()> > guard_t;

protected:
    ref_t* ref_;
    guard_t* guard_;

public:
    ref_base(void)
        : ref_(nx::nulptr), guard_(nx::nulptr)
    {}

public:
    friend bool operator==(const ref_base& r1, const ref_base& r2)
    {
        return (r1.ref_ == r2.ref_);
    }

    template <typename F>
    void init(nx_fref(F, dest_fr))
    {
        dec();
        nx_verify(ref_   = nx::alloc<AllocT, ref_t>(1));
        nx_verify(guard_ = nx::alloc<AllocT, guard_t>(nx_forward(F, dest_fr)));
    }

    bool set(const ref_base& r)
    {
        if ((*this) == r) return true;
        dec();
        return inc(r);
    }

    bool inc(const ref_base& r)
    {
        ref_ = r.ref_;
        if (!ref_) return false;
        ref_->fetch_add(1, memory_order::relaxed);
        guard_ = r.guard_;
        return true;
    }

    void dec(void)
    {
        if (!ref_ ||
             ref_->fetch_sub(1, memory_order::relaxed) > 1) return;
        // free the ref counter
        nx::free<AllocT>(ref_);
        ref_ = nx::nulptr;
        // call and free guard
        nx::free<AllocT>(guard_);
        guard_ = nx::nulptr;
    }

    ulong ref(void)
    {
        return (ref_ ? ref_->load(memory_order::relaxed) : 0);
    }

    void swap(ref_base& rhs)
    {
        nx::swap(ref_  , rhs.ref_);
        nx::swap(guard_, rhs.guard_);
    }
};

/*
    The reference counter
*/

template <typename P>
class ref_counter : public P
{
public:
    ref_counter(void) {}

    ref_counter(const ref_counter& r)
    {
        P::set(r);
    }

    template <typename U>
    ref_counter(nx_fref(U, r), 
                typename nx::enable_if<!nx::is_same<U, ref_counter>::value, int>::type_t = 0)
    {
        P::set(nx_forward(U, r));
    }

    template <typename U, typename F>
    ref_counter(nx_fref(U, r), nx_fref(F, dest_fr))
    {
        P::set(nx_forward(U, r), nx_forward(F, dest_fr));
    }

    ~ref_counter(void)
    {
        P::dec();
    }
};

/*
    The destructor maker
*/

#ifdef NX_SP_CXX11_RVALUE_REF
template <typename T, typename F>
inline functor<void()> make_destructor(nx_fref(T, r), nx_fref(F, dest_fr))
{
    return bind<void>(nx_forward(F, dest_fr), nx_forward(T, r));
}
#else /*NX_SP_CXX11_RVALUE_REF*/
template <typename T, typename F>
inline nx_rval(functor<void()>, true)
    make_destructor(nx_fref(T, r), nx_fref(F, dest_fr))
{
    return nx::move(functor<void()>(bind<void>(nx_forward(F, dest_fr), nx_forward(T, r))));
}
#endif/*NX_SP_CXX11_RVALUE_REF*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
