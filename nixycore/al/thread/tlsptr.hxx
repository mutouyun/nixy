/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

//////////////////////////////////////////////////////////////////////////

#if defined(NX_OS_WIN)
#   include "al/thread/tlsptr_win.hxx"
#elif defined(NX_OS_LINUX)
#   include "al/thread/tlsptr_linux.hxx"
#endif

//////////////////////////////////////////////////////////////////////////
