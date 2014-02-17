/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "nixycore/typemanip/typetools.h"

#include "nixycore/general/general.h"

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
    final class interface
*/

template <class T>
class final : virtual public private_final::detail<final<T>, T>
{
public:
    final() {}
    ~final() {}
};

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////
