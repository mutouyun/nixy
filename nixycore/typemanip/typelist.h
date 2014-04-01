/*
    The Nixy Library
    Code covered by the MIT License

    Modified from The Loki Library
    Modified by : mutouyun (http://darkc.at)

    Copyright (c) 2001 by Andrei Alexandrescu
*/

#pragma once

#include "nixycore/typemanip/typedefs.h"
#include "nixycore/typemanip/typetools.h"
#include "nixycore/typemanip/typerelation.h"

#include "nixycore/general/general.h"
#include "nixycore/preprocessor/preprocessor.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    Define type list
*/

template <typename T, typename U>
struct TypeList
{
    typedef T head_t;
    typedef U tail_t;
};

/*
    Get the types number of the typelist
*/

template <class ListT>
struct types_len;

template <>
struct types_len<null_t>
{
    NX_STATIC_VALUE(int, 0);
};

template <template <typename, typename> class L_>
struct types_len<L_<null_t, null_t> >
{
    NX_STATIC_VALUE(int, 0);
};

template <typename T, typename U, 
template <typename, typename> class L_>
struct types_len<L_<T, U> >
{
    NX_STATIC_VALUE(int, 1 + types_len<U>::value);
};

/*
    Get a type from the typelist with the index
*/

template <class ListT, int I>
struct types_at;

template <typename T, typename U, 
template <typename, typename> class L_>
struct types_at<L_<T, U>, 0>
{
    typedef T type_t;
};

template <typename T, typename U, int I, 
template <typename, typename> class L_>
struct types_at<L_<T, U>, I>
{
    typedef typename types_at<U, I - 1>::type_t type_t;
};

/*
    Find one type from the typelist
*/

template <class ListT, typename T>
struct types_find;

template <typename TypeT>
struct types_find<null_t, TypeT>
{
    NX_STATIC_VALUE(int, -1);
};

template <typename U, typename TypeT,
template <typename, typename> class L_>
struct types_find<L_<TypeT, U>, TypeT>
{
    NX_STATIC_VALUE(int, 0);
};

template <typename T, typename U, typename TypeT,
template <typename, typename> class L_>
struct types_find<L_<T, U>, TypeT>
{
private:
    NX_STATIC_PROPERTY(int, tmp, types_find<U, TypeT>::value);
public:
    NX_STATIC_VALUE(int, tmp == -1 ? -1 : 1 + tmp);
};

/*
    Check type is in the typelist
*/

template <class ListT, typename T>
struct types_exist : 
    type_if<types_find<ListT, T>::value != -1>
{};

/*
    Add one Type to a List
*/

template <class ListT, typename T>
struct types_add;

template <>
struct types_add<null_t, null_t>
{
    typedef null_t type_t;
};

template <typename T, typename U, 
template <typename, typename> class L_>
struct types_add<null_t, L_<T, U> >
{
    typedef L_<T, U> type_t;
};

template <
template <typename, typename> class L_>
struct types_add<L_<null_t, null_t>, null_t>
{
    typedef null_t type_t;
};

template <typename T, 
template <typename, typename> class L_>
struct types_add<L_<null_t, null_t>, T>
{
    typedef L_<T, null_t> type_t;
};

template <typename T, typename TypeT,
template <typename, typename> class L_>
struct types_add<L_<T, null_t>, TypeT>
{
    typedef L_<T, typename types_add<L_<null_t, null_t>, TypeT>::type_t> type_t;
};

template <typename T, typename U, typename TypeT,
template <typename, typename> class L_>
struct types_add<L_<T, U>, TypeT>
{
    typedef L_<T, typename types_add<U, TypeT>::type_t> type_t;
};

/*
    Delete a Type from a List
*/

template <class ListT, typename T>
struct types_del;

template <typename T>
struct types_del<null_t, T>
{
    typedef null_t type_t;
};

template <typename TypeT, typename U,
template <typename, typename> class L_>
struct types_del<L_<TypeT, U>, TypeT>
{
    typedef U type_t;
};

template <typename T, typename U, typename TypeT,
template <typename, typename> class L_>
struct types_del<L_<T, U>, TypeT>
{
    typedef L_<T, typename types_del<U, TypeT>::type_t> type_t;
};

/*
    Erase all this Type from a List
*/

template <class ListT, typename T>
struct types_erase;

template <typename T>
struct types_erase<null_t, T>
{
    typedef null_t type_t;
};

template <typename TypeT, typename U,
template <typename, typename> class L_>
struct types_erase<L_<TypeT, U>, TypeT>
{
    typedef typename types_erase<U, TypeT>::type_t type_t;
};

template <typename T, typename U, typename TypeT,
template <typename, typename> class L_>
struct types_erase<L_<T, U>, TypeT>
{
    typedef L_<T, typename types_erase<U, TypeT>::type_t> type_t;
};

/*
    Remove duplicate types, make a List more compact
*/

template <class ListT>
struct types_compact;

template <>
struct types_compact<null_t>
{
    typedef null_t type_t;
};

template <class T, class U, 
template <typename, typename> class L_>
struct types_compact<L_<T, U> >
{
private:
    typedef typename types_compact<U>::type_t L1;
    typedef typename types_del<L1, T>::type_t L2;
public:
    typedef L_<T, L2> type_t;
};

/*
    Replace a Type to another Type
*/

template <class ListT, typename T1, typename T2>
struct types_replace;

template <typename T1, typename T2>
struct types_replace<null_t, T1 , T2>
{
    typedef null_t type_t;
};

template <typename T1, typename U, typename T2, 
template <typename, typename> class L_>
struct types_replace<L_<T1, U>, T1 , T2>
{
    typedef L_<T2, U> type_t;
};

template <typename T, typename U, typename T1, typename T2, 
template <typename, typename> class L_>
struct types_replace<L_<T, U>, T1 , T2>
{
    typedef L_<T, typename types_replace<U, T1 , T2>::type_t> type_t;
};

/*
    Reverse the List
*/

template <class ListT>
struct types_reverse;

template <>
struct types_reverse<null_t>
{
    typedef null_t type_t;
};

template <typename T, typename U, 
template <typename, typename> class L_>
struct types_reverse<L_<T, U> >
{
    typedef typename types_add
                <typename types_reverse<U>::type_t,
                    T
                >::type_t type_t;
};

/*
    Get the most derived type of a List
*/

template <class ListT, typename T>
struct types_sub;

template <class T>
struct types_sub<null_t, T>
{
    typedef T type_t;
};

template <typename T, typename U, typename TypeT,
template <typename, typename> class L_>
struct types_sub<L_<T, U>, TypeT>
{
private:
    typedef typename types_sub<U, TypeT>::type_t tmp;
public:
    typedef typename select_if
            <is_supersub<tmp, T>::value, T, tmp
            >::type_t type_t;
};

/*
    Arranges the types in a typelist so that the most derived types appear first
*/

template <class ListT>
struct types_sort;

template <>
struct types_sort<null_t>
{
    typedef null_t type_t;
};

template <typename T, typename U, 
template <typename, typename> class L_>
struct types_sort<L_<T, U> >
{
private:
    typedef typename types_sub<U, T>::type_t                sub_type;
    typedef typename types_replace<U, sub_type, T>::type_t  rep_type;
    typedef typename types_sort<rep_type>::type_t           srt_type;
public:
    typedef L_<sub_type, srt_type> type_t;
};

/*
    Join types
*/

template <typename T, typename U>
struct types_join
{
    typedef TypeList<T, TypeList<U, null_t> > type_t;
};

template <>
struct types_join<null_t, null_t>
{
    typedef null_t type_t;
};

template <typename T>
struct types_join<T, null_t>
{
    typedef TypeList<T, null_t> type_t;
};

template <typename U>
struct types_join<null_t, U>
{
    typedef TypeList<U, null_t> type_t;
};

template <typename T, typename U, 
template <typename, typename> class L_>
struct types_join<L_<T, U>, null_t>
{
    typedef L_<T, U> type_t;
};

template <typename T, typename U, typename TypeT,
template <typename, typename> class L_>
struct types_join<L_<T, U>, TypeT>
{
    typedef typename types_add<L_<T, U>, TypeT>::type_t type_t;
};

template <typename T, typename U, 
template <typename, typename> class L_>
struct types_join<null_t, L_<T, U> >
{
    typedef typename types_add<null_t, L_<T, U> >::type_t type_t;
};

template <typename T, typename U, typename TypeT,
template <typename, typename> class L_>
struct types_join<TypeT, L_<T, U> >
{
    typedef L_<TypeT, L_<T, U> > type_t;
};

template <typename T1, typename U1, typename T2, typename U2, 
template <typename, typename> class L1_, 
template <typename, typename> class L2_>
struct types_join<L1_<T1, U1>, L2_<T2, U2> >
{
    typedef typename types_join<L1_<T1, U1>, T2>::type_t append_type;
    typedef typename types_join<append_type, U2>::type_t type_t;
};

/*
    Make types to a Sequence
*/

template <NX_PP_TYPE_MAX_1(typename T, = nx::null_t)>
struct types
{
private:
    typedef typename types<NX_PP_B1(NX_PP_TYPE_MAX_1(T))>::type_t tail_type;
public:
    typedef typename types_join<T1, tail_type>::type_t type_t;
};

template <>
struct types<>
{
    typedef null_t type_t;
};

/*
    Helper for special list algorithms
*/

#define NX_SPECIAL_TYPES_0(name, spl_class, ...) \
    template <NX_PP_PARAM(T, __VA_ARGS__)> \
    struct name<spl_class<NX_PP_TYPE_1(NX_PP_COUNT(__VA_ARGS__), T)> > \
         : name<typename spl_class<NX_PP_TYPE_1(NX_PP_COUNT(__VA_ARGS__), T)>::base_t> \
    {};

#define NX_SPECIAL_TYPES_1(name, typename1, spl_class, ...) \
    template <NX_PP_PARAM(T, __VA_ARGS__), typename1 TypeT> \
    struct name<spl_class<NX_PP_TYPE_1(NX_PP_COUNT(__VA_ARGS__), T)>, TypeT> \
         : name<typename spl_class<NX_PP_TYPE_1(NX_PP_COUNT(__VA_ARGS__), T)>::base_t, TypeT> \
    {};

#define NX_SPECIAL_TYPES_2(name, typename1, typename2, spl_class, ...) \
    template <NX_PP_PARAM(T, __VA_ARGS__), typename1 Type1_, typename2 Type2_> \
    struct name<spl_class<NX_PP_TYPE_1(NX_PP_COUNT(__VA_ARGS__), T)>, Type1_, Type2_> \
         : name<typename spl_class<NX_PP_TYPE_1(NX_PP_COUNT(__VA_ARGS__), T)>::base_t, Type1_, Type2_> \
    {};

#define NX_SPECIAL_TYPES_R(name, typename1, spl_class, ...) \
    template <typename1 TypeT, NX_PP_PARAM(T, __VA_ARGS__)> \
    struct name<TypeT, spl_class<NX_PP_TYPE_1(NX_PP_COUNT(__VA_ARGS__), T)> > \
         : name<TypeT, typename spl_class<NX_PP_TYPE_1(NX_PP_COUNT(__VA_ARGS__), T)>::base_t> \
    {};

#define NX_SPECIAL_TYPES_D(name, spl_class, ...) \
    template <NX_PP_PARAM(T, __VA_ARGS__), \
              NX_PP_PARAM(U, __VA_ARGS__)> \
    struct name<spl_class<NX_PP_TYPE_1(NX_PP_COUNT(__VA_ARGS__), T)>, \
                spl_class<NX_PP_TYPE_1(NX_PP_COUNT(__VA_ARGS__), U)> > \
         : name<typename spl_class<NX_PP_TYPE_1(NX_PP_COUNT(__VA_ARGS__), T)>::base_t, \
                typename spl_class<NX_PP_TYPE_1(NX_PP_COUNT(__VA_ARGS__), U)>::base_t> \
    {};

#define NX_SPECIAL_TYPES(spl_class, ...) \
    NX_SPECIAL_TYPES_0(types_len                        , spl_class, __VA_ARGS__) \
    NX_SPECIAL_TYPES_1(types_at     , int               , spl_class, __VA_ARGS__) \
    NX_SPECIAL_TYPES_1(types_find   , typename          , spl_class, __VA_ARGS__) \
    NX_SPECIAL_TYPES_1(types_add    , typename          , spl_class, __VA_ARGS__) \
    NX_SPECIAL_TYPES_1(types_del    , typename          , spl_class, __VA_ARGS__) \
    NX_SPECIAL_TYPES_1(types_erase  , typename          , spl_class, __VA_ARGS__) \
    NX_SPECIAL_TYPES_0(types_compact                    , spl_class, __VA_ARGS__) \
    NX_SPECIAL_TYPES_2(types_replace, typename, typename, spl_class, __VA_ARGS__) \
    NX_SPECIAL_TYPES_0(types_reverse                    , spl_class, __VA_ARGS__) \
    NX_SPECIAL_TYPES_1(types_sub    , typename          , spl_class, __VA_ARGS__) \
    NX_SPECIAL_TYPES_0(types_sort                       , spl_class, __VA_ARGS__)/* \
//  this may get an ambiguous class template instantiation
//  NX_SPECIAL_TYPES_1(types_join   , typename          , spl_class, __VA_ARGS__) \
//  NX_SPECIAL_TYPES_R(types_join   , typename          , spl_class, __VA_ARGS__) \
//  NX_SPECIAL_TYPES_D(types_join                       , spl_class, __VA_ARGS__)*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
