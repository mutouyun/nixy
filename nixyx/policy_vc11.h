#ifndef POLICY_VC11_H
#define POLICY_VC11_H

#include "policy_vc10.h"

//////////////////////////////////////////////////////////////////////////

struct PolicyVC11 : PolicyVC10
{
    /*
        Configuration PlatformToolset
    */

    void decorateConfiguration(QDomElement& prop, QDomDocument& doc)
    {
        prop.appendChild(doc.createElement("PlatformToolset"))
            .appendChild(doc.createTextNode("v110"));
    }

    /*
        Version of Solution File
    */

    const char* version(void)
    {
        return "Microsoft Visual Studio Solution File, Format Version 12.00\n# Visual Studio 2012";
    }
};

//////////////////////////////////////////////////////////////////////////

#endif // POLICY_VC11_H
