/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#pragma once

#include "nixycore/memory/default_alloc.h"
#include "nixycore/utility/rvalue.h"

#include "nixycore/general/general.h"

#include <list> // std::list

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

#ifdef NX_SP_CXX11_ALIAS
template <typename T, class AllocT = NX_DEFAULT_ALLOC>
using list = std::list<T, typename AllocT::template std_allocator<T>::type_t>;
#else /*NX_SP_CXX11_ALIAS*/
template <typename T, class AllocT = NX_DEFAULT_ALLOC>
class list : public std::list<T, typename AllocT::template std_allocator<T>::type_t>
{
    typedef std::list<T, typename AllocT::template std_allocator<T>::type_t> base_t;

public:
#ifdef NX_SP_CXX11_INHERITING
    using base_t::list;
#else /*NX_SP_CXX11_INHERITING*/
    list(void)
        : base_t()
    {}

    explicit list(const typename base_t::allocator_type& a)
        : base_t(a)
    {}

    explicit list(typename base_t::size_type n,
                  const typename base_t::value_type& v = typename base_t::value_type(),
                  const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(n, v, a)
    {}

    template <typename IteratorT>
    list(IteratorT f, IteratorT l,
         const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(f, l, a)
    {}

    list(const list& rhs)
        : base_t(rhs)
    {}

    list(nx_rref(list, true) rhs)
        : base_t()
    {
        base_t::swap(nx::moved(rhs));
    }
#endif/*NX_SP_CXX11_INHERITING*/

    list& operator=(list rhs)
    {
        rhs.swap(*this);
        return (*this);
    }
};

/*
    Special swap algorithm
*/

template <typename T, class A>
inline void swap(list<T, A>& x, list<T, A>& y)
{
    x.swap(y);
}
#endif/*NX_SP_CXX11_ALIAS*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
