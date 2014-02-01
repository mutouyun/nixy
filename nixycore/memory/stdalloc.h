/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "memory/construct.h"

#include "general/general.h"
#include "utility/utility.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    a adapter allocator for stl
*/

template <typename T, class Alloc_>
struct std_allocator
{
public:
    // type definitions
    typedef T         value_type;
    typedef T*        pointer;
    typedef const T*  const_pointer;
    typedef T&        reference;
    typedef const T&  const_reference;
    typedef size_t    size_type;
    typedef ptrdiff_t difference_type;

    // the other type of std_allocator
    template <typename U>
    struct rebind { typedef std_allocator<U, Alloc_> other; };

public:
    pointer address(reference val) const
    { return &val; }
    const_pointer address(const_reference val) const
    { return &val; }

    size_type max_size() const
    { return (nx::limit_of<size_type>::upper / sizeof(T)); }

public:
    std_allocator() {}
    std_allocator(const std_allocator<T, Alloc_>&) {}
    template <class U>
    std_allocator(const std_allocator<U, Alloc_>&) {}

    std_allocator<T, Alloc_>& operator=(const std_allocator<T, Alloc_>&) { return (*this); }
    template <class U>
    std_allocator<T, Alloc_>& operator=(const std_allocator<U, Alloc_>&) { return (*this); }

    template <class U>
    bool operator==(const std_allocator<U, Alloc_>&) { return true; }

public:
    pointer allocate(size_type count, const pvoid = nx::nulptr)
    {
        if (count > this->max_size()) throw std::bad_alloc();
        pvoid p = Alloc_::alloc(count * sizeof(T));
        if (!p) throw std::bad_alloc();
        return static_cast<pointer>(p);
    }

    void deallocate(pvoid p, size_type /*count*/)
    {
        Alloc_::free(p);
    }

    static void construct(pointer p, const T& val)
	{
        nx_construct(p, value_type, val);
    }

    static void destroy(pointer p)
    {
        nx_destruct(p, value_type);
    }
};

/*
    alloc policy base
*/

template <class Model_>
struct alloc_model : Model_
{
    /* the stl allocator type */
    template <typename T>
    struct std_allocator { typedef nx::std_allocator<T, Model_> type_t; };
};

/*
    standard alloc policy
*/

struct std_alloc_model
{
    static pvoid alloc(size_t size)            { return (size ? ::malloc(size) : nx::nulptr); }
    static void free(pvoid p)                  { if (p) ::free(p); }
    static pvoid realloc(pvoid p, size_t size) { return ((p || size) ? ::realloc(p, size) : nx::nulptr); }
};

typedef alloc_model<std_alloc_model> by_alloc_std;

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
