/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

//////////////////////////////////////////////////////////////////////////

#if defined(NX_PC_X86)

#ifdef NX_PC_X86_64
NX_STATIC_PROPERTY(int, SUPPORTED_MASK, 0xF);
#else
NX_STATIC_PROPERTY(int, SUPPORTED_MASK, 0x7);
#endif

/*
    sizeof(T) == 1
*/

template <typename T, typename U, typename M>
inline static typename nx::enable_if<sizeof(T) == 1,
T>::type_t fetch_add(T& dest, U val, M m)
{
    NX_FENCE_GUARD_(m);
    __asm__ __volatile__
    (
        "lock ; xaddb %b0, %1"
        : "+q" (val), "+m" (dest)
        :
        : "cc"
    );
    return val;
}

template <typename T, typename U, typename M>
inline static typename nx::enable_if<sizeof(T) == 1,
T>::type_t exchange(T& dest, U val, M m)
{
    NX_FENCE_GUARD_(m);
    __asm__ __volatile__
    (
        "xchgb %b0, %1"
        : "+q" (val), "+m" (dest)
    );
    return val;
}

template <typename T, typename U, typename M>
inline static typename nx::enable_if<sizeof(T) == 1,
bool>::type_t compare_exchange(T& dest, U val, T comp, M m)
{
    bool success;
    NX_FENCE_GUARD_(m);
    __asm__ __volatile__
    (
        "lock ; cmpxchgb %b3, %1\n\t"
        "sete %2"
        : "+a" (comp), "+m" (dest), "=q" (success)
        : "q" (val)
        : "cc"
    );
    return success;
}

/*
    sizeof(T) == 2
*/

template <typename T, typename U, typename M>
inline static typename nx::enable_if<sizeof(T) == 2,
T>::type_t fetch_add(T& dest, U val, M m)
{
    NX_FENCE_GUARD_(m);
    __asm__ __volatile__
    (
        "lock ; xaddw %w0, %1"
        : "+q" (val), "+m" (dest)
        :
        : "cc"
    );
    return val;
}

template <typename T, typename U, typename M>
inline static typename nx::enable_if<sizeof(T) == 2,
T>::type_t exchange(T& dest, U val, M m)
{
    NX_FENCE_GUARD_(m);
    __asm__ __volatile__
    (
        "xchgw %w0, %1"
        : "+q" (val), "+m" (dest)
    );
    return val;
}

template <typename T, typename U, typename M>
inline static typename nx::enable_if<sizeof(T) == 2,
bool>::type_t compare_exchange(T& dest, U val, T comp, M m)
{
    bool success;
    NX_FENCE_GUARD_(m);
    __asm__ __volatile__
    (
        "lock ; cmpxchgw %w3, %1\n\t"
        "sete %2"
        : "+a" (comp), "+m" (dest), "=q" (success)
        : "q" (val)
        : "cc"
    );
    return success;
}

/*
    sizeof(T) == 4
*/

template <typename T, typename U, typename M>
inline static typename nx::enable_if<sizeof(T) == 4,
T>::type_t fetch_add(T& dest, U val, M m)
{
    NX_FENCE_GUARD_(m);
    __asm__ __volatile__
    (
        "lock ; xaddl %0, %1"
        : "+r" (val), "+m" (dest)
        :
        : "cc"
    );
    return val;
}

template <typename T, typename U, typename M>
inline static typename nx::enable_if<sizeof(T) == 4,
T>::type_t exchange(T& dest, U val, M m)
{
    NX_FENCE_GUARD_(m);
    __asm__ __volatile__
    (
        "xchgl %0, %1"
        : "+r" (val), "+m" (dest)
    );
    return val;
}

template <typename T, typename U, typename M>
inline static typename nx::enable_if<sizeof(T) == 4,
bool>::type_t compare_exchange(T& dest, U val, T comp, M m)
{
    bool success;
    NX_FENCE_GUARD_(m);
    __asm__ __volatile__
    (
        "lock ; cmpxchgl %3, %1\n\t"
        "sete %2"
        : "+a,a" (comp), "+m,m" (dest), "=q,m" (success)
        : "r,r" (val)
        : "cc"
    );
    return success;
}

#ifdef NX_PC_X86_64

/*
    sizeof(T) == 8
*/

template <typename T, typename U, typename M>
inline static typename nx::enable_if<sizeof(T) == 8,
T>::type_t fetch_add(T& dest, U val, M m)
{
    NX_FENCE_GUARD_(m);
    __asm__ __volatile__
    (
        "lock ; xaddq %0, %1"
        : "+r" (val), "+m" (dest)
        :
        : "cc"
    );
    return val;
}

template <typename T, typename U, typename M>
inline static typename nx::enable_if<sizeof(T) == 8,
T>::type_t exchange(T& dest, U val, M m)
{
    NX_FENCE_GUARD_(m);
    __asm__ __volatile__
    (
        "xchgq %0, %1"
        : "+r" (val), "+m" (dest)
    );
    return val;
}

template <typename T, typename U, typename M>
inline static typename nx::enable_if<sizeof(T) == 8,
bool>::type_t compare_exchange(T& dest, U val, T comp, M m)
{
    bool success;
    NX_FENCE_GUARD_(m);
    __asm__ __volatile__
    (
        "lock ; cmpxchgq %3, %1\n\t"
        "sete %2"
        : "+a,a" (comp), "+m,m" (dest), "=q,m" (success)
        : "r,r" (val)
        : "cc"
    );
    return success;
}

#endif/*NX_PC_X86_64*/

#elif defined(NX_PC_IA_64)

#if NX_CHECK_GNUC(4, 1, 2)

NX_STATIC_PROPERTY(int, SUPPORTED_MASK, 0xF);

template <typename T, typename U, typename M>
inline static T fetch_add(T& dest, U val, M m)
{
    NX_FENCE_GUARD_(m);
    return __sync_fetch_and_add(&(dest), val);
}

template <typename T, typename U, typename M>
inline static T exchange(T& dest, U val, M m)
{
    NX_FENCE_GUARD_(m);
    return __sync_lock_test_and_set(&(dest), val);
}

template <typename T, typename U, typename M>
inline static bool compare_exchange(T& dest, U val, T comp, M m)
{
    NX_FENCE_GUARD_(m);
    return __sync_bool_compare_and_swap(&(dest), comp, val);
}

#else /*NX_CHECK_GNUC(4, 1, 2)*/

/* T.B.D */

#endif/*NX_CHECK_GNUC(4, 1, 2)*/

#elif defined(NX_PC_ARM)

NX_STATIC_PROPERTY(int, SUPPORTED_MASK, 0x4);

template <typename T, typename U, typename M>
inline static typename nx::enable_if<sizeof(T) == 4,
T>::type_t fetch_add(T& dest, U val, M m)
{
    T old = const_cast<const volatile T &>(dest);
    do {} while (!compare_exchange(dest, old + val, old, m));
    return old;
}

template <typename T, typename U, typename M>
inline static typename nx::enable_if<sizeof(T) == 4,
T>::type_t exchange(T& dest, U val, M m)
{
    T old = const_cast<const volatile T &>(dest);
    do {} while (!compare_exchange(dest, val, old, m));
    return old;
}

template <typename T, typename U, typename M>
inline static typename nx::enable_if<sizeof(T) == 4,
bool>::type_t compare_exchange(T& dest, U val, T comp, M m)
{
    typedef T (*kernel_cmpxchg32_t)(T oldval, T newval, volatile T * ptr);
    NX_FENCE_GUARD_(m);
    return (((kernel_cmpxchg32_t)0xffff0fc0)(comp, (T)val, (volatile T *)&dest) == 0);
}

#endif/*NX_CHECK_GNUC(4, 1, 2)*/

//////////////////////////////////////////////////////////////////////////
