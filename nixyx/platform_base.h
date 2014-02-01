#ifndef PLATFORM_BASE_H
#define PLATFORM_BASE_H

#include "config.h"
#include "solution.h"

//////////////////////////////////////////////////////////////////////////

class PlatformBase
{
public:
    virtual bool make(Config& cfg)
    {
         return cfg.isValid();
    }

public:
    virtual ~PlatformBase() {}
};

//////////////////////////////////////////////////////////////////////////

#endif // PLATFORM_BASE_H
