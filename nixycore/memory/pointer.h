/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/alloc.h"
#include "nixycore/memory/memguard.h"

#include "nixycore/thread/threadmodel.h"
#include "nixycore/finalizer/holder.h"

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    destructor policy for memory
*/

struct by_dest_memory
{
    template <typename T> bool is_valid(const T& r) const
    { return !!r; }
    template <typename T> void reset(T& r)
    { r = nx::nulptr; }
};

//////////////////////////////////////////////////////////////////////////

namespace private_pointer
{
    template <typename T, class Ptr_>
    struct opt : Ptr_
    {
        typedef Ptr_ base_t;

        opt() : base_t() {}
        template <typename U>
        opt(const U& r) : base_t(r) {}
        template <typename U, typename F>
        opt(const U& r, F dest_fr) : base_t(r, dest_fr) {}

        T* operator->(void) const { return  Ptr_::get(); }
        T& operator* (void) const { return *Ptr_::get(); }
    };

    template <class Ptr_>
    struct opt<void, Ptr_> : Ptr_
    {
        typedef Ptr_ base_t;

        opt() : base_t() {}
        template <typename U>
        opt(const U& r) : base_t(r) {}
        template <typename U, typename F>
        opt(const U& r, F dest_fr) : base_t(r, dest_fr) {}
    };
}

/*
    smart pointer
*/

template <typename T, class Alloc_ = NX_DEFAULT_ALLOC, class Model_ = NX_DEFAULT_THREAD_MODEL>
class pointer : public private_pointer::opt<T, holder<T*, by_dest_memory, Alloc_, Model_> >
{
    typedef private_pointer::opt<T, holder<T*, by_dest_memory, Alloc_, Model_> > opt_t;

public:
    typedef typename opt_t::base_t   base_t;
    typedef Alloc_                   alloc_t;
    typedef typename base_t::model_t model_t;
    typedef T                        type_t;
    typedef typename base_t::type_t  point_t;

public:
    pointer(void)
        : opt_t()
    {}
    template <typename U>
    pointer(const U& r)
        : opt_t(r)
    {}
    template <typename U, typename F>
    pointer(const U& r, F dest_fr)
        : opt_t(r, dest_fr)
    {}
    pointer(const pointer& r)
        : opt_t(static_cast<const base_t&>(r))
    {}
    pointer(const rvalue<pointer>& r)
        : opt_t()
    { swap(unmove(r)); }

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

        static rvalue<ret_t> cast(prm_t& p)
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
