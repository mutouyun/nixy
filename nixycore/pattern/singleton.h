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

#if defined(NX_CC_MSVC) && (NX_CC_MSVC == 1800)
/*
    type_traits(1509): fatal error C1001
    With: NX_SINGLETON_( (nx_forward(P, par)...) );
*/
#   undef NX_SP_CXX11_TEMPLATES
#endif

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

#ifdef NX_SP_CXX11_TEMPLATES
    template <typename... P>
    static T& instance(nx_fref(P, ... par))
    {
        NX_SINGLETON_( (nx_forward(P, par)...) );
    }
#else /*NX_SP_CXX11_TEMPLATES*/
#define NX_INSTANCE_(n) \
    template <NX_PP_TYPE_1(n, typename P)> \
    static T& instance(NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
    { \
        NX_SINGLETON_( (NX_PP_FORWARD(n, P, par)) ); \
    }
    NX_PP_MULT_MAX(NX_INSTANCE_)
#undef NX_INSTANCE_
#undef NX_SINGLETON_
#endif/*NX_SP_CXX11_TEMPLATES*/
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

#ifdef NX_SP_CXX11_TEMPLATES
template <typename T, typename... P>
inline T& singleton(nx_fref(P, ... par))
{
    return Singleton<T>::instance(nx_forward(P, par)...);
}
#else /*NX_SP_CXX11_TEMPLATES*/
template <typename T>
inline T& singleton(void)
{
    return Singleton<T>::instance();
}

#define NX_SINGLETON_(n) \
template <typename T, NX_PP_TYPE_1(n, typename P)> \
inline T& singleton(NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
{ \
    return Singleton<T>::instance(NX_PP_FORWARD(n, P, par)); \
}
NX_PP_MULT_MAX(NX_SINGLETON_)
#undef NX_SINGLETON_
#endif/*NX_SP_CXX11_TEMPLATES*/

/*
    Get a singleton from a type
*/

template <typename TypeT, typename T>
class SingletonProxy : public Singleton<T> {};

#ifdef NX_SP_CXX11_TEMPLATES
template <typename TypeT, typename T, typename... P>
inline T& singleton(nx_fref(P, ... par))
{
    return SingletonProxy<TypeT, T>::instance(nx_forward(P, par)...);
}
#else /*NX_SP_CXX11_TEMPLATES*/
template <typename TypeT, typename T>
inline T& singleton(void)
{
    return SingletonProxy<TypeT, T>::instance();
}

#define NX_SINGLETON_(n) \
template <typename TypeT, typename T, NX_PP_TYPE_1(n, typename P)> \
inline T& singleton(NX_PP_TYPE_2(n, P, NX_PP_FREF(par))) \
{ \
    return SingletonProxy<TypeT, T>::instance(NX_PP_FORWARD(n, P, par)); \
}
NX_PP_MULT_MAX(NX_SINGLETON_)
#undef NX_SINGLETON_
#endif/*NX_SP_CXX11_TEMPLATES*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////

#if defined(NX_CC_MSVC) && (NX_CC_MSVC == 1800)
#   define NX_SP_CXX11_TEMPLATES
#endif
