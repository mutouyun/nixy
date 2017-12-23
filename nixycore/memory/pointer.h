/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#pragma once

#include "nixycore/memory/default_alloc.h"
#include "nixycore/memory/mem_guard.h"

#include "nixycore/thread/thread_model.h"
#include "nixycore/finalizer/holder.h"

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

namespace use
{
    struct dest_memory // destructor policy for memory
    {
        template <typename T> bool is_valid(nx_fref(T) r) const
        { return !!nx_extract(T, r); }
        template <typename T> void reset(T& r)
        { r = nx::nulptr; }
    };
}

namespace private_pointer
{
    template <typename T, class PtrT>
    struct opt : PtrT
    {
        typedef PtrT base_t;

        opt(void)
            : base_t()
        {}

        template <typename U>
        opt(nx_fref(U) r)
            : base_t(nx_forward(U, r))
        {}

        template <typename U, typename F>
        opt(nx_fref(U) r, nx_fref(F) dest_fr)
            : base_t(nx_forward(U, r), nx_forward(F, dest_fr))
        {}

        T* operator->(void) const { return  PtrT::get(); }
        T& operator* (void) const { return *PtrT::get(); }
    };

    template <class PtrT>
    struct opt<void, PtrT> : PtrT
    {
        typedef PtrT base_t;

        opt(void)
            : base_t()
        {}

        template <typename U>
        opt(nx_fref(U) r)
            : base_t(nx_forward(U, r))
        {}

        template <typename U>
        opt(nx_fref(U) r, size_t size)
            : base_t()
        {
            base_t::assign_to(nx_forward(U, r), nx_pass(nx::make_destructor(r, size)));
        }

        template <typename U, typename F>
        opt(nx_fref(U) r, nx_fref(F) dest_fr,
            typename nx::enable_if<!nx::is_scalar<F>::value, int>::type_t = 0)
            : base_t(nx_forward(U, r), nx_forward(F, dest_fr))
        {}

        template <typename U>
        void set(nx_fref(U) r, size_t size)
        {
            base_t::assign_to(nx_forward(U, r), nx_pass(nx::make_destructor(r, size)));
        }
    };
}

/*
    smart pointer
*/

template <typename T, class AllocT = NX_DEFAULT_ALLOC, class ModelT = NX_DEFAULT_THREAD_MODEL>
class pointer : public private_pointer::opt<T, holder<T*, use::dest_memory, AllocT, ModelT> >
{
    typedef private_pointer::opt<T, holder<T*, use::dest_memory, AllocT, ModelT> > opt_t;

public:
    typedef typename opt_t::base_t   base_t;
    typedef AllocT                   alloc_t;
    typedef typename base_t::model_t model_t;
    typedef T                        type_t;
    typedef typename base_t::type_t  point_t;

public:
    pointer(void)
        : opt_t()
    {}
    template <typename U>
    pointer(nx_fref(U) r)
        : opt_t(nx_forward(U, r))
    {}
    template <typename U, typename F>
    pointer(nx_fref(U) r, nx_fref(F) dest_fr)
        : opt_t(nx_forward(U, r), nx_forward(F, dest_fr))
    {}
    pointer(const pointer& r)
        : opt_t(static_cast<const base_t&>(r))
    {}
    pointer(nx_rref(pointer) r)
        : opt_t()
    { swap(nx::moved(r)); }

    pointer& operator=(pointer rhs)
    {
        rhs.swap(*this);
        return (*this);
    }

    void swap(pointer& rhs) { base_t::swap(static_cast<base_t&>(rhs)); }
};

/*
    Special swap algorithm
*/

template <typename T, class A, class M>
inline void swap(pointer<T, A, M>& x, pointer<T, A, M>& y)
{
    x.swap(y);
}

//////////////////////////////////////////////////////////////////////////
// cast pointer to another pointer type

namespace private_pointer_cast
{
    template <typename T, typename U>
    struct impl;

    template <typename T1, typename T2, class A, class M>
    struct impl<T1, pointer<T2, A, M> >
    {
        typedef T1 ret_t;
        typedef pointer<T2, A, M> prm_t;

        static ret_t cast(prm_t& p)
        {
            return static_cast<ret_t>(p.get());
        }
    };

    template <typename T, class A, class M>
    struct impl<pointer<T, A, M>, pointer<T, A, M> >
    {
        typedef pointer<T, A, M>& ret_t;
        typedef pointer<T, A, M>  prm_t;

        static ret_t cast(prm_t& p)
        {
            return p;
        }
    };

    template <typename T1, typename T2, class A, class M>
    struct impl<pointer<T1, A, M>, pointer<T2, A, M> >
    {
        typedef pointer<T1, A, M> ret_t;
        typedef pointer<T2, A, M> prm_t;

        static nx_rval(ret_t) cast(prm_t& p)
        {
            return ret_t(p);
        }
    };
}

/*
    pointer_cast
    -->
    nx::pointer<A> p1 = nx::alloc<A>();
    nx::pointer<B> p2 = nx::pointer_cast<nx::pointer<B> >(p1);
*/

template <typename T, typename U>
typename private_pointer_cast::impl<T, U>::ret_t pointer_cast(U& operand)
{
    return private_pointer_cast::impl<T, U>::cast(operand);
}

template <typename T, typename U>
typename private_pointer_cast::impl<T, U>::ret_t pointer_cast(const U& operand)
{
    return pointer_cast<T>(const_cast<U&>(operand));
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
