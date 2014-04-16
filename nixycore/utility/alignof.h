/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/general/general.h"

#ifdef NX_SP_CXX11_ALIGNMENT
#include <type_traits> // std::aligned_storage
#endif

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

#ifdef NX_SP_CXX11_ALIGNMENT

#define nx_alignof(...)     alignof(__VA_ARGS__)
#define nx_alignas(...)     alignas(__VA_ARGS__)

template <size_t LenN, size_t AlignN>
struct aligned_storage
{
    typedef typename std::aligned_storage<LenN, AlignN>::type type_t;
};

#else /*NX_SP_CXX11_ALIGNMENT*/

#if defined(NX_CC_MSVC)

#define nx_alignof(...)     __alignof(__VA_ARGS__)
#define nx_alignas(...)     __declspec(align(__VA_ARGS__))

template <size_t AlignN>
struct aligned_t;

/*
    __declspec(align(#))
    Valid entries are integer powers of two from 1 to 8192 (bytes).
    See: http://msdn.microsoft.com/en-us/library/83ythb65.aspx
*/
template <> struct nx_alignas(1)    aligned_t<1>    {};
template <> struct nx_alignas(2)    aligned_t<2>    {};
template <> struct nx_alignas(4)    aligned_t<4>    {};
template <> struct nx_alignas(8)    aligned_t<8>    {};
template <> struct nx_alignas(16)   aligned_t<16>   {};
template <> struct nx_alignas(32)   aligned_t<32>   {};
template <> struct nx_alignas(64)   aligned_t<64>   {};
template <> struct nx_alignas(128)  aligned_t<128>  {};
template <> struct nx_alignas(256)  aligned_t<256>  {};
template <> struct nx_alignas(512)  aligned_t<512>  {};
template <> struct nx_alignas(1024) aligned_t<1024> {};
template <> struct nx_alignas(2048) aligned_t<2048> {};
template <> struct nx_alignas(4096) aligned_t<4096> {};
template <> struct nx_alignas(8192) aligned_t<8192> {};

#elif defined(NX_CC_GNUC)

#define nx_alignof(...)     __alignof__(__VA_ARGS__)
#define nx_alignas(...)     __attribute__((__aligned__((__VA_ARGS__))))

template <size_t AlignN>
struct nx_alignas(AlignN) aligned_t {};

#else /*defined(NX_CC_)*/

/* It's just a demo, may couldn't work */

#define NX_OFFSETOF_(s, m)  (size_t)&(((s*)NULL)->m)
#define nx_alignof(...)     (NX_OFFSETOF_(struct { char c_; __VA_ARGS__ t_; }, t_))

template <size_t AlignN>
struct aligned_t;

template <> struct aligned_t<1> { nx::uint8  dunmmy_; };
template <> struct aligned_t<2> { nx::uint16 dunmmy_; };
template <> struct aligned_t<4> { nx::uint32 dunmmy_; };
template <> struct aligned_t<8> { nx::uint64 dunmmy_; };

#endif/*defined(NX_CC_)*/

template <size_t LenN, size_t AlignN>
struct aligned_storage
{
    typedef union
    {
        nx::uchar data_[LenN];
        nx::aligned_t<AlignN> align_;
    } type_t;
};

#endif/*NX_SP_CXX11_ALIGNMENT*/

template <typename T, size_t AlignN = nx_alignof(T)>
struct aligned
{
    typedef typename aligned_storage<sizeof(T), AlignN>::type_t storage_t;
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
