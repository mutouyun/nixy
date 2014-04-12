/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/construct.h"
#include "nixycore/memory/alloc.h"

#include "nixycore/utility/limit_of.h"

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    a adapter allocator for stl
*/

template <typename T, class AllocT>
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
    struct rebind { typedef std_allocator<U, AllocT> other; };

public:
    pointer address(reference val) const
    { return &val; }
    const_pointer address(const_reference val) const
    { return &val; }

    size_type max_size() const
    { return (nx::limit_of<size_type>::upper / sizeof(T)); }

public:
    std_allocator(void) {}
    std_allocator(const std_allocator<T, AllocT>&) {}
    template <class U>
    std_allocator(const std_allocator<U, AllocT>&) {}

    std_allocator<T, AllocT>& operator=(const std_allocator<T, AllocT>&) { return (*this); }
    template <class U>
    std_allocator<T, AllocT>& operator=(const std_allocator<U, AllocT>&) { return (*this); }

    template <class U>
    bool operator==(const std_allocator<U, AllocT>&) const { return true; }
    /* vs2010 need this, or may get the error C2593 in vector(609) */
    template <class U>
    bool operator!=(const std_allocator<U, AllocT>&) const { return false; }

public:
    pointer allocate(size_type count, const pvoid = NULL)
    {
        if (count > this->max_size()) throw std::bad_alloc();
        pvoid p = nx::alloc<AllocT>(count * sizeof(T));
        if (!p) throw std::bad_alloc();
        return static_cast<pointer>(p);
    }

    void deallocate(pvoid p, size_type count)
    {
        nx::free<AllocT>(p, count * sizeof(T));
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

template <class ModelT>
struct alloc_model : ModelT
{
    /* the stl allocator type */
    template <typename T>
    struct std_allocator { typedef nx::std_allocator<T, ModelT> type_t; };
};

/*
    standard alloc policy model
*/

struct std_alloc_model
{
    static pvoid alloc(size_t size)
    { return (size ? ::malloc(size) : NULL); }
    static void free(pvoid p, size_t /*size*/)
    { if (p) ::free(p); }
    static pvoid realloc(pvoid p, size_t old_size, size_t new_size)
    { return (((p && old_size) || new_size) ? ::realloc(p, new_size) : NULL); }
};

namespace use
{
    typedef alloc_model<std_alloc_model> alloc_std;
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
