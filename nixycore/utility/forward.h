/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#pragma once

#include "nixycore/utility/rvalue.h"

#include "nixycore/typemanip/typetraits.h"

#include "nixycore/general/general.h"

#if defined(NX_SP_CXX11_PERFECT_FWD)
#include <utility> // std::forward
#endif

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    Perfect Forwarding
*/

#ifdef NX_SP_CXX11_PERFECT_FWD

// Helper

#define nx_fref(...)        __VA_ARGS__ &&
#define nx_pass(...)        __VA_ARGS__
#define nx_forward(T, ...)  std::forward<T>(__VA_ARGS__)
#define nx_extract(T, ...)  nx::moved(std::forward<T>(__VA_ARGS__))

// Type manipulation

template <typename T> struct rm_fvalue { typedef T type_t; };

template <typename T>
struct extract
{
    typedef typename nx::rm_rvalue<
            typename nx::rm_cv_ref<T>::type_t
                                    >::type_t type_t;
};

#else /*NX_SP_CXX11_PERFECT_FWD*/

// Helper

template <typename T>
struct fvalue
{
    typedef T type_t;

    type_t content_;

    fvalue(type_t r)
        : content_(r)
    {}

    operator type_t () const
    {
        return content_;
    }
};

template <typename T>
struct fvalue<nx_rref(T)>
{
    typedef nx_rref(T) type_t;

    type_t content_;

    fvalue(type_t r)
        : content_(nx::move(r))
    {}

    operator type_t () const
    {
        return nx::move(content_);
    }
};

template <typename T> inline fvalue<T&>         pass(T& r)         { return fvalue<T&>        (r); }
template <typename T> inline fvalue<const T&>   pass(const T& r)   { return fvalue<const T&>  (r); }
template <typename T> inline fvalue<nx_rref(T)> pass(nx_rref(T) r) { return fvalue<nx_rref(T)>(nx::move(r)); }

template <typename T> inline T  forward(fvalue<T>& r) { return r; }
template <typename T> inline T& forward(T& r)         { return r; }

#define nx_fref(...)        __VA_ARGS__
#define nx_pass(...)        nx::pass(__VA_ARGS__)
#define nx_forward(T, ...)  __VA_ARGS__
#define nx_extract(T, ...)  nx::moved(nx::forward(__VA_ARGS__))

// Type manipulation

template <typename T> struct rm_fvalue             { typedef T type_t; };
template <typename T> struct rm_fvalue<fvalue<T> > { typedef T type_t; };

template <typename T>
struct extract
{
    typedef typename nx::rm_rvalue<typename nx::rm_cv_ref<
            typename nx::rm_fvalue<typename nx::rm_cv_ref<T
    >::type_t>::type_t
    >::type_t>::type_t type_t;
};

#endif/*NX_SP_CXX11_PERFECT_FWD*/

/*
    NX_PP_FORWARD(3, P, par)
    C++98 -->
    par1 , par2 , par3
    C++11 -->
    std::forward<P1>(par1) , std::forward<P2>(par2) , std::forward<P3>(par3)
*/

#ifdef NX_SP_CXX11_PERFECT_FWD
#define NX_PP_FREF(...)                 && __VA_ARGS__
#define NX_PP_FORWARD_1_(n, P, ...)     nx_forward(NX_PP_JOIN(P, n), NX_PP_JOIN(__VA_ARGS__, n))
#define NX_PP_FORWARD_2_(n, P, ...)     , NX_PP_FORWARD_1_(n, P, __VA_ARGS__)
#define NX_PP_FORWARD(n, P, ...)        NX_PP_REPEATEX(n, NX_PP_FORWARD_1_, NX_PP_FORWARD_2_, P, __VA_ARGS__)
#else /*NX_SP_CXX11_PERFECT_FWD*/
#define NX_PP_FREF(...)                 __VA_ARGS__
#define NX_PP_FORWARD(n, P, ...)        NX_PP_TYPE_1(n, __VA_ARGS__)
#endif/*NX_SP_CXX11_PERFECT_FWD*/
#define NX_PP_FORWARD_MAX(P, ...)       NX_PP_FORWARD(NX_PP_MAX, P, __VA_ARGS__)

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
