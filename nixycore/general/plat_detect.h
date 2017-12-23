/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://orzz.org)
*/

//////////////////////////////////////////////////////////////////////////

// The compiler, must be one of: (NX_CC_XX)
#include "detail/plat_detect_cc.hxx"

// C++ features support, must be one of: (NX_SP_XX)
#include "detail/plat_detect_sp.hxx"

// The processor, must be one of: (NX_PC_XX)
#include "detail/plat_detect_pc.hxx"

// The operating system, must be one of: (NX_OS_XX)
#include "detail/plat_detect_os.hxx"

// The other defines

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

//////////////////////////////////////////////////////////////////////////
