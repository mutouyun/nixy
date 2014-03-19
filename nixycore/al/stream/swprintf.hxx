/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

//////////////////////////////////////////////////////////////////////////

#if defined(NX_OS_WIN)
#   define NX_SWPRINTF_(buf, num, fmt, ...) std::swprintf(buf, fmt,##__VA_ARGS__)
#elif defined(NX_OS_LINUX)
#   define NX_SWPRINTF_(buf, num, fmt, ...) std::swprintf(buf, num, fmt,##__VA_ARGS__)
#endif

//////////////////////////////////////////////////////////////////////////
