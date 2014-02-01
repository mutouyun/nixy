/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

//////////////////////////////////////////////////////////////////////////

/*
    Traits for functor class
*/

#if !(defined(NX_OS_WINCE) && (NX_CC_MSVC <= 1400))

template <typename T>
struct detail<T*, false> : detail<nx_typeof(&T::operator())>
{};

#endif

//////////////////////////////////////////////////////////////////////////
