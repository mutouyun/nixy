/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/std_alloc.h"
#include "nixycore/memory/mem_pool.h"

#include "nixycore/thread/tls_ptr.h"
#include "nixycore/pattern/singleton.h"

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    The thread local storage singleton
*/

template <typename T, class AllocT>
class TLSSingleton
{
    typedef tls_ptr<T> tls_t;

    static void destroy(void* p)
    {
        if (!p) return;
        nx_destruct(p, T);
        AllocT::free(p);
    }

public:
    static T& instance(void)
    {
        T* p = singleton<tls_t>(destroy);
        if (p) return (*p);
        singleton<tls_t>() = p =
            nx_construct(AllocT::alloc(sizeof(T)), T);
        return (*p);
    }
};

//////////////////////////////////////////////////////////////////////////

namespace private_mem_alloc
{
    typedef mem_pool<> tls_pool;

    inline tls_pool& create_pool(void)
    {
        return TLSSingleton<tls_pool, use::alloc_std>::instance();
    }
}

/*
    memory alloc policy model
*/

struct pool_alloc_model
{
    static pvoid alloc(size_t size)
    {
        return private_mem_alloc::create_pool().alloc(size);
    }

    static void free(pvoid p)
    {
        private_mem_alloc::create_pool().free(p);
    }

    static pvoid realloc(pvoid p, size_t size)
    {
        return private_mem_alloc::create_pool().realloc(p, size);
    }
};

namespace use
{
    typedef alloc_model<pool_alloc_model> alloc_pool;
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
