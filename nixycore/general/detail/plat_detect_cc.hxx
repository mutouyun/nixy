/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

//////////////////////////////////////////////////////////////////////////
/*
    The compiler, must be one of: (NX_CC_XX)

    MSVC    - Microsoft Visual C++
    GNUC    - GNU C++
    CLANG   - C++ front-end for the LLVM compiler
*/
//////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && (_MSC_VER >= 1400)     /* >= 8.0 */
#   define NX_CC_MSVC _MSC_VER
#elif defined(__GNUC__)
#   define NX_CC_GNUC
#   if defined(__clang__)
#   define NX_CC_CLANG                          // Clang also masquerades as GCC
#   else
#   define NX_CC_GCC
#   endif
#else
#   error "This CC is unsupported."
#endif

#if defined(NX_CC_MSVC)
#   pragma warning(disable:4996)                // This function or variable may be unsafe
#   pragma warning(disable:4101)                // The local variable is never used
#   define NX_UNUSED __pragma(warning(suppress:4100))
#   ifndef NX__FUNCTION__
#       define NX__FUNCTION__ __FUNCSIG__
#   endif
#elif defined(NX_CC_GNUC)
#   define NX_UNUSED __attribute__((__unused__))
#   ifndef NX__FUNCTION__
#       define NX__FUNCTION__ __PRETTY_FUNCTION__
#   endif
#endif

//////////////////////////////////////////////////////////////////////////
