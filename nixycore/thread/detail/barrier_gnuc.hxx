/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

//////////////////////////////////////////////////////////////////////////

#define NX_CC_BARRIER() __asm__ __volatile__ ("" ::: "memory")

//////////////////////////////////////////////////////////////////////////
NX_BEG namespace thread_ops {
//////////////////////////////////////////////////////////////////////////

inline static void cc_barrier(void)
{
    NX_CC_BARRIER();
}

inline static void mm_barrier(void)
{
#if NX_CHECK_GNUC(4, 1, 2)
    __sync_synchronize();
#elif defined(NX_PC_X86_64) || defined(NX_PC_SSE2)
    __asm__ __volatile__ ("mfence" ::: "memory");
#elif defined(NX_PC_X86_32)
    __asm__ __volatile__ ("lock ; addl $0, (%%esp)" ::: "memory");
#elif defined(NX_PC_IA_64)
    __asm__ __volatile__ ("mf" ::: "memory");
#elif defined(NX_PC_ARM)
    ((void(*)(void))0xffff0fa0)();
    NX_CC_BARRIER();
#endif
}

//////////////////////////////////////////////////////////////////////////
} NX_END
//////////////////////////////////////////////////////////////////////////
