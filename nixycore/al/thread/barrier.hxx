/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

//////////////////////////////////////////////////////////////////////////

#if defined(NX_CC_MSVC)
#   include "nixycore/al/thread/barrier_msvc.hxx"
#elif defined(NX_CC_GNUC)
#   include "nixycore/al/thread/barrier_gnuc.hxx"
#endif

//////////////////////////////////////////////////////////////////////////
