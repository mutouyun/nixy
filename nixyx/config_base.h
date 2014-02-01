#ifndef CONFIG_BASE_H
#define CONFIG_BASE_H

#include "solution.h"
#include "config.h"
#include "tools.h"

//////////////////////////////////////////////////////////////////////////

class ConfigBase
{
public:
    virtual bool load(const QString&) = 0;

public:
    virtual ~ConfigBase() {}
};

//////////////////////////////////////////////////////////////////////////

#endif // CONFIG_BASE_H
