/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#include <pthread.h>

//////////////////////////////////////////////////////////////////////////
NX_BEG namespace private_mutex {
//////////////////////////////////////////////////////////////////////////

class detail
{
protected:
    pthread_mutex_t mx_;

public:
    detail(void)
    {
#   ifndef PTHREAD_MUTEX_RECURSIVE
#       define PTHREAD_MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE_NP
#   endif
        pthread_mutexattr_t attr;
        if (pthread_mutexattr_init(&attr) != 0) return;
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&mx_, &attr);
        pthread_mutexattr_destroy(&attr);
    }

    ~detail()
    { pthread_mutex_destroy(&mx_); }

    typedef pthread_mutex_t handle_t;
    handle_t&       operator*(void)       { return mx_; }
    const handle_t& operator*(void) const { return mx_; }

public:
    bool trylock(void)
    {
        return (pthread_mutex_trylock(&mx_) == 0);
    }

    void lock(void)
    {
        pthread_mutex_lock(&mx_);
    }

    void unlock(void)
    {
        pthread_mutex_unlock(&mx_);
    }
};

//////////////////////////////////////////////////////////////////////////
} NX_END
//////////////////////////////////////////////////////////////////////////
