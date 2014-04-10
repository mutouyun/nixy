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

#define NX_CHECK_GNUC(ver, minor, patch) \
    defined(__GNUC__) /* >= ver.minor.patch */ && \
    ((__GNUC__ > (ver)) || ((__GNUC__ == (ver)) && \
    ((__GNUC_MINOR__ > (minor)) || ((__GNUC_MINOR__ == (minor)) && \
    (__GNUC_PATCHLEVEL__ >= (patch)) ) ) ) )

#if defined(_MSC_VER) && (_MSC_VER >= 1400)     /* >= 8.0 */
#   define NX_CC_MSVC _MSC_VER
#elif defined(__GNUC__)
#   define NX_CC_GNUC
#   if defined(__clang__)
#       define NX_CC_CLANG                      // Clang also masquerades as GCC
#   endif
#else
#   error "This CC is unsupported."
#endif

#if defined(NX_CC_MSVC)
#   pragma warning(disable:4996)                // This function or variable may be unsafe
#   pragma warning(disable:4101)                // The local variable is never used
#   define NX_UNUSED
#   ifndef NX__FUNCTION__
#       define NX__FUNCTION__ __FUNCSIG__
#   endif
#elif defined(NX_CC_GNUC)
#   define NX_UNUSED __attribute__ ((unused))
#   ifndef NX__FUNCTION__
#       define NX__FUNCTION__ __PRETTY_FUNCTION__
#   endif
#endif

//////////////////////////////////////////////////////////////////////////
/*
    C++ features support, must be one of: (NX_SP_XX)

    CXX11_BASIC - Support c++ 11 basic features
    NOEXCEPT    - noexcept
    ALIAS       - Type alias, alias template
    RANGEFOR    - Range-based for-loop
    ARRAY       - Container array
*/
//////////////////////////////////////////////////////////////////////////

#if defined(NX_CC_MSVC)
#   if (NX_CC_MSVC >= 1600)
#   define NX_SP_CXX11_BASIC
#   define NX_SP_ARRAY
#   endif
#   if (NX_CC_MSVC >= 1700)
#   define NX_SP_RANGEFOR
#   endif
#   if (NX_CC_MSVC >= 1800)
#   define NX_SP_ALIAS
#   endif
#elif defined(NX_CC_GNUC)
#   if (__cplusplus >= 201103L)
#   define NX_SP_CXX11_BASIC
#   define NX_SP_NOEXCEPT
#   define NX_SP_ALIAS
#   define NX_SP_RANGEFOR
#   define NX_SP_ARRAY
#   endif
#endif

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
/*
    The operating system, must be one of: (NX_OS_XX)

    WIN     - Windows
    LINUX   - Linux
*/
//////////////////////////////////////////////////////////////////////////

#if defined(_WIN32_WCE)
#   define NX_OS_WINCE _WIN32_WCE
#elif defined(WINCE)
#   define NX_OS_WINCE (0)
#elif defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
#   define NX_OS_WIN64
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#   define NX_OS_WIN32
#elif defined(__linux__) || defined(__linux)
#   define NX_OS_LINUX
#else
#   error "This OS is unsupported."
#endif

#if defined(NX_OS_WIN32) || defined(NX_OS_WIN64) || defined(NX_OS_WINCE)
#   define NX_OS_WIN
#endif

//////////////////////////////////////////////////////////////////////////

#ifdef NX_OS_WIN

#   ifndef _WIN32_WINNT
#       define _WIN32_WINNT                     0x0501
#   endif
#   ifndef WINVER
#       define WINVER                           _WIN32_WINNT
#   endif
#   ifndef _WIN32_WINDOWS
#       define _WIN32_WINDOWS                   0x0410
#   endif
#   ifndef _WIN32_IE
#       define _WIN32_IE                        0x0600
#   endif

#   include <windows.h>
#   ifdef NX_OS_WINCE
#       include <ceconfig.h>
#   endif

#   ifndef SPI_SETMENUANIMATION
#       define SPI_SETMENUANIMATION             0x1003
#   endif
#   ifndef SPI_SETMENUFADE
#       define SPI_SETMENUFADE                  0x1013
#   endif
#   ifndef SPI_SETCOMBOBOXANIMATION
#       define SPI_SETCOMBOBOXANIMATION         0x1005
#   endif
#   ifndef SPI_SETTOOLTIPANIMATION
#       define SPI_SETTOOLTIPANIMATION          0x1017
#   endif
#   ifndef SPI_SETTOOLTIPFADE
#       define SPI_SETTOOLTIPFADE               0x1019
#   endif
#   ifndef SPI_SETUIEFFECTS
#       define SPI_SETUIEFFECTS                 0x103F
#   endif
#   ifndef SPI_GETMENUANIMATION
#       define SPI_GETMENUANIMATION             0x1002
#   endif
#   ifndef SPI_GETMENUFADE
#       define SPI_GETMENUFADE                  0x1012
#   endif
#   ifndef SPI_GETCOMBOBOXANIMATION
#       define SPI_GETCOMBOBOXANIMATION         0x1004
#   endif
#   ifndef SPI_GETTOOLTIPANIMATION
#       define SPI_GETTOOLTIPANIMATION          0x1016
#   endif
#   ifndef SPI_GETTOOLTIPFADE
#       define SPI_GETTOOLTIPFADE               0x1018
#   endif
#   ifndef SPI_GETUIEFFECTS
#       define SPI_GETUIEFFECTS                 0x103E
#   endif
#   ifndef SPI_GETKEYBOARDCUES
#       define SPI_GETKEYBOARDCUES              0x100A
#   endif
#   ifndef SPI_GETGRADIENTCAPTIONS
#       define SPI_GETGRADIENTCAPTIONS          0x1008
#   endif
#   ifndef IDC_HAND
#       define IDC_HAND                         MAKEINTRESOURCE(32649)
#   endif
#   ifndef WM_MOUSEWHEEL
#       define WM_MOUSEWHEEL                    0x020A
#   endif
#   ifndef WM_MOUSEHWHEEL
#       define WM_MOUSEHWHEEL                   0x020E
#   endif
#   ifndef ETO_PDY
#       define ETO_PDY                          0x2000
#   endif
#   ifndef COLOR_GRADIENTACTIVECAPTION
#       define COLOR_GRADIENTACTIVECAPTION      27
#   endif
#   ifndef COLOR_GRADIENTINACTIVECAPTION
#       define COLOR_GRADIENTINACTIVECAPTION    28
#   endif
#   ifndef SPI_GETFLATMENU
#       define SPI_GETFLATMENU                  0x1022
#   endif
#   ifndef CS_DROPSHADOW
#       define CS_DROPSHADOW                    0x00020000
#   endif
#   ifndef CLEARTYPE_QUALITY
#       define CLEARTYPE_QUALITY                5
#   endif

#   ifdef NX_OS_WINCE
#       ifndef LR_DEFAULTSIZE
#           define LR_DEFAULTSIZE               0
#       endif
#       ifndef LR_SHARED
#           define LR_SHARED                    0
#       endif
#   endif

#endif

//////////////////////////////////////////////////////////////////////////
/*
    The other defines
*/
//////////////////////////////////////////////////////////////////////////

#ifndef NDEBUG
#   ifndef _DEBUG
#       define _DEBUG
#   endif
#   ifndef DEBUG
#       define DEBUG
#   endif
#endif

#ifndef CHAR_BIT
#   define CHAR_BIT 8
#endif
