/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/default_alloc.h"
#include "nixycore/utility/rvalue.h"

#include "nixycore/general/general.h"

#include <vector> // std::vector

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

#ifdef NX_SP_CXX11_ALIAS
template <typename T, class AllocT = NX_DEFAULT_ALLOC>
using vector = std::vector<T, typename AllocT::template std_allocator<T>::type_t>;
#else /*NX_SP_CXX11_ALIAS*/
template <typename T, class AllocT = NX_DEFAULT_ALLOC>
class vector : public std::vector<T, typename AllocT::template std_allocator<T>::type_t>
{
    typedef std::vector<T, typename AllocT::template std_allocator<T>::type_t> base_t;

public:
#ifdef NX_SP_CXX11_INHERITING
    using base_t::vector;
#else /*NX_SP_CXX11_INHERITING*/
    vector(void)
        : base_t()
    {}

    explicit vector(const typename base_t::allocator_type& a)
        : base_t(a)
    {}

    explicit vector(typename base_t::size_type n,
                    const typename base_t::value_type& v = typename base_t::value_type(),
                    const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(n, v, a)
    {}

    template <typename IteratorT>
    vector(IteratorT f, IteratorT l,
           const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(f, l, a)
    {}

    vector(const vector& rhs)
        : base_t(rhs)
    {}

    vector(nx_rref(vector, true) rhs)
        : base_t()
    {
        base_t::swap(nx::moved(rhs));
    }
#endif/*NX_SP_CXX11_INHERITING*/

    vector& operator=(vector rhs)
    {
        rhs.swap(*this);
        return (*this);
    }
};

/*
    Special swap algorithm
*/

template <typename T_, class A_>
inline void swap(vector<T_, A_>& x, vector<T_, A_>& y)
{
    x.swap(y);
}
#endif/*NX_SP_CXX11_ALIAS*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
