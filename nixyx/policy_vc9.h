#ifndef POLICY_VC9_H
#define POLICY_VC9_H

#include "policy_vc8.h"

//////////////////////////////////////////////////////////////////////////

struct PolicyVC9 : PolicyVC8
{
    /*
        Configurations of Debug/Release/Win32/x64
    */

    struct Debug
    {
        static const char* name(void)
        {
            return "Debug";
        }
        static void process(QDomElement& element)
        {
            if (element.attribute("Name") == "VCCLCompilerTool")
            {
                element.setAttribute("Optimization", 0);
                element.setAttribute("MinimalRebuild", "true");
                element.setAttribute("BasicRuntimeChecks", 3);
                element.setAttribute("RuntimeLibrary", 3);
                element.setAttribute("UsePrecompiledHeader", 0);
                element.setAttribute("WarningLevel", 3);
                element.setAttribute("DebugInformationFormat", 4);

                QString defs(";_DEBUG");
                defs += element.attribute("PreprocessorDefinitions");
                element.setAttribute("PreprocessorDefinitions", defs);
            }
            else if (element.attribute("Name") == "VCLinkerTool")
            {
                element.setAttribute("LinkIncremental", 2);
            }
        }
    };

    struct Release
    {
        static const char* name(void)
        {
            return "Release";
        }
        static void process(QDomElement& element)
        {
            if (element.tagName() == "Configuration")
            {
                element.setAttribute("WholeProgramOptimization", 1);
            }
            else if (element.attribute("Name") == "VCCLCompilerTool")
            {
                element.setAttribute("Optimization", 2);
                element.setAttribute("EnableIntrinsicFunctions", "true");
                element.setAttribute("RuntimeLibrary", 2);
                element.setAttribute("EnableFunctionLevelLinking", "true");
                element.setAttribute("UsePrecompiledHeader", 0);
                element.setAttribute("WarningLevel", 3);
                element.setAttribute("DebugInformationFormat", 3);

                QString defs(";NDEBUG");
                defs += element.attribute("PreprocessorDefinitions");
                element.setAttribute("PreprocessorDefinitions", defs);
            }
            else if (element.attribute("Name") == "VCLinkerTool")
            {
                element.setAttribute("LinkIncremental", 1);
                element.setAttribute("OptimizeReferences", 2);
                element.setAttribute("EnableCOMDATFolding", 2);
            }
        }
    };

    /*
        The options of root maker
    */

    QDomElement makeRoot(Config& cfg, const Project& prj, QDomDocument& doc)
    {
        QDomElement root = PolicyVC8::makeRoot(cfg, prj, doc);
        root.setAttribute("Version", "9.00");
        root.setAttribute("TargetFrameworkVersion", "196613");
        return root;
    }

    QDomElement makeConfigurations(Config& cfg, const Project& prj, QDomDocument& doc, QDomElement& /*root*/)
    {
        QDomElement node = doc.createElement("Configurations");
        node.appendChild(createConfiguration<Platform<Debug  , Win32> >(cfg, prj, doc));
        node.appendChild(createConfiguration<Platform<Debug  , x64>   >(cfg, prj, doc));
        node.appendChild(createConfiguration<Platform<Release, Win32> >(cfg, prj, doc));
        node.appendChild(createConfiguration<Platform<Release, x64>   >(cfg, prj, doc));
        return node;
    }

    /*
        Version of Solution File
    */

    const char* version(void)
    {
        return "Microsoft Visual Studio Solution File, Format Version 10.00\n# Visual Studio 2008";
    }
};

//////////////////////////////////////////////////////////////////////////

#endif // POLICY_VC9_H
