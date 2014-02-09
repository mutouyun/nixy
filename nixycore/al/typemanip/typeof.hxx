/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

//////////////////////////////////////////////////////////////////////////

#if defined(NX_CC_GNUC)
#   ifndef nx_typeof
#       define nx_typeof(...) __typeof(__VA_ARGS__)
#   endif
#elif defined(NX_CC_MSVC)
#   include "nixycore/al/typemanip/typeof_msvc.hxx"
#endif

//////////////////////////////////////////////////////////////////////////
