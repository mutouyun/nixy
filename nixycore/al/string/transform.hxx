/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

//////////////////////////////////////////////////////////////////////////
/// The transform between local character set and UTF-X
//////////////////////////////////////////////////////////////////////////

size_t local_to_utf(const char* src, wchar_t* des = 0, size_t len = 0)
{
#if defined(NX_CC_MSVC)
    int n = 0;
    if (des)
    {
        n = static_cast<int>(len);
        if (n <= 0)
            n = MultiByteToWideChar(CP_OEMCP, 0, src, -1, NULL, 0);
        n = MultiByteToWideChar(CP_OEMCP, 0, src, -1, des, n);
    }
    else
        n = MultiByteToWideChar(CP_OEMCP, 0, src, -1, NULL, 0);
    return static_cast<size_t>(n);
#elif defined(NX_CC_GNUC)
    (void)len;
    return utf(src, des);
#else
    (void)src; (void)des; (void)len;
    return 0;
#endif
}

size_t utf_to_local(const wchar_t* src, char* des = 0, size_t len = 0)
{
#if defined(NX_CC_MSVC) || defined(NX_OS_WIN)
    int n = 0;
    if (des)
    {
        n = static_cast<int>(len);
        if (n <= 0)
            n = WideCharToMultiByte(CP_OEMCP, 0, src, -1, NULL, 0, NULL, NULL);
        n = WideCharToMultiByte(CP_OEMCP, 0, src, -1, des, n, NULL, NULL);
    }
    else
        n = WideCharToMultiByte(CP_OEMCP, 0, src, -1, NULL, 0, NULL, NULL);
    return static_cast<size_t>(n);
#elif defined(NX_CC_GNUC)
    (void)len;
    return utf(src, des);
#else
    (void)src; (void)des; (void)len;
    return 0;
#endif
}

//////////////////////////////////////////////////////////////////////////
