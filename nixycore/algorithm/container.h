/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/utility/maxmin.h"

#include "nixycore/typemanip/typetools.h"
#include "nixycore/typemanip/typeconcept.h"
#include "nixycore/typemanip/typedetect.h"

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    detect container
*/

namespace private_is_container
{
    NX_CONCEPT(iterator, iterator) // has_iterator

    template <typename T>
    struct detail
        : Judge<has_iterator<T>::value || is_array<T>::value>
    {};

    template <>
    struct detail<null_t>
        : Judge<false>
    {};
}

template <typename T>
struct is_container
    : private_is_container::detail<typename rm_reference
                                  <typename rm_cv
                                  <T
    >::type_t
    >::type_t
    >
{};

/*
    container traits
*/

namespace private_container_traits
{
    /* Detail of TraitsContainer */
    template
    <
        typename T, 
        bool = is_array<T>::value, 
        bool = is_container<T>::value, 
        bool = is_pod<T>::value
    >
    struct detail;

    template <typename T, bool IsContainer_, bool IsPod_>
    struct detail<T, true, IsContainer_, IsPod_>
    {
        typedef typename rm_array<T>::type_t  val_t;
        typedef typename rm_array<T>::type_t* ite_t;
    };

    template <typename T, bool IsPod_>
    struct detail<T, false, true, IsPod_>
    {
        typedef typename T::value_type val_t;
        typedef typename T::iterator   ite_t;
    };

    template <typename T>
    struct detail<T, false, false, true>
    {
        typedef T val_t;
        typedef T ite_t;
    };

    template <typename T>
    struct detail<T, false, false, false>
    {
        typedef T  val_t;
        typedef T* ite_t;
    };

    template <bool IsArray_, bool IsContainer_, bool IsPod_>
    struct detail<null_t, IsArray_, IsContainer_, IsPod_>
    {};
}

template <typename T>
struct container_traits
    : private_container_traits::detail<typename rm_cv<T>::type_t>
{};

/*
    Get the begin iterator
*/

namespace private_begin
{
    template <typename T>
    inline typename enable_if<is_array<T>::value
    >::type_t detail(typename container_traits<T>::ite_t& ite, T& set)
    { ite = (typename container_traits<T>::ite_t)set; }

    template <typename T>
    inline typename enable_if<!is_array<T>::value && 
                               is_pod<T>::value
    >::type_t detail(typename container_traits<T>::ite_t& ite, T& set)
    { ite = nx_min((T)0, set); }

    template <typename T>
    inline typename enable_if<!is_array<T>::value && 
                              !is_pod<T>::value && 
                               is_container<T>::value
    >::type_t detail(typename container_traits<T>::ite_t& ite, T& set)
    { ite = set.begin(); }
}

template <typename T>
inline typename container_traits<T>::ite_t begin(const T& set)
{
    typename container_traits<T>::ite_t ite;
    private_begin::detail(ite, const_cast<T&>(set));
    return ite;
}

/*
    Get the end iterator
*/

namespace private_end
{
    template <typename T>
    inline typename enable_if<is_array<T>::value
    >::type_t detail(typename container_traits<T>::ite_t& ite, T& set)
    { ite = (typename container_traits<T>::ite_t)*((&set) + 1); }

    template <typename T>
    inline typename enable_if<!is_array<T>::value && 
                               is_pod<T>::value
    >::type_t detail(typename container_traits<T>::ite_t& ite, T& set)
    { ite = nx_max((T)0, set); }

    template <typename T>
    inline typename enable_if<!is_array<T>::value && 
                              !is_pod<T>::value && 
                               is_container<T>::value
    >::type_t detail(typename container_traits<T>::ite_t& ite, T& set)
    { ite = set.end(); }
}

template <typename T>
inline typename container_traits<T>::ite_t end(const T& set)
{
    typename container_traits<T>::ite_t ite;
    private_end::detail(ite, const_cast<T&>(set));
    return ite;
}

/*
    insert
*/

namespace private_insert
{
    template <typename V, typename T>
    inline typename enable_if<is_array<T>::value
    >::type_t detail(T& /*set*/, typename container_traits<T>::ite_t ite, const V& val)
    { (*ite) = val; }

    template <typename V, typename T>
    inline typename enable_if<!is_array<T>::value && 
                               is_pod<T>::value
    >::type_t detail(T& set, typename container_traits<T>::ite_t /*ite*/, const V& val)
    { set = val; }

    template <typename V, typename T>
    inline typename enable_if<!is_array<T>::value && 
                              !is_pod<T>::value && 
                               is_container<T>::value
    >::type_t detail(T& set, typename container_traits<T>::ite_t ite, const V& val)
    { set.insert(ite, val); }
}

template <typename V, typename T>
inline void insert(T& set, typename container_traits<T>::ite_t ite, const V& val)
{
    private_insert::detail(set, ite, val);
}

/*
    erase
*/

namespace private_erase
{
    template <typename T>
    inline typename enable_if<is_array<T>::value
    >::type_t detail(T& /*set*/, typename container_traits<T>::ite_t /*ite*/)
    { /* Do nothing */ }

    template <typename T>
    inline typename enable_if<!is_array<T>::value && 
                               is_pod<T>::value
    >::type_t detail(T& /*set*/, typename container_traits<T>::ite_t /*ite*/)
    { /* Do nothing */ }

    template <typename T>
    inline typename enable_if<!is_array<T>::value && 
                              !is_pod<T>::value && 
                               is_container<T>::value
    >::type_t detail(T& set, typename container_traits<T>::ite_t ite)
    { set.erase(ite); }
}

template <typename V, typename T>
inline void erase(T& set, typename container_traits<T>::ite_t ite)
{
    private_erase::detail(set, ite);
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
