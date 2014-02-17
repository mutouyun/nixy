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
        : type_if<is_fundamental<T>::value ||
                  __has_trivial_constructor(T) && __is_pod(T)>
#elif defined(NX_CC_GNUC)
        : type_if<__is_pod(T)>
#else
        : type_if<is_fundamental<T>::value>
#endif
    {};
}

//////////////////////////////////////////////////////////////////////////
