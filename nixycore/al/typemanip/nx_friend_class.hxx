/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

//////////////////////////////////////////////////////////////////////////

#if defined(NX_CC_GNUC)
#   define NX_FRIEND_CLASS(...) friend class type_wrap<__VA_ARGS__>::type_t
#else
#   define NX_FRIEND_CLASS(...) friend __VA_ARGS__
#endif

//////////////////////////////////////////////////////////////////////////
