/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/alloc.h"

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"
#include "nixycore/utility/utility.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

NX_CONCEPT(clone, U*, clone)

template <typename T>
inline typename enable_if<has_clone<T>::value, 
T*>::type_t clone(T* ob)
{
    if (!ob) return nx::nulptr;
    return static_cast<T*>(ob->clone());
}

template <typename T>
inline typename enable_if<!has_clone<T>::value, 
T*>::type_t clone(T* ob)
{
    if (!ob) return nx::nulptr;
    return nx::alloc<T>(nx::ref(*ob));
}

template <typename Alloc_, typename T>
T* clone(T* ob)
{
    if (!ob) return nx::nulptr;
    return nx::alloc<Alloc_, T>(nx::ref(*ob));
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
