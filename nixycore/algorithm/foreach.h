/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/algorithm/container.h"

#include "nixycore/typemanip/typetools.h"
#include "nixycore/typemanip/typetraits.h"
#include "nixycore/typemanip/typeof.h"

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

#ifdef NX_SP_CXX11_RANGEFOR
#define nx_foreach(var, col, ...) \
    for(auto var : col) \
    for(bool once__ = true; once__; once__ = false,##__VA_ARGS__)
#else
namespace private_foreach
{
    template <typename T> struct storage     { typedef const T & type_t; };
    template <typename T> struct storage<T&> { typedef       T & type_t; };

    template <typename T>
    struct helper
    {
        typedef typename traits<T>::type_t               type_t;
        typedef typename container_traits<type_t>::ite_t ite_t;
        typedef typename container_traits<type_t>::val_t val_t;
        typedef typename select_if<
                is_scalar<val_t>::value || is_reference<val_t>::value,
                val_t,
                typename rm_reference<val_t>::type_t&
        >::type_t deref_t;
        typedef typename storage<T>::type_t storage_t;

        T set_; ite_t cur_, end_;
        bool mark_;

        helper(storage_t s)
            : set_(s)
            , cur_(nx::begin(set_))
            , end_(nx::end(set_))
            , mark_(false)
        {}

        inline void next(void)      { ++cur_; }
        inline bool is_inside(void) { return (cur_ != end_); }
        inline bool go_next(void)   { return (mark_ = !mark_); }
    };

    template <typename T>
    inline helper<T&> contain(T & t)        // lvalue
    {
        return helper<T&>(t);
    }

    template <typename T>
    inline helper<T> contain(T const & t)   // rvalue
    {
        return helper<T>(t);
    }

    template <typename T>
    inline typename enable_if<is_numeric<typename helper<T>::ite_t>::value, 
    typename helper<T>::deref_t>::type_t deref(helper<T>& hp)
    {
        return hp.cur_;
    }

    template <typename T>
    inline typename enable_if<!is_numeric<typename helper<T>::ite_t>::value, 
    typename helper<T>::deref_t>::type_t deref(helper<T>& hp)
    {
        return *(hp.cur_);
    }
}

#define nx_foreach(var, col, ...) \
    for(nx_auto(hp__, nx::private_foreach::contain(col)); hp__.is_inside(); hp__.next(),##__VA_ARGS__) \
    for(nx_auto(var , nx::private_foreach::deref(hp__)) ; hp__.go_next();)

#endif/*NX_SP_CXX11_RANGEFOR*/

#define nx_forever(...) \
    for(__VA_ARGS__;;)

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
