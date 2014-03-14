/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

//////////////////////////////////////////////////////////////////////////

#if defined(NX_OS_WIN)
#define NX_SWPRINTF_(buf, fmt, ...) std::swprintf(buf, fmt,##__VA_ARGS__)
#elif defined(NX_OS_LINUX)
#define NX_SWPRINTF_(buf, fmt, ...) std::swprintf(buf, nx_countof(buf), fmt,##__VA_ARGS__)
#endif

#define NX_SWSCANF_(buf, fmt, ...)  std::swscanf(buf, fmt,##__VA_ARGS__)

//////////////////////////////////////////////////////////////////////////
