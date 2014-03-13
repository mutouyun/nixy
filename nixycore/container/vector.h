/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/alloc.h"
#include "nixycore/utility/rvalue.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"

// std::vector
#include <vector>

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <typename Type_, class Alloc_ = NX_DEFAULT_ALLOC>
class vector : public std::vector<Type_, typename Alloc_::template std_allocator<Type_>::type_t>
{
public:
    typedef std::vector<Type_, typename Alloc_::template std_allocator<Type_>::type_t> base_t;

public:
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

    template <typename Iterator_>
    vector(Iterator_ f, Iterator_ l,
           const typename base_t::allocator_type& a = typename base_t::allocator_type())
        : base_t(f, l, a)
    {}

    vector(const vector& rhs)
        : base_t(rhs)
    {}

    vector(const rvalue<vector, true>& rhs)
        : base_t()
    {
        base_t::swap(unmove(rhs));
    }

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

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
