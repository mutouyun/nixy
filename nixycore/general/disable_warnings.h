/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

//////////////////////////////////////////////////////////////////////////
#ifndef _NIXY_CORE_DISABLE_WARNINGS_HXX_
#define _NIXY_CORE_DISABLE_WARNINGS_HXX_
//////////////////////////////////////////////////////////////////////////

#if defined(NX_CC_MSVC)
#   pragma warning(push)
#   pragma warning(disable: 4800)   // forcing value to bool 'true' or 'false' (performance warning)
#   pragma warning(disable: 4311)   // pointer truncation from 'type1' to 'type2'
#   pragma warning(disable: 4312)   // conversion from 'type1' to 'type2' of greater size
#   pragma warning(disable: 4355)   // 'this' : used in base member initializer list
#endif

//////////////////////////////////////////////////////////////////////////
#else
//////////////////////////////////////////////////////////////////////////

#if defined(NX_CC_MSVC)
#   pragma warning(pop)
#endif

//////////////////////////////////////////////////////////////////////////
#undef  _NIXY_CORE_DISABLE_WARNINGS_HXX_
#endif/*_NIXY_CORE_DISABLE_WARNINGS_HXX_*/
//////////////////////////////////////////////////////////////////////////
