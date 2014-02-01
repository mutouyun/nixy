/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

//////////////////////////////////////////////////////////////////////////

#if defined(NX_CC_MSVC)
#   include "al/thread/barrier_msvc.hxx"
#elif defined(NX_CC_GNUC)
#   include "al/thread/barrier_gnuc.hxx"
#endif

//////////////////////////////////////////////////////////////////////////
