/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/general/general.h"

#ifdef NX_SP_CXX11_FORWARD_LIST

#include "nixycore/memory/default_alloc.h"
#include "nixycore/utility/rvalue.h"

#include <forward_list> // std::forward_list

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

#ifdef NX_SP_CXX11_ALIAS
template <typename T, class AllocT = NX_DEFAULT_ALLOC>
using forward_list = std::forward_list<T, typename AllocT::template std_allocator<T>::type_t>;
#else /*NX_SP_CXX11_ALIAS*/
template <typename T, class AllocT = NX_DEFAULT_ALLOC>
class forward_list : public std::forward_list<T, typename AllocT::template std_allocator<T>::type_t>
{
    typedef std::forward_list<T, typename AllocT::template std_allocator<T>::type_t> base_t;

public:
#ifdef NX_SP_CXX11_INHERITING
    using base_t::forward_list;
#else /*NX_SP_CXX11_INHERITING*/
    forward_list(void)
        : base_t()
    {}

    explicit forward_list(typename base_t::size_type count)
        : base_t(count)
    {}

    forward_list(typename base_t::size_type count, const T& val, 
                 const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(count, val, a)
    {}

    explicit forward_list(const typename base_t::allocator_type& a)
        : base_t(a)
    {}

    template <typename IteratorT>
    forward_list(IteratorT f, IteratorT l,
                 const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(f, l, a)
    {}

    forward_list(const forward_list& rhs)
        : base_t(rhs)
    {}

    forward_list(nx_rref(forward_list, true) rhs)
        : base_t()
    {
        base_t::swap(nx::moved(rhs));
    }
#endif/*NX_SP_CXX11_INHERITING*/

    forward_list& operator=(forward_list rhs)
    {
        rhs.swap(*this);
        return (*this);
    }
};

/*
    Special swap algorithm
*/

template <typename T, class A>
inline void swap(forward_list<T, A>& x, forward_list<T, A>& y)
{
    x.swap(y);
}
#endif/*NX_SP_CXX11_ALIAS*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////

#endif/*NX_SP_CXX11_FORWARD_LIST*/
