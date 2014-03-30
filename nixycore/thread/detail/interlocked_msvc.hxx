/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

//////////////////////////////////////////////////////////////////////////
NX_BEG namespace use { struct interlocked_mt : private_interlocked::base<interlocked_mt> {
//////////////////////////////////////////////////////////////////////////

#ifdef NX_INTERLOCKED_EXCHANGE8
NX_STATIC_PROPERTY(int, mask_8_, 0x1);
#else
NX_STATIC_PROPERTY(int, mask_8_, 0x0);
#endif

#ifdef NX_INTERLOCKED_EXCHANGE16
NX_STATIC_PROPERTY(int, mask_16_, 0x2);
#else
NX_STATIC_PROPERTY(int, mask_16_, 0x0);
#endif

#ifdef NX_INTERLOCKED_EXCHANGE
NX_STATIC_PROPERTY(int, mask_32_, 0x4);
#else
NX_STATIC_PROPERTY(int, mask_32_, 0x0);
#endif

#ifdef NX_INTERLOCKED_EXCHANGE64
NX_STATIC_PROPERTY(int, mask_64_, 0x8);
#else
NX_STATIC_PROPERTY(int, mask_64_, 0x0);
#endif

NX_STATIC_PROPERTY(int, supported_mask, mask_8_ | mask_16_ | mask_32_ | mask_64_);

#ifdef NX_INTERLOCKED_EXCHANGE8

/*
    sizeof(T) == 1
*/

template <typename T, typename U>
inline static typename nx::enable_if<sizeof(T) == 1,
T>::type_t fetch_add(T& dest, U val)
{
#if defined(NX_INTERLOCKED_NOASM)
    return (T)NX_INTERLOCKED_EXCHANGEADD8(dest, val);
#else
    thread_ops::cc_barrier();
    __asm
    {
        mov  al , byte ptr [val]
        mov  ecx, [dest]
        lock xadd [ecx] , al
    }
    thread_ops::cc_barrier();
#endif
}

template <typename T, typename U>
inline static typename nx::enable_if<sizeof(T) == 1,
T>::type_t exchange(T& dest, U val)
{
#if defined(NX_INTERLOCKED_NOASM)
    return (T)NX_INTERLOCKED_EXCHANGE8(dest, val);
#else
    thread_ops::cc_barrier();
    __asm
    {
        mov  al , byte ptr [val]
        mov  ecx, [dest]
        xchg al , [ecx]
    }
    thread_ops::cc_barrier();
#endif
}

template <typename T, typename U>
inline static typename nx::enable_if<sizeof(T) == 1,
bool>::type_t compare_exchange(T& dest, U val, T comp)
{
#if defined(NX_INTERLOCKED_NOASM)
    return (comp == (T)NX_INTERLOCKED_COMPAREEXCHANGE8(dest, val, comp));
#else
    bool success;
    thread_ops::cc_barrier();
    __asm
    {
        mov  dl ,    byte ptr [val]
        mov  esi,    [dest]
        mov  al ,    [comp]
        lock cmpxchg [esi] , dl
        sete [success]
    }
    thread_ops::cc_barrier();
    return success;
#endif
}

#endif/*NX_INTERLOCKED_EXCHANGE8*/

#ifdef NX_INTERLOCKED_EXCHANGE16

/*
    sizeof(T) == 2
*/

template <typename T, typename U>
inline static typename nx::enable_if<sizeof(T) == 2,
T>::type_t fetch_add(T& dest, U val)
{
#if defined(NX_INTERLOCKED_NOASM)
    return (T)NX_INTERLOCKED_EXCHANGEADD16(dest, val);
#else
    thread_ops::cc_barrier();
    __asm
    {
        mov  ax , word ptr [val]
        mov  ecx, [dest]
        lock xadd [ecx] , ax
    }
    thread_ops::cc_barrier();
#endif
}

template <typename T, typename U>
inline static typename nx::enable_if<sizeof(T) == 2,
T>::type_t exchange(T& dest, U val)
{
#if defined(NX_INTERLOCKED_NOASM)
    return (T)NX_INTERLOCKED_EXCHANGE16(dest, val);
#else
    thread_ops::cc_barrier();
    __asm
    {
        mov  ax , word ptr [val]
        mov  ecx, [dest]
        xchg ax , [ecx]
    }
    thread_ops::cc_barrier();
#endif
}

template <typename T, typename U>
inline static typename nx::enable_if<sizeof(T) == 2,
bool>::type_t compare_exchange(T& dest, U val, T comp)
{
#if defined(NX_INTERLOCKED_NOASM)
    return (comp == (T)NX_INTERLOCKED_COMPAREEXCHANGE16(dest, val, comp));
#else
    bool success;
    thread_ops::cc_barrier();
    __asm
    {
        mov  dx ,    word ptr [val]
        mov  esi,    [dest]
        mov  ax ,    [comp]
        lock cmpxchg [esi] , dx
        sete [success]
    }
    thread_ops::cc_barrier();
    return success;
#endif
}

#endif/*NX_INTERLOCKED_EXCHANGE16*/

#ifdef NX_INTERLOCKED_EXCHANGE

/*
    sizeof(T) == 4
*/

template <typename T, typename U>
inline static typename nx::enable_if<sizeof(T) == 4,
T>::type_t fetch_add(T& dest, U val)
{
    return (T)NX_INTERLOCKED_EXCHANGEADD(dest, val);
}

template <typename T, typename U>
inline static typename nx::enable_if<sizeof(T) == 4,
T>::type_t exchange(T& dest, U val)
{
    return (T)NX_INTERLOCKED_EXCHANGE(dest, val);
}

template <typename T, typename U>
inline static typename nx::enable_if<sizeof(T) == 4,
bool>::type_t compare_exchange(T& dest, U val, T comp)
{
    return (comp == (T)NX_INTERLOCKED_COMPAREEXCHANGE(dest, val, comp));
}

#endif/*NX_INTERLOCKED_EXCHANGE*/

#ifdef NX_INTERLOCKED_EXCHANGE64

/*
    sizeof(T) == 8
*/

template <typename T, typename U>
inline static typename nx::enable_if<sizeof(T) == 8,
T>::type_t fetch_add(T& dest, U val)
{
    return (T)NX_INTERLOCKED_EXCHANGEADD64(dest, val);
}

template <typename T, typename U>
inline static typename nx::enable_if<sizeof(T) == 8,
T>::type_t exchange(T& dest, U val)
{
    return (T)NX_INTERLOCKED_EXCHANGE64(dest, val);
}

template <typename T, typename U>
inline static typename nx::enable_if<sizeof(T) == 8,
bool>::type_t compare_exchange(T& dest, U val, T comp)
{
    return (comp == (T)NX_INTERLOCKED_COMPAREEXCHANGE64(dest, val, comp));
}

#endif/*NX_INTERLOCKED_EXCHANGE64*/

//////////////////////////////////////////////////////////////////////////
}; } NX_END
//////////////////////////////////////////////////////////////////////////
