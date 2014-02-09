/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/general/general.h"
#include "nixycore/typemanip/typemanip.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

namespace private_final
{
    template <class P, class T>
    class detail
    {
        NX_FRIEND_CLASS(P);
        NX_FRIEND_CLASS(T);

    private:
        detail() {}
        ~detail() {}
    };
}

/*
    Final class interface
*/

template <class T>
class Final : virtual public private_final::detail<Final<T>, T>
{
public:
    Final() {}
    ~Final() {}
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
