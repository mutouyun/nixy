/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/utility/refer.h"

#include "nixycore/typemanip/typetools.h"
#include "nixycore/typemanip/typedetect.h"
#include "nixycore/typemanip/typetraits.h"

#include "nixycore/bugfix/noexcept.h"

#include "nixycore/general/general.h"

#ifdef NX_SP_CXX11_RVALUE_REF
#include <utility> // std::move, std::forward
#endif

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

#ifdef NX_SP_CXX11_RVALUE_REF

#define nx_rval(T, ...) T
#define nx_rref(T, ...) T &&

using std::move;

template <typename T>
inline T& moved(nx_rref(T) rv) nx_noexcept
{
    return rv;
}

template <typename T>             struct is_rvalue : type_if<false> {};
template <typename T>             struct rm_rvalue { typedef T type_t; };
template <typename T, typename R> struct cp_rvalue { typedef R type_t; };

#else/*NX_SP_CXX11_RVALUE_REF*/

/*
    rvalue
    -->
    MyClass::MyClass(const nx::rvalue<MyClass>& rv);
    MyClass& MyClass::operator=(const nx::rvalue<MyClass>& rv);
*/

template <typename T, bool = is_class<T>::value>
class rvalue : public nx::traits<T>::type_t
{
    typedef typename nx::traits<T>::type_t base_t;
    typedef base_t value_t;

public:
    rvalue(const rvalue& rv)
        : base_t(rv)
    {}
    rvalue(const value_t& rv)
        : base_t(static_cast<rvalue&>(const_cast<value_t&>(rv)))
    {}
};

template <typename T>
class rvalue<T, false>
{
    typedef typename rm_const<
            typename rm_reference<T>::type_t
                                   >::type_t value_t;

    value_t content_;

public:
    rvalue(const rvalue& rv)
        : content_(rv.content_)
    {}
    rvalue(const value_t& rv)
        : content_(rv)
    {}

    operator value_t() const { return content_; }
};

#define nx_rval(T, ...) nx::rvalue<T,##__VA_ARGS__>
#define nx_rref(T, ...) const nx::rvalue<T,##__VA_ARGS__>&

/*
    Move
*/

template <typename T>
inline typename enable_if<is_class<T>::value,
nx_rref(T)>::type_t move(const T& rv) nx_noexcept
{
    return static_cast<const nx::rvalue<T>&>(rv);
}

template <typename T>
inline typename enable_if<!is_class<T>::value,
const T&>::type_t move(const T& rv) nx_noexcept
{
    return rv;
}

template <typename T>
inline typename enable_if<is_class<T>::value,
T&>::type_t moved(nx_rref(T) rv) nx_noexcept
{
    return const_cast<T&>(static_cast<const T&>(rv));
}

template <typename T>
inline typename enable_if<!is_class<T>::value,
T>::type_t moved(nx_rref(T) rv) nx_noexcept
{
    return rv;
}

/*
    type detected
*/

template <typename T>             struct is_rvalue_reference<nx_rref(T)>    : type_if<true> {};
template <typename T>             struct rm_rvalue_reference<nx_rref(T)>    { typedef T type_t; };
template <typename T, typename R> struct cp_rvalue_reference<nx_rref(T), R> { typedef nx_rref(R) type_t; };

template <typename T>             struct is_rvalue                          : type_if<false> {};
template <typename T>             struct is_rvalue<nx_rval(T) >             : type_if<true>  {};
template <typename T>             struct rm_rvalue                          { typedef T type_t; };
template <typename T>             struct rm_rvalue<nx_rval(T) >             { typedef T type_t; };
template <typename T, typename R> struct cp_rvalue                          { typedef R type_t; };
template <typename T, typename R> struct cp_rvalue<nx_rval(T), R>           { typedef nx_rval(R) type_t; };

#endif/*NX_SP_CXX11_RVALUE_REF*/

//////////////////////////////////////////////////////////////////////////

/*
    Perfect Forwarding
*/

#ifdef NX_SP_CXX11_RVALUE_REF
#define nx_fref(T, ...) T && __VA_ARGS__
#define nx_fval(T, ...) std::forward<T>(__VA_ARGS__)
#define nx_fpar(...)    __VA_ARGS__
#else/*NX_SP_CXX11_RVALUE_REF*/
#define nx_fref(T, ...) T __VA_ARGS__
#define nx_fval(T, ...) nx::unref(__VA_ARGS__)
#define nx_fpar(...)    nx::ref(__VA_ARGS__)
#endif/*NX_SP_CXX11_RVALUE_REF*/

/*
    NX_PP_FORWARD(3, P, par)
    C++98 -->
    par1 , par2 , par3
    C++11 -->
    std::forward<P1>(par1) , std::forward<P2>(par2) , std::forward<P3>(par3)
*/

#define NX_PP_FPAR(...)                 nx_fref(, __VA_ARGS__)
#ifdef NX_SP_CXX11_RVALUE_REF
#define NX_PP_FORWARD_1_(n, P, ...)     nx_fval(NX_PP_JOIN(P, n), NX_PP_JOIN(__VA_ARGS__, n))
#define NX_PP_FORWARD_2_(n, P, ...)     , NX_PP_FORWARD_1_(n, P, __VA_ARGS__)
#define NX_PP_FORWARD(n, P, ...)        NX_PP_REPEATEX(n, NX_PP_FORWARD_1_, NX_PP_FORWARD_2_, P, __VA_ARGS__)
#else/*NX_SP_CXX11_RVALUE_REF*/
#define NX_PP_FORWARD(n, P, ...)        NX_PP_TYPE_1(n, __VA_ARGS__)
#endif/*NX_SP_CXX11_RVALUE_REF*/
#define NX_PP_FORWARD_MAX(P, ...)       NX_PP_FORWARD(NX_PP_MAX, P, __VA_ARGS__)

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
