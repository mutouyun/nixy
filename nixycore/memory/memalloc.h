/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/stdalloc.h"
#include "nixycore/memory/mempool.h"

#include "nixycore/thread/tlsptr.h"
#include "nixycore/pattern/singleton.h"

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    The thread local storage singleton
*/

template <class Alloc_>
struct tls_singleton
{
    template <typename T>
    class type_t
    {
        typedef tls_ptr<T> tls_t;

        static void destroy(void* p)
        {
            if (!p) return;
            nx_destruct(p, T);
            Alloc_::free(p);
        }

    public:
        static T& instance(void)
        {
            T* p = singleton<tls_t>(destroy);
            if (p) return (*p);
            singleton<tls_t>() = p = 
                nx_construct(Alloc_::alloc(sizeof(T)), T);
            return (*p);
        }
    };
};

//////////////////////////////////////////////////////////////////////////

namespace private_memalloc
{
    typedef mem_pool<> tls_pool;

    inline tls_pool& create_pool(void)
    {
        return tls_singleton<by_alloc_std>::type_t<tls_pool>::instance();
    }
}

/*
    memory alloc policy
*/

struct pool_alloc_model
{
    static pvoid alloc(size_t size)
    {
        return private_memalloc::create_pool().alloc(size);
    }

    static void free(pvoid p)
    {
        private_memalloc::create_pool().free(p);
    }

    static pvoid realloc(pvoid p, size_t size)
    {
        return private_memalloc::create_pool().realloc(p, size);
    }
};

typedef alloc_model<pool_alloc_model> by_alloc_pool;

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
