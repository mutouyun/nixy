/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#pragma once

#include "nixycore/memory/default_alloc.h"
#include "nixycore/utility/rvalue.h"

#include "nixycore/general/general.h"

#include <deque> // std::deque

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

#ifdef NX_SP_CXX11_ALIAS
template <typename T, class AllocT = NX_DEFAULT_ALLOC>
using deque = std::deque<T, typename AllocT::template std_allocator<T>::type_t>;
#else /*NX_SP_CXX11_ALIAS*/
template <typename T, class AllocT = NX_DEFAULT_ALLOC>
class deque : public std::deque<T, typename AllocT::template std_allocator<T>::type_t>
{
    typedef std::deque<T, typename AllocT::template std_allocator<T>::type_t> base_t;

public:
#ifdef NX_SP_CXX11_INHERITING
    using base_t::deque;
#else /*NX_SP_CXX11_INHERITING*/
    deque(void)
        : base_t()
    {}

    explicit deque(const typename base_t::allocator_type& a)
        : base_t(a)
    {}

    explicit deque(typename base_t::size_type n,
                   const typename base_t::value_type& v = typename base_t::value_type(),
                   const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(n, v, a)
    {}

    template <typename IteratorT>
    deque(IteratorT f, IteratorT l,
          const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(f, l, a)
    {}

    deque(const deque& rhs)
        : base_t(rhs)
    {}

    deque(nx_rref(deque, true) rhs)
        : base_t()
    {
        base_t::swap(nx::moved(rhs));
    }
#endif/*NX_SP_CXX11_INHERITING*/

    deque& operator=(deque rhs)
    {
        rhs.swap(*this);
        return (*this);
    }
};

/*
    Special swap algorithm
*/

template <typename T, class A>
inline void swap(deque<T, A>& x, deque<T, A>& y)
{
    x.swap(y);
}
#endif/*NX_SP_CXX11_ALIAS*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
