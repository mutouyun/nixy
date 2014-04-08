/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/thread/interlocked.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/utility/utility.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    atomic storage
*/

namespace private_atomic
{
    /*
        remove unsigned
    */

    template <typename T> struct rm_unsigned         { typedef T     type_t; };
    template <>           struct rm_unsigned<uchar>  { typedef char  type_t; };
    template <>           struct rm_unsigned<ushort> { typedef short type_t; };
    template <>           struct rm_unsigned<uint>   { typedef int   type_t; };
    template <>           struct rm_unsigned<ulong>  { typedef long  type_t; };
    template <>           struct rm_unsigned<ullong> { typedef llong type_t; };

    /*
        check if the size of type is supported
    */

    template <int MaskN, int SizeN>
    struct is_supported
        : type_if<(MaskN & SizeN) ? true : false>
    {};

    /*
        get the supported size
    */

    template <int MaskN, int SizeN, bool = is_supported<MaskN, SizeN>::value>
    struct get_supported
    {
        NX_STATIC_VALUE(int, SizeN);
    };

    template <int MaskN>
    struct get_supported<MaskN, 0, false>
    {
        NX_STATIC_VALUE(int, 0);
    };

    template <int MaskN, int SizeN>
    struct get_supported<MaskN, SizeN, false>
    {
    private:
        NX_STATIC_PROPERTY(int, new_size, SizeN <= MaskN ? SizeN << 1 : 0);
    public:
        NX_STATIC_VALUE(int, get_supported<MaskN, new_size>::value);
    };

    /*
        get the supported storage type
    */

    template <typename T, int MaskN, int SpN = get_supported<MaskN, sizeof(T)>::value
                                   , bool = (sizeof(T) == SpN)>
    struct get_storage;

    template <typename T, int MaskN, int SpN>
    struct get_storage<T, MaskN, SpN, true>
    {
        typedef T type_t;
    };
    /*
        (SpN == 1) && (sizeof(T) != 1)
        Means we cann't get a comfortable size for this type.
        At this time, SpN must be zero.
        So when (sizeof(T) != 1), SpN will never equal to 1
    */
    template <typename T, int MaskN>
    struct get_storage<T, MaskN, 2, false>
    {
        typedef typename nx::select_if
                <nx::is_signed<T>::value, nx::sint16, nx::uint16
                >::type_t type_t;
    };
    template <typename T, int MaskN>
    struct get_storage<T, MaskN, 4, false>
    {
        typedef typename nx::select_if
                <nx::is_signed<T>::value, nx::sint32, nx::uint32
                >::type_t type_t;
    };
    template <typename T, int MaskN>
    struct get_storage<T, MaskN, 8, false>
    {
        typedef typename nx::select_if
                <nx::is_signed<T>::value, nx::sint64, nx::uint64
                >::type_t type_t;
    };

    /*
        atomic storage detail
    */

    template <typename T, typename ModelT>
    struct storage
    {
        typedef typename get_storage<T, ModelT::supported_mask>::type_t type_t;
        typedef typename rm_unsigned<T>::type_t value_t;
    };
}

template <typename T, typename ModelT>
struct atomic_storage
    : private_atomic::storage<typename nx::rm_cv<T>::type_t, ModelT>
{};

/*
    atomic type
*/

namespace private_atomic
{
    /*
        atomic type base
    */

    template <typename T, typename ModelT>
    struct base
    {
        typedef ModelT model_t;
        typedef typename atomic_storage<T, model_t>::type_t type_t;
        typedef typename atomic_storage<T, model_t>::value_t value_t;

        T get(void) const                            { return reinterpret_cast<const T>(v_); }
        T get(void) const volatile                   { return reinterpret_cast<const T>(v_); }

        operator T(void) const                       { return model_t::template load<T>(v_); }
        operator T(void) const volatile              { return model_t::template load<T>(v_); }

        value_t operator=(value_t val)               { model_t::store(v_, val); return val; }
        value_t operator=(value_t val) volatile      { model_t::store(v_, val); return val; }

        bool cas(value_t comp, value_t val)          { return model_t::compare_exchange(v_, val, (type_t)comp); }
        bool cas(value_t comp, value_t val) volatile { return model_t::compare_exchange(v_, val, (type_t)comp); }

    protected:
        type_t v_;
    };

    /*
        atomic type detail
    */

    template <typename T, typename ModelT, bool = nx::is_integral<T>::value
                                         , bool = nx::is_pointer <T>::value>
    struct detail;

    template <typename ModelT>
    struct detail<bool, ModelT, true, false> : base<bool, ModelT>
    {
        typedef base<bool, ModelT> base_t;
        typedef typename base_t::value_t value_t;

        detail(void)        { base_t::operator=(0); }
        detail(value_t val) { base_t::operator=(val); }

        using base_t::operator=;
    };

    template <typename T, typename ModelT>
    struct detail<T, ModelT, true, false> : base<T, ModelT>
    {
        typedef base<T, ModelT> base_t;
        typedef typename base_t::value_t value_t;
        typedef typename base_t::model_t model_t;

        detail(void)        { base_t::operator=(0); }
        detail(value_t val) { base_t::operator=(val); }

        using base_t::operator=;

        T operator++(int)         { return model_t::fetch_add(base_t::v_, 1); }
        T operator--(int)         { return model_t::fetch_add(base_t::v_,-1); }
        T operator++()            { return model_t::fetch_add(base_t::v_, 1) + 1; }
        T operator--()            { return model_t::fetch_add(base_t::v_,-1) - 1; }
        T operator+=(value_t val) { return model_t::fetch_add(base_t::v_, val) + val; }
        T operator-=(value_t val) { return model_t::fetch_add(base_t::v_,-val) - val; }

        T operator++(int)         volatile { return model_t::fetch_add(base_t::v_, 1); }
        T operator--(int)         volatile { return model_t::fetch_add(base_t::v_,-1); }
        T operator++()            volatile { return model_t::fetch_add(base_t::v_, 1) + 1; }
        T operator--()            volatile { return model_t::fetch_add(base_t::v_,-1) - 1; }
        T operator+=(value_t val) volatile { return model_t::fetch_add(base_t::v_, val) + val; }
        T operator-=(value_t val) volatile { return model_t::fetch_add(base_t::v_,-val) - val; }
    };

    template <typename T, typename ModelT>
    struct detail<T, ModelT, false, true> : detail<T, ModelT, true, false>
    {
        typedef typename detail<T, ModelT, true, false>::base_t base_t;
        typedef typename base_t::value_t value_t;

        detail(void)        { base_t::operator=(0); }
        detail(value_t val) { base_t::operator=(val); }

        using base_t::operator=;

        T operator->(void) const          { return (*this); }
        T operator->(void) const volatile { return (*this); }

        typename rm_pointer<T>::type_t& operator*(void) const          { return *(T)(*this); }
        typename rm_pointer<T>::type_t& operator*(void) const volatile { return *(T)(*this); }
    };
}

#ifndef NX_DEFAULT_INTERLOCKED
#   ifdef NX_SINGLE_THREAD
#       define NX_DEFAULT_INTERLOCKED  nx::use::interlocked_st
#   else
#       define NX_DEFAULT_INTERLOCKED  nx::use::interlocked_mt
#   endif
#endif

template <typename T, typename ModelT = NX_DEFAULT_INTERLOCKED>
class atomic
    : public private_atomic::detail<T, ModelT>, nx::noncopyable
{
public:
    typedef private_atomic::detail<T, ModelT> base_t;
    typedef typename base_t::value_t value_t;

    atomic(void)        : base_t() {}
    atomic(value_t val) : base_t(val) {}

    using base_t::operator=;

private:
    /* no swap */
    void swap(atomic& r);
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
