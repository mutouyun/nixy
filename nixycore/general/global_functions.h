/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#if defined(NX_OS_LINUX)
#include <sys/sysinfo.h>
#endif

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    Detect the numbers of the CPU cores
*/
inline long detect_cpu_count(void)
{
    static long cpu_count = 0;
    if (cpu_count > 0) return cpu_count;
#if defined(NX_OS_WIN)
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    cpu_count = static_cast<long>(si.dwNumberOfProcessors);
#elif defined(NX_OS_LINUX)
    cpu_count = static_cast<long>(get_nprocs());
#endif
    if (cpu_count < 1) cpu_count = 1; // fail-safe
    return cpu_count;
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
