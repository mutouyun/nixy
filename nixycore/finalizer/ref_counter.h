/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/finalizer/scope_guard.h"

#include "nixycore/memory/alloc.h"
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

protected:
    ref_t* ref_;
    scope_guard* guard_;

public:
    ref_base(void)
        : ref_(nx::nulptr), guard_(nx::nulptr)
    {}

public:
    friend bool operator==(const ref_base& r1, const ref_base& r2)
    {
        return (r1.ref_ == r2.ref_);
    }

    void init(const nx::functor<void()>& guard)
    {
        dec();
        ref_   = nx::alloc<AllocT, ref_t>(1);
        nx_assert(ref_);
        guard_ = nx::alloc<AllocT, scope_guard>(nx::ref(guard));
        nx_assert(guard_);
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
        ++ (*ref_);
        guard_ = r.guard_;
        return true;
    }

    void dec(void)
    {
        if (!ref_ || (*ref_) == 0) return;
        if (--(*ref_)) return;
        // free the ref counter
        nx::free<AllocT>(ref_);
        ref_ = nx::nulptr;
        // call and free guard
        nx::free<AllocT>(guard_);
        guard_ = nx::nulptr;
    }

    ulong ref(void)
    {
        return (ref_ ? (*ref_) : 0);
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

    template <typename T>
    ref_counter(const T& r)
    {
        P::set(r);
    }

    template <typename T, typename F>
    ref_counter(const T& r, F f)
    {
        P::set(r, f);
    }

    ~ref_counter(void)
    {
        P::dec();
    }

public:
    template <typename T>
    ref_counter& operator=(const T& r)
    {
        P::set(r);
        return (*this);
    }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
