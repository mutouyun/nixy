#ifndef POLICY_VC8_H
#define POLICY_VC8_H

#include "policy_vc_base.h"

//////////////////////////////////////////////////////////////////////////

struct PolicyVC8 : PolicyVCBase
{
    /*
        Configuration Creator
    */

    template <typename P>
    QDomElement createConfiguration(Config& cfg, const Project& prj, QDomDocument& doc)
    {
        solution_t& solution = cfg.solution();
        const QString& cc = cfg.cc();

        QDomElement node = doc.createElement("Configuration");
        node.setAttribute("Name", P::name());
        {
            QString path("$(SolutionDir)");
            path += solution.out_path_;
            path.replace("$(Configuration)", "$(ConfigurationName)");
            path.replace("$(CC)", cc);
            path += "\\";
            node.setAttribute("OutputDirectory", path);
        }
        {
            QString path("$(SolutionDir)");
            path += solution.tmp_path_;
            path.replace("$(Configuration)", "$(ConfigurationName)");
            path.replace("$(CC)", cc);
            path += "\\$(RootNamespace)\\";
            node.setAttribute("IntermediateDirectory", path);
        }
        {
            int type = 1;
            if (prj.type_ == "dll")
                type = 2;
            else if (prj.type_ == "lib")
                type = 4;
            node.setAttribute("ConfigurationType", type);
        }
        node.setAttribute("CharacterSet", 1);
#define DEF_DOMELEMENT_TOOL(name, ...) \
        { \
            QDomElement tool = doc.createElement("Tool"); \
            tool.setAttribute("Name", name); \
            __VA_ARGS__ \
            P::process(tool); \
            node.appendChild(tool); \
        }
        DEF_DOMELEMENT_TOOL("VCPreBuildEventTool")
        DEF_DOMELEMENT_TOOL("VCCustomBuildTool")
        DEF_DOMELEMENT_TOOL("VCXMLDataGeneratorTool")
        DEF_DOMELEMENT_TOOL("VCWebServiceProxyGeneratorTool")
        DEF_DOMELEMENT_TOOL("VCMIDLTool")
        DEF_DOMELEMENT_TOOL("VCCLCompilerTool",
            tool.setAttribute("AdditionalIncludeDirectories", createIncludePaths(solution, prj));
            tool.setAttribute("PreprocessorDefinitions"     , createDefines     (solution, prj));
        )
        DEF_DOMELEMENT_TOOL("VCManagedResourceCompilerTool")
        DEF_DOMELEMENT_TOOL("VCResourceCompilerTool")
        DEF_DOMELEMENT_TOOL("VCPreLinkEventTool")
        if (prj.type_ == "lib")
            DEF_DOMELEMENT_TOOL("VCLibrarianTool",
                tool.setAttribute("AdditionalDependencies", prj.libs_);
            )
        else
        {
            DEF_DOMELEMENT_TOOL("VCLinkerTool",
                if (prj.type_ == "console")
                    tool.setAttribute("SubSystem", 1);
                else
                    tool.setAttribute("SubSystem", 2);
                tool.setAttribute("GenerateDebugInformation", "true");
                {
                    QString cons;
                    foreach(QString con, prj.depends_)
                    {
                        cons += "\"$(OutDir)";
                        cons += con;
                        cons += ".lib\" ";
                    }
                    cons += prj.libs_;
                    tool.setAttribute("AdditionalDependencies", cons);
                }
            )
            DEF_DOMELEMENT_TOOL("VCManifestTool")
            DEF_DOMELEMENT_TOOL("VCAppVerifierTool")
        }
        DEF_DOMELEMENT_TOOL("VCALinkTool")
        DEF_DOMELEMENT_TOOL("VCXDCMakeTool")
        DEF_DOMELEMENT_TOOL("VCBscMakeTool")
        DEF_DOMELEMENT_TOOL("VCFxCopTool")
        DEF_DOMELEMENT_TOOL("VCPostBuildEventTool")
#undef  DEF_DOMELEMENT_TOOL
        P::process(node);
        return node;
    }

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
                element.setAttribute("Detect64BitPortabilityProblems", "true");
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
                element.setAttribute("RuntimeLibrary", 2);
                element.setAttribute("UsePrecompiledHeader", 0);
                element.setAttribute("WarningLevel", 3);
                element.setAttribute("Detect64BitPortabilityProblems", "true");
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

    struct Win32
    {
        static const char* name(void)
        {
            return "Win32";
        }
        static void process(QDomElement& element)
        {
            if (element.attribute("Name") == "VCCLCompilerTool")
            {
                QString defs("WIN32");
                defs += element.attribute("PreprocessorDefinitions");
                element.setAttribute("PreprocessorDefinitions", defs);
            }
            else if (element.attribute("Name") == "VCLinkerTool")
            {
                element.setAttribute("TargetMachine", 1);
            }
        }
    };

    struct x64
    {
        static const char* name(void)
        {
            return "x64";
        }
        static void process(QDomElement& element)
        {
            if (element.attribute("Name") == "VCCLCompilerTool")
            {
                if (element.attribute("Name") == "VCMIDLTool")
                {
                    element.setAttribute("TargetEnvironment", 3);
                }
                else if (element.attribute("Name") == "VCCLCompilerTool")
                {
                    QString defs("WIN64");
                    defs += element.attribute("PreprocessorDefinitions");
                    element.setAttribute("PreprocessorDefinitions", defs);
                }
                else if (element.attribute("Name") == "VCLinkerTool")
                {
                    element.setAttribute("TargetMachine", 17);
                }
            }
        }
    };

    template <typename Mode_, typename Plat_>
    struct Platform
    {
        static QString name(void)
        {
            return QString("%1|%2").arg(Mode_::name())
                                   .arg(Plat_::name());
        }
        static void process(QDomElement& element)
        {
            Mode_::process(element);
            Plat_::process(element);
        }
    };

    /*
        Filter Creator
    */

    void createFilter(Config& cfg, const Project& prj, QDomDocument& doc, QDomElement& pare)
    {
#define DEF_DOMELEMENT_FILE(file_path, filter) \
        { \
            QDomElement file = doc.createElement("File"); \
            file.setAttribute("RelativePath", file_path); \
            filter.appendChild(file); \
        }

        QMap<QString, QList<QString> > filters;
        const QList<QString>* (files[2]) = {&(prj.heads_), &(prj.sources_)};
        for(int i = 0; i < 2; ++i)
        {
            foreach(QString con, *(files[i]))
            {
                QString tmp = createFilterName(cfg.solution(), con);
                if (tmp.isEmpty())
                    DEF_DOMELEMENT_FILE(con, pare)
                else
                    filters[tmp].append(con);
            }
        }

        foreach(QString key, filters.keys())
        {
            QDomElement filter = doc.createElement("Filter");
            filter.setAttribute("Name", key);
            foreach(QString con, filters.value(key))
            {
                DEF_DOMELEMENT_FILE(con, filter)
            }
            pare.appendChild(filter);
        }

#undef  DEF_DOMELEMENT_FILE
    }

    /*
        The options of createRoot
    */

    QDomElement makeRoot(Config& /*cfg*/, const Project& prj, QDomDocument& doc)
    {
        QDomElement root = doc.createElement("VisualStudioProject");
        root.setAttribute("ProjectType"    , "Visual C++");
        root.setAttribute("Version"        , "8.00");
        root.setAttribute("Name"           , prj.name_);
        root.setAttribute("ProjectGUID"    , prj.GUID_);
        root.setAttribute("RootNamespace"  , prj.name_);
        root.setAttribute("Keyword"        , "Win32Proj");
        return root;
    }

    QDomElement makePlatforms(Config& /*cfg*/, const Project& /*prj*/, QDomDocument& doc, QDomElement& /*root*/)
    {
        QDomElement node = doc.createElement("Platforms");
        {
            QDomElement leaf = doc.createElement("Platform");
            leaf.setAttribute("Name", "Win32");
            node.appendChild(leaf);
        }
        {
            QDomElement leaf = doc.createElement("Platform");
            leaf.setAttribute("Name", "x64");
            node.appendChild(leaf);
        }
        return node;
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

    QDomElement makeFilters(Config& cfg, const Project& prj, QDomDocument& doc, QDomElement& /*root*/)
    {
        QDomElement node = doc.createElement("Files");
        createFilter(cfg, prj, doc, node);
        return node;
    }

    /*
        Version of Solution File
    */

    const char* version(void)
    {
        return "Microsoft Visual Studio Solution File, Format Version 9.00\n# Visual Studio 2005";
    }

    /*
        The project file expanded name
    */

    const char* form(void)
    {
        return ".vcproj";
    }
};

//////////////////////////////////////////////////////////////////////////

#endif // POLICY_VC8_H
