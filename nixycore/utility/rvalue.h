/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

#pragma once

#include "nixycore/typemanip/typetools.h"
#include "nixycore/typemanip/typedetect.h"
#include "nixycore/typemanip/typetraits.h"

#include "nixycore/bugfix/noexcept.h"

#include "nixycore/general/general.h"

#if defined(NX_SP_CXX11_RVALUE_REF)
#include <utility> // std::move
#endif

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    Rvalue && rvalue references
*/

#ifdef NX_SP_CXX11_RVALUE_REF

// Helper

#define nx_rval(T, ...) T
#define nx_rref(T, ...) T &&

// Move

using std::move;

// Moved

template <typename T>
inline T& moved(nx_rref(T) rv) nx_noexcept
{
    return rv;
}

template <typename T>
inline const T& moved(const T& rv) nx_noexcept
{
    return rv;
}

// Type cast

template <typename T, typename U>
inline nx_rref(T) move_cast(nx_rref(U) rv) nx_noexcept
{
    return nx::move(static_cast<T&>(nx::moved(rv)));
}

// Type manipulation

template <typename T>             struct is_rvalue : type_if<false> {};
template <typename T>             struct rm_rvalue { typedef T type_t; };
template <typename T, typename R> struct cp_rvalue { typedef R type_t; };

#else /*NX_SP_CXX11_RVALUE_REF*/

// Helper

template <typename T, bool = is_class<T>::value>
class rvalue : public nx::rm_cv_ref<T>::type_t
{
    typedef typename nx::rm_cv_ref<T>::type_t base_t;
    typedef base_t value_t;

public:
    typedef rvalue self_t;

    rvalue(const rvalue& rv)
        : base_t(rv)
    {}
    rvalue(const value_t& rv)
        : base_t(static_cast<rvalue&>(const_cast<value_t&>(rv)))
    {}

    operator value_t&()             { return (*this); }
    operator const value_t&() const { return (*this); }
};

template <typename T>
class rvalue<T, false>
{
    typedef typename rm_const<
            typename rm_reference<T>::type_t
                                   >::type_t value_t;

    value_t content_;

public:
    typedef value_t self_t;

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

// Move

template <typename T>
inline typename enable_if<is_class<T>::value,
nx_rref(T)>::type_t move(const T& rv) nx_noexcept
{
    return static_cast<nx_rref(T)>(rv);
}

template <typename T>
inline typename enable_if<!is_class<T>::value,
const T&>::type_t move(const T& rv) nx_noexcept
{
    return rv;
}

template <typename T>
inline nx_rref(T) move(nx_rref(T) rv) nx_noexcept
{
    return rv;
}

template <typename T>
inline nx_rref(T) move(nx_rval(T)& rv) nx_noexcept
{
    return const_cast<nx_rref(T)>(rv);
}

// Moved

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

template <typename T>
inline const T& moved(const T& rv) nx_noexcept
{
    return rv;
}

template <typename T>
inline T& moved(T& rv) nx_noexcept
{
    return rv;
}

// Type cast

template <typename T, typename U>
inline typename enable_if<is_class<U>::value,
nx_rref(T)>::type_t move_cast(nx_rref(U) rv) nx_noexcept
{
    return nx::move(static_cast<T&>(nx::moved(rv)));
}

template <typename T, typename U>
inline typename enable_if<!is_class<U>::value,
T>::type_t move_cast(nx_rref(U) rv) nx_noexcept
{
    return nx::moved(rv);
}

template <typename T, typename U>
inline typename enable_if<is_class<T>::value,
nx_rref(T)>::type_t move_cast(const U& rv) nx_noexcept
{
    return nx::move(static_cast<const T&>(rv));
}

template <typename T, typename U>
inline typename enable_if<!is_class<T>::value,
const T&>::type_t move_cast(const U& rv) nx_noexcept
{
    return nx::move(static_cast<const T&>(rv));
}

// Type manipulation

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
NX_END
//////////////////////////////////////////////////////////////////////////
