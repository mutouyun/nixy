/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/memory/construct.h"
#include "nixycore/memory/alloc_model.h"
#include "nixycore/memory/alloc.h"

#include "nixycore/general/general.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

/*
    standard alloc policy model
*/

struct std_alloc_model
{
    static pvoid alloc(size_t size)
    { return (size ? ::malloc(size) : NULL); }
    static void free(pvoid p, size_t /*size*/)
    { if (p) ::free(p); }
    static pvoid realloc(pvoid p, size_t old_size, size_t new_size)
    { return (((p && old_size) || new_size) ? ::realloc(p, new_size) : NULL); }
};

namespace use
{
    typedef alloc_model<std_alloc_model> alloc_std;
}

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
