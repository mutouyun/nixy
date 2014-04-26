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

#ifdef NX_SP_CXX11_ATOMIC
#include <atomic> // std::atomic ...
#endif

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
        typedef typename get_storage<T, ModelT::SUPPORTED_MASK>::type_t scalar_type;
        typedef typename ModelT::template atomic<scalar_type>::type_t type_t;
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

#if defined(NX_CC_MSVC)
#   pragma warning(push)            // <MSVC 2005>
#   pragma warning(disable: 4800)   // forcing value to bool 'true' or 'false' (performance warning)
#endif

    template <typename T, typename ModelT>
    struct base
    {
        typedef ModelT model_t;
        typedef typename atomic_storage<T, model_t>::type_t type_t;
        typedef typename atomic_storage<T, model_t>::scalar_type value_t;
        typedef typename rm_unsigned<value_t>::type_t svalue_t;

        base(value_t val) : v_(val) {}
        T load(void) const                                { return model_t::load(v_); }
        T load(void) const volatile                       { return model_t::load(v_); }
        template <typename M>
        T load(M m) const                                 { return model_t::load(v_, m); }
        template <typename M>
        T load(M m) const volatile                        { return model_t::load(v_, m); }

        void store(value_t val)                           { model_t::store(v_, val); }
        void store(value_t val) volatile                  { model_t::store(v_, val); }
        template <typename M>
        void store(value_t val, M m)                      { model_t::store(v_, val, m); }
        template <typename M>
        void store(value_t val, M m) volatile             { model_t::store(v_, val, m); }

        T fetch_add(value_t val)                          { return model_t::fetch_add(v_, val); }
        T fetch_add(value_t val) volatile                 { return model_t::fetch_add(v_, val); }
        template <typename M>
        T fetch_add(value_t val, M m)                     { return model_t::fetch_add(v_, val, m); }
        template <typename M>
        T fetch_add(value_t val, M m) volatile            { return model_t::fetch_add(v_, val, m); }

        T fetch_sub(value_t val)                          { return model_t::fetch_add(v_, -static_cast<svalue_t>(val)); }
        T fetch_sub(value_t val) volatile                 { return model_t::fetch_add(v_, -static_cast<svalue_t>(val)); }
        template <typename M>
        T fetch_sub(value_t val, M m)                     { return model_t::fetch_add(v_, -static_cast<svalue_t>(val), m); }
        template <typename M>
        T fetch_sub(value_t val, M m) volatile            { return model_t::fetch_add(v_, -static_cast<svalue_t>(val), m); }

        T exchange(value_t val)                           { return model_t::exchange(v_, val); }
        T exchange(value_t val) volatile                  { return model_t::exchange(v_, val); }
        template <typename M>
        T exchange(value_t val, M m)                      { return model_t::exchange(v_, val, m); }
        template <typename M>
        T exchange(value_t val, M m) volatile             { return model_t::exchange(v_, val, m); }

        bool cas(value_t comp, value_t val)               { return model_t::compare_exchange(v_, val, (value_t)comp); }
        bool cas(value_t comp, value_t val) volatile      { return model_t::compare_exchange(v_, val, (value_t)comp); }
        template <typename M>
        bool cas(value_t comp, value_t val, M m)          { return model_t::compare_exchange(v_, val, (value_t)comp, m); }
        template <typename M>
        bool cas(value_t comp, value_t val, M m) volatile { return model_t::compare_exchange(v_, val, (value_t)comp, m); }

        operator T(void) const                            { return load(); }
        operator T(void) const volatile                   { return load(); }

        value_t operator=(value_t val)                    { store(val); return val; }
        value_t operator=(value_t val) volatile           { store(val); return val; }

    private:
        type_t v_;
    };

#if defined(NX_CC_MSVC)
#   pragma warning(pop) // <MSVC 2005>
#endif

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

        detail(void)        : base_t(false) {}
        detail(value_t val) : base_t(val)   {}

        using base_t::operator=;
    };

    template <typename T, typename ModelT>
    struct detail<T, ModelT, true, false> : base<T, ModelT>
    {
        typedef base<T, ModelT> base_t;
        typedef typename base_t::value_t value_t;

        detail(void)        : base_t(0)   {}
        detail(value_t val) : base_t(val) {}

        using base_t::operator=;

        T operator++(int)                  { return base_t::fetch_add(1); }
        T operator--(int)                  { return base_t::fetch_sub(1); }
        T operator++(void)                 { return base_t::fetch_add(1) + 1; }
        T operator--(void)                 { return base_t::fetch_sub(1) - 1; }
        T operator+=(value_t val)          { return base_t::fetch_add(val) + val; }
        T operator-=(value_t val)          { return base_t::fetch_sub(val) - val; }

        T operator++(int)         volatile { return base_t::fetch_add(1); }
        T operator--(int)         volatile { return base_t::fetch_sub(1); }
        T operator++(void)        volatile { return base_t::fetch_add(1) + 1; }
        T operator--(void)        volatile { return base_t::fetch_sub(1) - 1; }
        T operator+=(value_t val) volatile { return base_t::fetch_add(val) + val; }
        T operator-=(value_t val) volatile { return base_t::fetch_sub(val) - val; }
    };

    template <typename T, typename ModelT>
    struct detail<T, ModelT, false, true> : detail<T, ModelT, true, false>
    {
        typedef detail<T, ModelT, true, false> base_t;
        typedef typename base_t::value_t value_t;

        detail(void)        : base_t(NULL) {}
        detail(value_t val) : base_t(val)  {}

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

    atomic(void)        : base_t()    {}
    atomic(value_t val) : base_t(val) {}

#ifdef NX_CC_GNUC
    /*
        <GNUC> compiler error:
            'nx::noncopyable::noncopyable(const nx::noncopyable&)' is private
        or
            use of deleted function 'nx::atomic<T>::atomic(nx::atomic<T>&&)'
        When using like this: atomic<int> xx = 10
    */
    atomic(const atomic&)
        : base_t()
        , nx::noncopyable()
    { nx_assert(false); } // = deleted
#endif

    using base_t::operator=;
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
