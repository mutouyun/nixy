/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

// std::type_info
#include <typeinfo>

//////////////////////////////////////////////////////////////////////////
NX_BEG namespace private_typeof {
//////////////////////////////////////////////////////////////////////////

/*
    Get a Type from a std::type_info
*/

template <const std::type_info& type_id>
struct TypeID {};

#define NX_TYPE_ID_(...) nx::private_typeof::TypeID<typeid(__VA_ARGS__)>

/*
    Extract a type from TypeID
*/

template <typename ID, typename T = nx::null_t>
struct type_extractor;

template <typename ID>
struct type_extractor<ID, nx::null_t>
{
    template <bool>
    struct id2type { typedef nx::null_t type_t; };
};

template <typename ID, typename T>
struct type_extractor : type_extractor<ID, nx::null_t>
{
    template <>
    struct id2type<true> { typedef T type_t; };
};

/*
    Register a type
*/

template <typename T, typename ID>
struct type_register : type_extractor<ID, T>
{
    typedef typename id2type<true>::type_t type_t;
};

/*
    Encode a expression
*/

template <typename T>
struct type_encoder
{
    typedef T* enc_type;
    typedef type_register<enc_type, NX_TYPE_ID_(enc_type)> reg_type;
    typedef typename reg_type::type_t type_t;
};

template <typename T>
typename type_encoder<T>::type_t encode_type(const T&);

/*
    Decode a type
*/

template <typename ID>
struct type_decoder
{
    typedef typename type_extractor<ID>::template id2type<true>::type_t enc_type;
    typedef typename rm_pointer<enc_type>::type_t type_t;
};

#define NX_TYPEOF_(...) \
    nx::private_typeof::type_decoder<NX_TYPE_ID_(nx::private_typeof::encode_type(__VA_ARGS__))>::type_t

//////////////////////////////////////////////////////////////////////////
} NX_END
//////////////////////////////////////////////////////////////////////////
