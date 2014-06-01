/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/typemanip/typebehavior.h"

#include "nixycore/general/general.h"

#ifdef NX_SP_CXX11_TYPE_TRAITS
#include <type_traits>
#endif

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <typename T>
class refer
{
public:
    typedef typename rm_const<
            typename rm_reference<T>::type_t
                                   >::type_t type_t;
    typedef typename rm_reference<T>::type_t value_t;

protected:
    value_t* content_;

public:
    refer(value_t& r)
        : content_(&r)
    {}

    operator value_t& () const           { return *content_; }

    value_t&      operator* (void)       { return *content_; }
    const type_t& operator* (void) const { return *content_; }
    value_t*      operator& (void)       { return  content_; }
    const type_t* operator& (void) const { return  content_; }
    value_t*      operator->(void)       { return  content_; }
    const type_t* operator->(void) const { return  content_; }
};

/*
    For get a reference of a variable
*/

template <typename T>
inline refer<T>         ref(T& r)               { return refer<T>(r); }
template <typename T>
inline refer<const T>   ref(const T& r)         { return refer<const T>(r); }
template <typename T>
inline refer<T>&        ref(refer<T>& r)        { return r; }
template <typename T>
inline const refer<T>&  ref(const refer<T>& r)  { return r; }
#ifdef NX_SP_CXX11_RVALUE_REF
template <typename T>
inline refer<T>&&       ref(refer<T>&& r)       { return static_cast<refer<T>&&>(r); }
template <typename T>
inline const refer<T>&& ref(const refer<T>&& r) { return static_cast<const refer<T>&&>(r); }
#endif/*NX_SP_CXX11_RVALUE_REF*/

template <typename T>
inline T&                               unref(T& r)              { return r; }
template <typename T>
inline const T&                         unref(const T& r)        { return r; }
template <typename T>
inline typename refer<T>::value_t&      unref(refer<T>& r)       { return (*r); }
template <typename T>
inline const typename refer<T>::type_t& unref(const refer<T>& r) { return (*r); }

/*
    Unwrap a reference wrapper
*/

namespace private_unrefwrap
{
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

    template <typename T>
    struct detail
    {
        typedef T type_t;
    };

    template <typename T>
    struct detail<refer<T> >
    {
        typedef T& type_t;
    };
}

template <typename T>
struct unrefwrap
    : private_unrefwrap::detail<typename private_unrefwrap::decay<T>::type_t>
{};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
