/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

//////////////////////////////////////////////////////////////////////////

#define NX_INTERLOCKED_NOASM
#if defined(NX_PC_X86_32) && (NX_CC_MSVC < 1600)
#   undef NX_INTERLOCKED_NOASM
#endif

#if defined(NX_OS_WINCE) && (NX_OS_WINCE >= 0x600)

extern "C" long __cdecl _InterlockedExchangeAdd(long volatile *, long);
extern "C" long __cdecl _InterlockedExchange(long volatile *, long);
extern "C" long __cdecl _InterlockedCompareExchange(long volatile *, long, long);

#define NX_INTERLOCKED_EXCHANGEADD(a, b)            _InterlockedExchangeAdd((long volatile *)&(a), (long)(b))
#define NX_INTERLOCKED_EXCHANGE(a, b)               _InterlockedExchange((long volatile *)&(a), (long)(b))
#define NX_INTERLOCKED_COMPAREEXCHANGE(a, b, c)     _InterlockedCompareExchange((long volatile *)&(a), (long)(b), (long)(c))

#elif defined(NX_OS_WINCE)

extern "C" long __cdecl InterlockedExchangeAdd(long *, long);
extern "C" long __cdecl InterlockedExchange(long *, long);
extern "C" long __cdecl InterlockedCompareExchange(long *, long, long);

#define NX_INTERLOCKED_EXCHANGEADD(a, b)            InterlockedExchangeAdd((long *)&(a), (long)(b))
#define NX_INTERLOCKED_EXCHANGE(a, b)               InterlockedExchange((long *)&(a), (long)(b))
#define NX_INTERLOCKED_COMPAREEXCHANGE(a, b, c)     InterlockedCompareExchange((long *)&(a), (long)(b), (long)(c))

#else /*NX_OS_WINCE*/

#include <intrin.h>

#pragma intrinsic(_InterlockedExchangeAdd)
#pragma intrinsic(_InterlockedExchange)
#pragma intrinsic(_InterlockedCompareExchange)

#define NX_INTERLOCKED_EXCHANGEADD(a, b)            _InterlockedExchangeAdd((long volatile *)&(a), (long)(b))
#define NX_INTERLOCKED_EXCHANGE(a, b)               _InterlockedExchange((long volatile *)&(a), (long)(b))
#define NX_INTERLOCKED_COMPAREEXCHANGE(a, b, c)     _InterlockedCompareExchange((long volatile *)&(a), (long)(b), (long)(c))

#ifdef NX_INTERLOCKED_NOASM
#   if (NX_CC_MSVC >= 1600)
#   pragma intrinsic(_InterlockedExchangeAdd8)
#   pragma intrinsic(_InterlockedExchange8)
#   pragma intrinsic(_InterlockedCompareExchange8)

#   define NX_INTERLOCKED_EXCHANGEADD8(a, b)        _InterlockedExchangeAdd8((char volatile *)&(a), (char)(b))
#   define NX_INTERLOCKED_EXCHANGE8(a, b)           _InterlockedExchange8((char volatile *)&(a), (char)(b))
#   define NX_INTERLOCKED_COMPAREEXCHANGE8(a, b, c) _InterlockedCompareExchange8((char volatile *)&(a), (char)(b), (char)(c))

#   pragma intrinsic(_InterlockedExchangeAdd16)
#   pragma intrinsic(_InterlockedExchange16)
#   pragma intrinsic(_InterlockedCompareExchange16)

#   define NX_INTERLOCKED_EXCHANGEADD16(a, b)        _InterlockedExchangeAdd16((short volatile *)&(a), (short)(b))
#   define NX_INTERLOCKED_EXCHANGE16(a, b)           _InterlockedExchange16((short volatile *)&(a), (short)(b))
#   define NX_INTERLOCKED_COMPAREEXCHANGE16(a, b, c) _InterlockedCompareExchange16((short volatile *)&(a), (short)(b), (short)(c))
#   endif
#else /*NX_INTERLOCKED_NOASM*/
#   define NX_INTERLOCKED_EXCHANGE8(a, b)
#   define NX_INTERLOCKED_EXCHANGE16(a, b)
#endif/*NX_INTERLOCKED_NOASM*/

#if defined(NX_PC_X64)
#   pragma intrinsic(_InterlockedExchangeAdd64)
#   pragma intrinsic(_InterlockedExchange64)
#   pragma intrinsic(_InterlockedCompareExchange64)

#   define NX_INTERLOCKED_EXCHANGEADD64(a, b)        _InterlockedExchangeAdd64((llong volatile *)&(a), (llong)(b))
#   define NX_INTERLOCKED_EXCHANGE64(a, b)           _InterlockedExchange64((llong volatile *)&(a), (llong)(b))
#   define NX_INTERLOCKED_COMPAREEXCHANGE64(a, b, c) _InterlockedCompareExchange64((llong volatile *)&(a), (llong)(b), (llong)(c))
#endif

#endif/*NX_OS_WINCE*/

//////////////////////////////////////////////////////////////////////////
