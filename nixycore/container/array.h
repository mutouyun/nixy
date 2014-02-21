/*
    The Nixy Library
    Code covered by the MIT License

    Modified from The Boost Library
    Modified by : mutouyun (http://darkc.at)

    (C) Copyright Nicolai M. Josuttis 2001.
*/

#pragma once

#include "nixycore/bugfix/assert.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/utility/utility.h"
#include "nixycore/algorithm/algorithm.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <typename T, size_t N>
class array : nx_operator(typename NX_SHIELD(array<T, N>), Unequal, Comparable)
{
public:
    // type definitions
    typedef T         value_type;
    typedef T*        iterator;
    typedef const T*  const_iterator;
    typedef T&        reference;
    typedef const T&  const_reference;
    typedef size_t    size_type;
    typedef ptrdiff_t difference_type;

public:
    // fixed-size array of elements of type T
    T elems_[N];
    
    // iterator support
    iterator        begin()                 { return elems_; }
    const_iterator  begin() const           { return elems_; }

    iterator        end()                   { return elems_ + N; }
    const_iterator  end() const             { return elems_ + N; }

public:
    // operator[]
    reference operator[](size_type i)
    {
        nx_assert(i < N);
        return elems_[i];
    }
    const_reference operator[](size_type i) const
    {
        nx_assert(i < N);
        return elems_[i];
    }

    // at()
    reference       at(size_type i)         { return elems_[i]; }
    const_reference at(size_type i) const   { return elems_[i]; }

    // front() and back()
    reference front()                       { return elems_[0]; }
    const_reference front() const           { return elems_[0]; }
    reference back()                        { return elems_[N - 1]; }
    const_reference back() const            { return elems_[N - 1]; }

    // size is constant
    static size_type size()                 { return N; }
    static bool empty()                     { return false; }
    static size_type max_size()             { return N; }

    // swap (note: linear complexity)
    void swap(array<T, N>& y)
    {
        for(size_type i = 0; i < N; ++i)
            nx::swap(elems_[i], y.elems_[i]);
    }

    // direct access to data (read-only)
    const T* data() const                   { return elems_; }
    T* data()                               { return elems_; }

    // use array as C array (direct read/write access to data)
    T* c_array()                            { return elems_; }

    // assignment with type conversion
    template <typename T2>
    array<T, N>& operator=(const array<T2, N>& rhs)
    {
        nx::copy(*this, rhs);
        return *this;
    }

    // assign one value to all elements
    void assign (const T& value)            { fill(value); }    // A synonym for fill
    void fill   (const T& value)            { nx::fill(*this, value); }

    // comparisons
    friend bool operator==(const array<T, N>& x, const array<T, N>& y)
    {
        return nx::equal(x, y);
    }
    friend bool operator< (const array<T, N>& x, const array<T, N>& y)
    {
        return nx::compare(x, y);
    }
};

/*
    Special assign algorithm
*/

template <typename V, typename T, size_t N>
inline void insert(array<T, N>& /*set*/, typename array<T, N>::iterator ite, const V& val)
{
    (*ite) = val;
}

template <typename V, typename T, size_t N>
inline void erase(array<T, N>& /*set*/, typename array<T, N>::iterator /*ite*/)
{
    // Do nothing
}

namespace private_assign
{
    template <typename T, size_t N>
    struct policy_type<array<T, N> >
    {
        typedef policy_array<array<T, N> > type_t;
    };
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
