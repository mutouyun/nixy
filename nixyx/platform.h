#ifndef PLATFORM_H
#define PLATFORM_H

#include "config.h"

//////////////////////////////////////////////////////////////////////////

class Platform
{
public:
    Platform(Config&);

    bool make(void);

private:
    struct Impl_;
    Impl_* pimpl_;

public:
    virtual ~Platform();
};

//////////////////////////////////////////////////////////////////////////

#endif // PLATFORM_H
