/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

//////////////////////////////////////////////////////////////////////////
/*
    The processor, must be one of: (NX_PC_XX)

    X86_32  - x86 32-bit processors
    X86_64  - x86 64-bit processors
    IA_64   - Itanium Processor Family 64-bit processors
    ARM     - ARM processors
*/
//////////////////////////////////////////////////////////////////////////

#if defined(__i386__) || defined(_M_IX86)
#   define NX_PC_X86_32
#elif defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64)
#   define NX_PC_X86_64
#elif defined(__ia64__) || defined(__ia64) || defined(_M_IA64)
#   define NX_PC_IA_64
#elif defined(__arm__) || defined(_ARM_) || defined(_M_ARM)
#   define NX_PC_ARM
#else
#   error "This processor is unsupported."
#endif

#if defined(NX_PC_X86_32) || defined(NX_PC_X86_64)
#   define NX_PC_X86
#endif

#if defined(NX_PC_X86_32)
#   define NX_PC_X32
#endif

#if defined(NX_PC_X86_64) || defined(NX_PC_IA_64)
#   define NX_PC_X64
#endif

#if defined(NX_CC_GNUC)
#   if defined(__MMX__)
#       define NX_PC_MMX
#   endif
#   if defined(__SSE__)
#       define NX_PC_SSE
#   endif
#   if defined(__SSE2__)
#       define NX_PC_SSE2
#   endif
#elif defined(NX_CC_MSVC) && defined(NX_PC_X86)
#   if !defined(NX_PC_X86_64)
#       define NX_PC_MMX
#   endif
#   if defined(_M_IX86_FP) && (_M_IX86_FP >= 1)
#       define NX_PC_SSE
#   endif
#   if defined(_M_IX86_FP) && (_M_IX86_FP >= 2)
#       define NX_PC_SSE2
#   endif
#endif

//////////////////////////////////////////////////////////////////////////
