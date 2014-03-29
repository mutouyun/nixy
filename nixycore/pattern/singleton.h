/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/thread/spin_lock.h"
#include "nixycore/thread/atomic.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/utility/utility.h"
#include "nixycore/preprocessor/preprocessor.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <typename T>
class Singleton : noncopyable
{
#ifndef NX_SINGLE_THREAD

    static spin_lock  lc_;
    static atomic<T*> ip_;

#   define NX_SINGLETON_(...) \
        if (!ip_) \
        { \
            lc_.lock(); \
            if (!(ip_.get())) \
            { \
                static T ir __VA_ARGS__; \
                ip_ = &ir; \
            } \
            lc_.unlock(); \
        } \
        return (*(ip_.get()))

#else /*NX_SINGLE_THREAD*/

    static T* ip_;

#   define NX_SINGLETON_(...) \
        if (!ip_) \
        { \
            static T ir __VA_ARGS__; \
            ip_ = &ir; \
        } \
        return (*ip_)

#endif/*NX_SINGLE_THREAD*/

public:

    /* Get the instance of this singleton */

    static T& instance(void)
    {
        NX_SINGLETON_();
    }

#define NX_INSTANCE_(n) \
    template <NX_PP_TYPE_1(n, typename P)> \
    static T& instance(NX_PP_TYPE_2(n, P, par)) \
    { \
        NX_SINGLETON_( (NX_PP_TYPE_1(n, par)) ); \
    }
    NX_PP_MULT_MAX(NX_INSTANCE_)
#undef NX_INSTANCE_
#undef NX_SINGLETON_
};

#ifndef NX_SINGLE_THREAD

template <typename T> spin_lock  Singleton<T>::lc_;
template <typename T> atomic<T*> Singleton<T>::ip_;

#else /*NX_SINGLE_THREAD*/

template <typename T> T* Singleton<T>::ip_;

#endif/*NX_SINGLE_THREAD*/

/*
    singleton functions
*/

template <typename T>
inline T& singleton(void)
{
    return Singleton<T>::instance();
}

#define NX_SINGLETON_(n) \
template <typename T, NX_PP_TYPE_1(n, typename P)> \
inline T& singleton(NX_PP_TYPE_2(n, P, par)) \
{ \
    return Singleton<T>::instance(NX_PP_TYPE_1(n, par)); \
}
NX_PP_MULT_MAX(NX_SINGLETON_)
#undef NX_SINGLETON_

/*
    Get a singleton from a type
*/

template <typename Type_, typename T>
class SingletonProxy : public Singleton<T> {};

template <typename Type_, typename T>
inline T& singleton(void)
{
    return SingletonProxy<Type_, T>::instance();
}

#define NX_SINGLETON_(n) \
template <typename Type_, typename T, NX_PP_TYPE_1(n, typename P)> \
inline T& singleton(NX_PP_TYPE_2(n, P, par)) \
{ \
    return SingletonProxy<Type_, T>::instance(NX_PP_TYPE_1(n, par)); \
}
NX_PP_MULT_MAX(NX_SINGLETON_)
#undef NX_SINGLETON_

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
