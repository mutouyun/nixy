/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#include <pthread.h>
#include <unistd.h>

//////////////////////////////////////////////////////////////////////////
NX_BEG namespace thread_ops {
//////////////////////////////////////////////////////////////////////////

typedef pthread_t   id_t;
typedef pthread_t   handle_t;

#define NX_THREAD_PROC(name, ...) void* (name)(void* __VA_ARGS__)
typedef NX_THREAD_PROC(*proc_t);

inline handle_t create(proc_t proc, pvoid arg = nx::nulptr, id_t* thr_id = nx::nulptr)
{
    handle_t hd = 0;
    int err = pthread_create(&hd, NULL, proc, arg);
    if (err == 0)
    {
        if (thr_id) (*thr_id) = hd;
    }
    else
        hd = 0;
    return hd;
}

inline handle_t current_handle(void)
{
    return pthread_self();
}

inline id_t current_id(void)
{
    return pthread_self();
}

inline handle_t id2handle(id_t id)
{
    return (handle_t)id;
}

inline id_t handle2id(handle_t hd)
{
    return (id_t)hd;
}

inline bool cancel(handle_t hd)
{
    return pthread_cancel(hd);
}

inline bool join(handle_t hd)
{
    return (pthread_join(hd, NULL) == 0);
}

inline bool detach(handle_t hd)
{
    return (pthread_detach(hd) == 0);
}

inline void exit(void)
{
    pthread_exit(0);
}

inline void sleep(unsigned ms)
{
    usleep(ms * 1000);
}

inline void yield(void)
{
    pthread_yield();
}

//////////////////////////////////////////////////////////////////////////
} NX_END
//////////////////////////////////////////////////////////////////////////
