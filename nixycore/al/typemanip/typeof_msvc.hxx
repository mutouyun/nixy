/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

//////////////////////////////////////////////////////////////////////////

#ifndef nx_typeof

/*
    Get a Type from a std::type_info
*/

template <const std::type_info& type_id>
struct TypeID {};

#define NX_TYPE_ID(...) nx::TypeID<typeid(__VA_ARGS__)>

/*
    Extract a type from TypeID
*/

template <typename ID, typename T = nx::null_t>
struct TypeExtract;

template <typename ID>
struct TypeExtract<ID, nx::null_t>
{
    template <bool>
    struct id2type { typedef nx::null_t type_t; };
};

template <typename ID, typename T>
struct TypeExtract : TypeExtract<ID, nx::null_t>
{
    template <>
    struct id2type<true> { typedef T type_t; };
};

/*
    Register a type
*/

template <typename T, typename ID>
struct TypeRegister : TypeExtract<ID, T>
{
    typedef typename id2type<true>::type_t type_t;
};

/*
    Encode a expression
*/

template <typename T>
struct TypeEncode
{
    typedef T* enc_type;
    typedef TypeRegister<enc_type, NX_TYPE_ID(enc_type)> reg_type;
    typedef typename reg_type::type_t type_t;
};

template <typename T>
typename TypeEncode<T>::type_t encodeType(const T&);

/*
    Decode a type
*/

template <typename ID>
struct TypeDecode
{
    typedef typename TypeExtract<ID>::template id2type<true>::type_t enc_type;
    typedef typename nx::rm_pointer<enc_type>::type_t type_t;
};

#define nx_typeof(...) nx::TypeDecode<NX_TYPE_ID(nx::encodeType(__VA_ARGS__))>::type_t

#endif

//////////////////////////////////////////////////////////////////////////
