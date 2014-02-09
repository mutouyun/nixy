/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/general/general.h"
#include "nixycore/utility/utility.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

template <typename Model_>
struct iterator : public Model_, nx_operator(iterator<Model_>, Operable)
{
    typedef typename Model_::type_t type_t;

    iterator(int v = 0)
    {
        Model_::operator()(v);
    }

    iterator& operator+=(int v)
    {
        Model_::operator()(v);
        return (*this);
    }

    type_t value(void) const
    {
        return Model_::value();
    }

    type_t operator*(void) const { return value(); }
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
