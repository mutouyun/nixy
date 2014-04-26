/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

//////////////////////////////////////////////////////////////////////////
/*
    C++ features support, must be one of: (NX_SP_XX)

    CXX11_TYPE_TRAITS   - <type_traits>
    CXX11_CHAR_TYPE     - <uchar.h>
    CXX11_ATOMIC        - <atomic>
    CXX11_MUTEX         - <mutex>
    CXX11_CONDITION     - <condition_variable>
    CXX11_THREAD        - <thread>
    CXX11_RESULT_OF     - std::result_of
    CXX11_TUPLE         - std::tuple
    CXX11_STACK_SWAP    - std::stack/std::queue/std::priority_queue's swap
    CXX11_ARRAY         - std::array
    CXX11_FORWARD_LIST  - std::forward_list
    CXX11_UNORDERED_MAP - std::unordered_map
    CXX11_UNORDERED_SET - std::unordered_set
    CXX11_NULLPTR       - nullptr
    CXX11_AUTO          - auto
    CXX11_RVALUE_REF    - Rvalue references
    CXX11_PERFECT_FWD   - Perfect Forwarding
    CXX11_SATIC_ASSERT  - static_assert
    CXX11_ALIAS         - Type alias, alias template
    CXX11_RANGEFOR      - Range-based for-loop
    CXX11_NOEXCEPT      - noexcept
    CXX11_TEMPLATES     - Variadic templates
    CXX11_TEMPLATES_EX  - Extending variadic template template parameters
    CXX11_DELEGATING    - Delegating constructors
    CXX11_INHERITING    - Inheriting constructors
    CXX11_ALIGNMENT     - Alignment
    CXX11_INITIALIZER   - Initializer lists
    CXX11_LAMBDA        - Lambda expressions
*/
//////////////////////////////////////////////////////////////////////////

#define NX_CHECK_GNUC(ver, minor, patch) \
    defined(__GNUC__) /* >= ver.minor.patch */ && \
          ((__GNUC__ > (ver)) || ((__GNUC__ == (ver)) && \
          ((__GNUC_MINOR__ > (minor)) || ((__GNUC_MINOR__ == (minor)) && \
           (__GNUC_PATCHLEVEL__ >= (patch)) ) ) ) )

#define NX_CHECK_CLANG(ver, minor, patch) \
    defined(__clang__) /* >= ver.minor.patch */ && \
          ((__clang_major__ > (ver)) || ((__clang_major__ == (ver)) && \
          ((__clang_minor__ > (minor)) || ((__clang_minor__ == (minor)) && \
          (__clang_patchlevel__ >= (patch)) ) ) ) )

#ifndef NX_NO_DETECT_SP

#if defined(NX_CC_MSVC)
/*
    Microsoft Visual C++

    Start from Visual Studio 2010
    See: http://msdn.microsoft.com/en-us/library/hh567368.aspx
*/
#   if (NX_CC_MSVC >= 1600)
#       define NX_SP_CXX11_RESULT_OF
#       define NX_SP_CXX11_STACK_SWAP
#       define NX_SP_CXX11_ARRAY
#       define NX_SP_CXX11_FORWARD_LIST
#       define NX_SP_CXX11_UNORDERED_MAP
#       define NX_SP_CXX11_UNORDERED_SET
#       define NX_SP_CXX11_NULLPTR
#       define NX_SP_CXX11_SATIC_ASSERT
#   endif
#   if (NX_CC_MSVC >= 1700)
#       define NX_SP_CXX11_TYPE_TRAITS
#       define NX_SP_CXX11_ATOMIC
#       define NX_SP_CXX11_MUTEX
#       define NX_SP_CXX11_CONDITION
#       define NX_SP_CXX11_THREAD
#       define NX_SP_CXX11_RVALUE_REF
#       define NX_SP_CXX11_PERFECT_FWD
#       define NX_SP_CXX11_RANGEFOR
        /*
            <MSVC 2010> fatal error C1001
            When: static nx_guard_scope(onSoleExit)
        */
#       define NX_SP_CXX11_AUTO
        /*
            <MSVC 2010> compiler error C2664
            cannot convert parameter number from 'nx::rvalue<...>' to 'const nx::rvalue<...> &'
        */
#       define NX_SP_CXX11_LAMBDA
#   endif
#   if (NX_CC_MSVC >= 1800)
#       define NX_SP_CXX11_ALIAS
#       define NX_SP_CXX11_TEMPLATES
#       define NX_SP_CXX11_TEMPLATES_EX
#       define NX_SP_CXX11_TUPLE
#       define NX_SP_CXX11_DELEGATING
#       define NX_SP_CXX11_INITIALIZER
#   endif

#elif defined(NX_CC_GCC)
/*
    GNU C++

    Start from GCC 4.3
    See: http://gcc.gnu.org/projects/cxx0x.html
*/
#if (defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus >= 201103L))

#   if NX_CHECK_GNUC(4, 3, 0)
#       define NX_SP_CXX11_TYPE_TRAITS
#       define NX_SP_CXX11_STACK_SWAP
#       define NX_SP_CXX11_ARRAY
#       define NX_SP_CXX11_FORWARD_LIST
#       define NX_SP_CXX11_UNORDERED_MAP
#       define NX_SP_CXX11_UNORDERED_SET
#       define NX_SP_CXX11_SATIC_ASSERT
#       define NX_SP_CXX11_TEMPLATES
#       define NX_SP_CXX11_TUPLE
#   endif
#   if NX_CHECK_GNUC(4, 4, 0)
#       define NX_SP_CXX11_AUTO
#       define NX_SP_CXX11_TEMPLATES_EX
#       define NX_SP_CXX11_INITIALIZER
#   endif
#   if NX_CHECK_GNUC(4, 5, 0)
#       define NX_SP_CXX11_ATOMIC
#       define NX_SP_CXX11_LAMBDA
#   endif
#   if NX_CHECK_GNUC(4, 6, 0)
#       define NX_SP_CXX11_NULLPTR
#       define NX_SP_CXX11_RANGEFOR
#       define NX_SP_CXX11_NOEXCEPT
#   endif
#   if NX_CHECK_GNUC(4, 7, 0)
#       define NX_SP_CXX11_CHAR_TYPE
#       define NX_SP_CXX11_MUTEX
#       define NX_SP_CXX11_CONDITION
#       define NX_SP_CXX11_THREAD
#       define NX_SP_CXX11_RESULT_OF
#       define NX_SP_CXX11_RVALUE_REF
#       define NX_SP_CXX11_PERFECT_FWD
#       define NX_SP_CXX11_ALIAS
#       define NX_SP_CXX11_DELEGATING
#   endif
#   if NX_CHECK_GNUC(4, 8, 0)
#       define NX_SP_CXX11_INHERITING
#       define NX_SP_CXX11_ALIGNMENT
#   endif

#endif/*(defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus >= 201103L))*/

#elif defined(NX_CC_CLANG)
/*
    C++ front-end for the LLVM compiler

    Start from Clang 2.9 (with Rvalue references)
    So I can just use __cxx_rvalue_references__ to check C++11 support.
    See: http://clang.llvm.org/cxx_status.html
*/
#if __has_feature(__cxx_rvalue_references__)

#   if NX_CHECK_CLANG(2, 9, 0)
#       define NX_SP_CXX11_TYPE_TRAITS
#       define NX_SP_CXX11_RESULT_OF
#       define NX_SP_CXX11_CHAR_TYPE
#       define NX_SP_CXX11_MUTEX
#       define NX_SP_CXX11_CONDITION
#       define NX_SP_CXX11_THREAD
#       define NX_SP_CXX11_TUPLE
#       define NX_SP_CXX11_STACK_SWAP
#       define NX_SP_CXX11_ARRAY
#       define NX_SP_CXX11_FORWARD_LIST
#       define NX_SP_CXX11_UNORDERED_MAP
#       define NX_SP_CXX11_UNORDERED_SET
#       define NX_SP_CXX11_AUTO
#       define NX_SP_CXX11_RVALUE_REF
#       define NX_SP_CXX11_PERFECT_FWD
#       define NX_SP_CXX11_SATIC_ASSERT
#       define NX_SP_CXX11_TEMPLATES
#       define NX_SP_CXX11_TEMPLATES_EX
#   endif
#   if NX_CHECK_CLANG(3, 0, 0)
#       define NX_SP_CXX11_NULLPTR
#       define NX_SP_CXX11_ALIAS
#       define NX_SP_CXX11_RANGEFOR
#       define NX_SP_CXX11_NOEXCEPT
#       define NX_SP_CXX11_DELEGATING
#   endif
#   if NX_CHECK_CLANG(3, 1, 0)
#       define NX_SP_CXX11_ATOMIC
#       define NX_SP_CXX11_INITIALIZER
#       define NX_SP_CXX11_LAMBDA
#   endif
#   if NX_CHECK_CLANG(3, 3, 0)
#       define NX_SP_CXX11_INHERITING
#       define NX_SP_CXX11_ALIGNMENT
#   endif

#endif/*__has_feature(__cxx_rvalue_references__)*/

#endif

#endif/*NX_NO_DETECT_SP*/

/*
    The switch of C++ features
*/

#ifdef NX_NO_CXX11_TYPE_TRAITS
#undef NX_SP_CXX11_TYPE_TRAITS
#endif
#ifdef NX_NO_CXX11_CHAR_TYPE
#undef NX_SP_CXX11_CHAR_TYPE
#endif
#ifdef NX_NO_CXX11_ATOMIC
#undef NX_SP_CXX11_ATOMIC
#endif
#ifdef NX_NO_CXX11_MUTEX
#undef NX_SP_CXX11_MUTEX
#endif
#ifdef NX_NO_CXX11_CONDITION
#undef NX_SP_CXX11_CONDITION
#endif
#ifdef NX_NO_CXX11_THREAD
#undef NX_SP_CXX11_THREAD
#endif
#ifdef NX_NO_CXX11_RESULT_OF
#undef NX_SP_CXX11_RESULT_OF
#endif
#ifdef NX_NO_CXX11_TUPLE
#undef NX_SP_CXX11_TUPLE
#endif
#ifdef NX_NO_CXX11_STACK_SWAP
#undef NX_SP_CXX11_STACK_SWAP
#endif
#ifdef NX_NO_CXX11_ARRAY
#undef NX_SP_CXX11_ARRAY
#endif
#ifdef NX_NO_CXX11_FORWARD_LIST
#undef NX_SP_CXX11_FORWARD_LIST
#endif
#ifdef NX_NO_CXX11_UNORDERED_MAP
#undef NX_SP_CXX11_UNORDERED_MAP
#endif
#ifdef NX_NO_CXX11_UNORDERED_SET
#undef NX_SP_CXX11_UNORDERED_SET
#endif
#ifdef NX_NO_CXX11_NULLPTR
#undef NX_SP_CXX11_NULLPTR
#endif
#ifdef NX_NO_CXX11_AUTO
#undef NX_SP_CXX11_AUTO
#endif
#ifdef NX_NO_CXX11_RVALUE_REF
#undef NX_SP_CXX11_RVALUE_REF
#endif
#ifdef NX_NO_CXX11_PERFECT_FWD
#undef NX_SP_CXX11_PERFECT_FWD
#endif
#ifdef NX_NO_CXX11_SATIC_ASSERT
#undef NX_SP_CXX11_SATIC_ASSERT
#endif
#ifdef NX_NO_CXX11_ALIAS
#undef NX_SP_CXX11_ALIAS
#endif
#ifdef NX_NO_CXX11_RANGEFOR
#undef NX_SP_CXX11_RANGEFOR
#endif
#ifdef NX_NO_CXX11_NOEXCEPT
#undef NX_SP_CXX11_NOEXCEPT
#endif
#ifdef NX_NO_CXX11_TEMPLATES
#undef NX_SP_CXX11_TEMPLATES
#endif
#ifdef NX_NO_CXX11_TEMPLATES_EX
#undef NX_SP_CXX11_TEMPLATES_EX
#endif
#ifdef NX_NO_CXX11_DELEGATING
#undef NX_SP_CXX11_DELEGATING
#endif
#ifdef NX_NO_CXX11_INHERITING
#undef NX_SP_CXX11_INHERITING
#endif
#ifdef NX_NO_CXX11_ALIGNMENT
#undef NX_SP_CXX11_ALIGNMENT
#endif
#ifdef NX_NO_CXX11_INITIALIZER
#undef NX_SP_CXX11_INITIALIZER
#endif
#ifdef NX_NO_CXX11_LAMBDA
#undef NX_SP_CXX11_LAMBDA
#endif

//////////////////////////////////////////////////////////////////////////
