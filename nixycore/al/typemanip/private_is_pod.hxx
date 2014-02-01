/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

//////////////////////////////////////////////////////////////////////////

/*
    detect POD
*/

namespace private_is_pod
{
    template <typename T>
    struct detail
#ifdef NX_CC_MSVC
        : Judge<is_fundamental<T>::value ||
                __has_trivial_constructor(T) && __is_pod(T)>
#elif defined(NX_CC_GNUC)
        : Judge<__is_pod(T)>
#else
        : Judge<is_fundamental<T>::value>
#endif
    {};
}

//////////////////////////////////////////////////////////////////////////
