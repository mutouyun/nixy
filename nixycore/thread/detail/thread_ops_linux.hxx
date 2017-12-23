/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#include <pthread.h>
#include <unistd.h>
#include <sys/sysinfo.h> // get_nprocs

//////////////////////////////////////////////////////////////////////////
NX_BEG namespace thread_ops {
//////////////////////////////////////////////////////////////////////////

typedef pthread_t id_t;
typedef pthread_t handle_t;
typedef pthread_t native_id_t;

inline void clear_id(id_t& id)
{
    id = 0;
}

inline void clear_hd(handle_t& hd)
{
    hd = 0;
}

#define NX_THREAD_PROC(name, ...) void* (name)(void* __VA_ARGS__)
typedef NX_THREAD_PROC(*proc_t);

inline handle_t create(proc_t proc, pvoid arg = 0, id_t* thr_id = 0)
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

inline void exit(void)
{
    pthread_exit(0);
}

inline native_id_t native_current_id(void)
{
    return pthread_self();
}

inline id_t current_id(void)
{
    return pthread_self();
}

inline id_t handle2id(handle_t hd)
{
    return (id_t)hd;
}

inline bool join(handle_t hd)
{
    return (pthread_join(hd, NULL) == 0);
}

inline bool detach(handle_t hd)
{
    return (pthread_detach(hd) == 0);
}

inline void sleep(unsigned ms)
{
    usleep(ms * 1000);
}

inline void yield(void)
{
    pthread_yield();
}

inline unsigned hardware_concurrency(void)
{
    static unsigned cpu_count = 0;
    if (cpu_count > 0) return cpu_count;
    cpu_count = static_cast<unsigned>(get_nprocs());
    if (cpu_count < 1) cpu_count = 1; // fail-safe
    return cpu_count;
}

//////////////////////////////////////////////////////////////////////////
} NX_END
//////////////////////////////////////////////////////////////////////////
