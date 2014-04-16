/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/typemanip/typedetect.h"

#include "nixycore/general/general.h"

#ifdef NX_SP_CXX11_TYPE_TRAITS
#include <type_traits>
#endif

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    Some functional features
*/

template <typename T>
struct traits
{
    typedef typename rm_reference<
            typename rm_cv<T>::type_t
    >::type_t type_t;

    typedef type_t*       point_t;
    typedef type_t&       refer_t;
    typedef const type_t* cpoint_t;
    typedef const type_t& crefer_t;

    typedef typename select_if<
            is_scalar<T>::value || is_reference<T>::value,
            T, 
            const typename rm_reference<
                  typename rm_const<T>::type_t>::type_t&
    >::type_t param_t;
};

/*
    Obtains the decay type of T
*/

#ifdef NX_SP_CXX11_TYPE_TRAITS
template <typename T>
struct decay
{
    typedef typename std::decay<T>::type type_t;
};
#else /*NX_SP_CXX11_TYPE_TRAITS*/
template <typename T>
struct decay
{
private:
    typedef typename rm_reference<T>::type_t no_ref_type;
public:
    typedef typename select_if<is_array<no_ref_type>::value,
            typename rm_array<no_ref_type>::type_t*,
            typename select_if<is_function<no_ref_type>::value && !is_pointer<no_ref_type>::value,
                no_ref_type*,
                typename rm_cv<no_ref_type>::type_t
            >::type_t
    >::type_t type_t;
};
#endif/*NX_SP_CXX11_TYPE_TRAITS*/

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
