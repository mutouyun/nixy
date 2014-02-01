/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#include "bugfix/assert.h"

#include <pthread.h>

//////////////////////////////////////////////////////////////////////////
NX_BEG namespace private_tls_ptr {
//////////////////////////////////////////////////////////////////////////

template <typename T>
class detail
{
    pthread_key_t index_;
    void (*destructor_)(void*);

public:
    detail(void (*destructor)(void*))
        : destructor_(destructor)
    {
        pthread_key_create(&index_, destructor);
    }

    ~detail()
    {
        pthread_key_delete(index_);
    }

    bool set(T* ptr)
    {
        return (pthread_setspecific(index_, (void*)ptr) == 0);
    }

    T* get(void) const
    {
        return (T*)pthread_getspecific(index_);
    }
};

//////////////////////////////////////////////////////////////////////////
} NX_END
//////////////////////////////////////////////////////////////////////////
