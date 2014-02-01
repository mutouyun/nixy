#ifndef POLICY_VC10_H
#define POLICY_VC10_H

#include "policy_vc_base.h"
#include "tools.h"

//////////////////////////////////////////////////////////////////////////

struct PolicyVC10 : PolicyVCBase
{
    /*
        Configuration Creator
    */

    virtual void decorateConfiguration(QDomElement& /*prop*/, QDomDocument& /*doc*/) {}

#define DEF_DOMELEMENT(name, ...) \
        { \
            QDomElement node = doc.createElement(name); \
            __VA_ARGS__ \
            P::process(node, doc); \
            prop.appendChild(node); \
        }

    template <typename P>
    void createConfigurationHead(Config& /*cfg*/, const Project& prj, QDomDocument& doc, QDomElement& root)
    {
        QString con_name(QString("\'$(Configuration)|$(Platform)\'==\'%1\'").arg(P::name()));
        {
            QDomElement prop = doc.createElement("PropertyGroup");
            prop.setAttribute("Condition", con_name);
            prop.setAttribute("Label", "Configuration");

            DEF_DOMELEMENT("ConfigurationType",
                QString type;
                if (prj.type_ == "lib")
                   type = "StaticLibrary";
                else if (prj.type_ == "dll")
                   type = "DynamicLibrary";
                else
                   type = "Application";
                node.appendChild(doc.createTextNode(type));
            )
            DEF_DOMELEMENT("UseDebugLibraries")
            DEF_DOMELEMENT("CharacterSet",
                node.appendChild(doc.createTextNode("Unicode"));
            )

            decorateConfiguration(prop, doc);

            P::process(prop, doc);
            root.appendChild(prop);
        }
    }

    template <typename P>
    void createConfiguration(Config& cfg, const Project& prj, QDomDocument& doc, QDomElement& root)
    {
        solution_t& solution = cfg.solution();
        const QString& cc = cfg.cc();

        QString con_name(QString("\'$(Configuration)|$(Platform)\'==\'%1\'").arg(P::name()));
        {
            QDomElement prop = doc.createElement("ImportGroup");
            prop.setAttribute("Condition", con_name);
            prop.setAttribute("Label", "PropertySheets");

            DEF_DOMELEMENT("Import",
                node.setAttribute("Project", "$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props");
                node.setAttribute("Condition", "exists(\'$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\')");
                node.setAttribute("Label", "LocalAppDataPlatform");
            )

            P::process(prop, doc);
            root.appendChild(prop);
        }
        {
            QDomElement prop = doc.createElement("PropertyGroup");
            prop.setAttribute("Condition", con_name);

            DEF_DOMELEMENT("OutDir",
                QString path("$(SolutionDir)");
                path += solution.out_path_;
                path.replace("$(CC)", cc);
                path += "\\";
                node.appendChild(doc.createTextNode(path));
            )
            DEF_DOMELEMENT("IntDir",
                QString path("$(SolutionDir)");
                path += solution.tmp_path_;
                path.replace("$(CC)", cc);
                path += "\\$(RootNamespace)\\";
                node.appendChild(doc.createTextNode(path));
            )
            DEF_DOMELEMENT("TargetName",
                node.appendChild(doc.createTextNode("$(RootNamespace)"));
            )

            P::process(prop, doc);
            root.appendChild(prop);
        }
        {
            QDomElement prop = doc.createElement("ItemDefinitionGroup");
            prop.setAttribute("Condition", con_name);

            DEF_DOMELEMENT("ClCompile",
                node.appendChild(doc.createElement("PrecompiledHeader"))
                    .appendChild(doc.createTextNode("NotUsing"));
                node.appendChild(doc.createElement("WarningLevel"))
                    .appendChild(doc.createTextNode("Level3"));
                {
                    QDomElement leaf = doc.createElement("PreprocessorDefinitions");
                    leaf.appendChild(doc.createTextNode(
                                     createDefines(solution, prj) + ";%(PreprocessorDefinitions)"));
                    P::process(leaf, doc);
                    node.appendChild(leaf);
                }
                {
                    QDomElement leaf = doc.createElement("AdditionalIncludeDirectories");
                    leaf.appendChild(doc.createTextNode(
                                     createIncludePaths(solution, prj) + ";%(AdditionalIncludeDirectories)"));
                    P::process(leaf, doc);
                    node.appendChild(leaf);
                }
            )
            DEF_DOMELEMENT("Link",
                if (prj.type_ == "console")
                    node.appendChild(doc.createElement("SubSystem"))
                        .appendChild(doc.createTextNode("Console"));
                else
                    node.appendChild(doc.createElement("SubSystem"))
                        .appendChild(doc.createTextNode("Windows"));
                node.appendChild(doc.createElement("GenerateDebugInformation"))
                    .appendChild(doc.createTextNode("true"));
                if (prj.type_ != "lib")
                {
                    QString cons;
                    foreach(QString con, prj.depends_)
                    {
                        cons += "\"$(OutDir)";
                        cons += con;
                        cons += ".lib\";";
                    }
                    if (!prj.libs_.isEmpty())
                    {
                        cons += prj.libs_;
                        cons += ';';
                    }
                    cons += "%(AdditionalDependencies)";
                    node.appendChild(doc.createElement("AdditionalDependencies"))
                        .appendChild(doc.createTextNode(cons));
                }
            )
            if (prj.type_ == "lib")
            {
                DEF_DOMELEMENT("Lib",
                    node.appendChild(doc.createElement("AdditionalDependencies"))
                        .appendChild(doc.createTextNode(prj.libs_));
                )
            }

            P::process(prop, doc);
            root.appendChild(prop);
        }
    }

#undef  DEF_DOMELEMENT

    /*
        Configurations of Debug/Release/Win32/x64
    */

    struct Debug
    {
        static const char* name(void)
        {
            return "Debug";
        }
        static void process(QDomElement& element, QDomDocument& doc)
        {
            if (element.tagName() == "UseDebugLibraries")
            {
                element.appendChild(doc.createTextNode("true"));
            }
            else if (element.tagName() == "PreprocessorDefinitions")
            {
                element.namedItem("#text").toText().setData(";_DEBUG" + element.text());
            }
            else if (element.tagName() == "ClCompile")
            {
                element.appendChild(doc.createElement("Optimization"))
                       .appendChild(doc.createTextNode("Disabled"));
            }
        }
    };

    struct Release
    {
        static const char* name(void)
        {
            return "Release";
        }
        static void process(QDomElement& element, QDomDocument& doc)
        {
            if (element.tagName() == "UseDebugLibraries")
            {
                element.appendChild(doc.createTextNode("false"));
            }
            else if (element.tagName() == "PropertyGroup" &&
                     element.attribute("Label") == "Configuration")
            {
                element.appendChild(doc.createElement("WholeProgramOptimization"))
                       .appendChild(doc.createTextNode("true"));
            }
            else if (element.tagName() == "PreprocessorDefinitions")
            {
                element.namedItem("#text").toText().setData(";NDEBUG" + element.text());
            }
            else if (element.tagName() == "ClCompile")
            {
                element.appendChild(doc.createElement("Optimization"))
                       .appendChild(doc.createTextNode("MaxSpeed"));
                element.appendChild(doc.createElement("FunctionLevelLinking"))
                       .appendChild(doc.createTextNode("true"));
                element.appendChild(doc.createElement("IntrinsicFunctions"))
                       .appendChild(doc.createTextNode("true"));
            }
            else if (element.tagName() == "Link")
            {
                element.appendChild(doc.createElement("EnableCOMDATFolding"))
                       .appendChild(doc.createTextNode("true"));
                element.appendChild(doc.createElement("OptimizeReferences"))
                       .appendChild(doc.createTextNode("true"));
            }
        }
    };

    struct Win32
    {
        static const char* name(void)
        {
            return "Win32";
        }
        static void process(QDomElement& element, QDomDocument& /*doc*/)
        {
            if (element.tagName() == "PreprocessorDefinitions")
            {
                element.namedItem("#text").toText().setData("WIN32" + element.text());
            }
        }
    };

    struct x64
    {
        static const char* name(void)
        {
            return "x64";
        }
        static void process(QDomElement& element, QDomDocument& /*doc*/)
        {
            if (element.tagName() == "PreprocessorDefinitions")
            {
                element.namedItem("#text").toText().setData("WIN64" + element.text());
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
        static void process(QDomElement& element, QDomDocument& doc)
        {
            Mode_::process(element, doc);
            Plat_::process(element, doc);
        }
    };

    /*
        The options of createRoot
    */

    QDomElement makeRoot(Config& /*cfg*/, const Project& prj, QDomDocument& doc)
    {
        QDomElement root = doc.createElement("Project");
        root.setAttribute("DefaultTargets", "Build");
        root.setAttribute("ToolsVersion"  , "4.0");
        root.setAttribute("xmlns"         , "http://schemas.microsoft.com/developer/msbuild/2003");
        {
            QDomElement node = doc.createElement("PropertyGroup");
            node.setAttribute("Label", "Globals");
            {
                QDomElement leaf = doc.createElement("ProjectGuid");
                leaf.appendChild(doc.createTextNode(prj.GUID_));
                node.appendChild(leaf);
            }
            {
                QDomElement leaf = doc.createElement("Keyword");
                leaf.appendChild(doc.createTextNode("Win32Proj"));
                node.appendChild(leaf);
            }
            {
                QDomElement leaf = doc.createElement("RootNamespace");
                leaf.appendChild(doc.createTextNode(prj.name_));
                node.appendChild(leaf);
            }
            root.appendChild(node);
        }
        {
            QDomElement node = doc.createElement("Import");
            node.setAttribute("Project", "$(VCTargetsPath)\\Microsoft.Cpp.Default.props");
            root.appendChild(node);
        }
        return root;
    }

    QDomElement makePlatforms(Config& /*cfg*/, const Project& /*prj*/, QDomDocument& doc, QDomElement& /*root*/)
    {
        QDomElement node = doc.createElement("ItemGroup");
        node.setAttribute("Label", "ProjectConfigurations");
#define DEF_PROJECT_CONFIGURATION(config, plat) \
        { \
            QDomElement leaf = doc.createElement("ProjectConfiguration"); \
            leaf.setAttribute("Include", DEF_STR(DEF_ARGS(config)|DEF_ARGS(plat))); \
            { \
                QDomElement node = doc.createElement("Configuration"); \
                node.appendChild(doc.createTextNode(DEF_STR(config))); \
                leaf.appendChild(node); \
            } \
            { \
                QDomElement node = doc.createElement("Platform"); \
                node.appendChild(doc.createTextNode(DEF_STR(plat))); \
                leaf.appendChild(node); \
            } \
            node.appendChild(leaf); \
        }
        DEF_PROJECT_CONFIGURATION(Debug  , Win32)
        DEF_PROJECT_CONFIGURATION(Debug  , x64)
        DEF_PROJECT_CONFIGURATION(Release, Win32)
        DEF_PROJECT_CONFIGURATION(Release, x64)
#undef  DEF_PROJECT_CONFIGURATION
        return node;
    }

    QDomElement makeConfigurations(Config& cfg, const Project& prj, QDomDocument& doc, QDomElement& root)
    {
        createConfigurationHead<Platform<Debug  , Win32> >(cfg, prj, doc, root);
        createConfigurationHead<Platform<Debug  , x64>   >(cfg, prj, doc, root);
        createConfigurationHead<Platform<Release, Win32> >(cfg, prj, doc, root);
        createConfigurationHead<Platform<Release, x64>   >(cfg, prj, doc, root);
        {
            QDomElement node = doc.createElement("Import");
            node.setAttribute("Project", "$(VCTargetsPath)\\Microsoft.Cpp.props");
            root.appendChild(node);
        }
        createConfiguration<Platform<Debug  , Win32> >(cfg, prj, doc, root);
        createConfiguration<Platform<Debug  , x64>   >(cfg, prj, doc, root);
        createConfiguration<Platform<Release, Win32> >(cfg, prj, doc, root);
        createConfiguration<Platform<Release, x64>   >(cfg, prj, doc, root);
        return QDomElement();
    }

    bool createFiltersFile(const Project& prj, QSet<QString>& filters, QMap<QString, QString>(& files_map)[2])
    {
        QFile filter_file(QString("%1%2%3").arg(prj.name_).arg(form()).arg(".filters"));
        if (!filter_file.open(QIODevice::WriteOnly |
                              QIODevice::Truncate)) return false;

        QDomDocument doc;
        doc.appendChild(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));

        QDomElement root = doc.createElement("Project");
        root.setAttribute("ToolsVersion"  , "4.0");
        root.setAttribute("xmlns"         , "http://schemas.microsoft.com/developer/msbuild/2003");
        {
            QDomElement pare = doc.createElement("ItemGroup");
            foreach(QString key, filters.values())
            {
                if (key == "") continue;
                QDomElement filter = doc.createElement("Filter");
                filter.setAttribute("Include", key);
                filter.appendChild(doc.createElement("UniqueIdentifier"))
                      .appendChild(doc.createTextNode(GUID()));
                pare.appendChild(filter);
            }
            if (pare.childNodes().count() > 0)
                root.appendChild(pare);
        }
        const char* (tags[2]) = {"ClInclude", "ClCompile"};
        for(int i = 0; i < 2; ++i)
        {
            QDomElement pare = doc.createElement("ItemGroup");
            foreach(QString con, files_map[i].keys())
            {
                QDomElement node = doc.createElement(tags[i]);
                node.setAttribute("Include", con);
                QString key = files_map[i][con];
                if (key != "")
                {
                    node.appendChild(doc.createElement("Filter"))
                        .appendChild(doc.createTextNode(key));
                }
                pare.appendChild(node);
            }
            root.appendChild(pare);
        }

        doc.appendChild(root);
        QTextStream stream(&filter_file);
        doc.save(stream, 4);

        return true;
    }

    QDomElement makeFilters(Config& cfg, const Project& prj, QDomDocument& doc, QDomElement& root)
    {
        const QList<QString>* (files[2]) = {&(prj.heads_), &(prj.sources_)};
        const char*           (tags[2])  = {"ClInclude"  , "ClCompile"};

        QSet<QString> filters;
        QMap<QString, QString> files_map[2];

        for(int i = 0; i < 2; ++i)
        {
            QDomElement node = doc.createElement("ItemGroup");
            foreach(QString con, *(files[i]))
            {
                QString tmp = createFilterName(cfg.solution(), con);
                if (tmp.isEmpty())
                    files_map[i][con] = "";
                else
                    files_map[i][con] = *(filters.insert(tmp));

                node.appendChild(doc.createElement(tags[i]))
                    .toElement().setAttribute("Include", con);
            }
            root.appendChild(node);
        }

        /* Create .vcxproj.filters */
        createFiltersFile(prj, filters, files_map);

        {
            QDomElement node = doc.createElement("Import");
            node.setAttribute("Project", "$(VCTargetsPath)\\Microsoft.Cpp.targets");
            root.appendChild(node);
        }
        return QDomElement();
    }

    /*
        Version of Solution File
    */

    const char* version(void)
    {
        return "Microsoft Visual Studio Solution File, Format Version 11.00\n# Visual Studio 2010";
    }

    /*
        The project file expanded name
    */

    const char* form(void)
    {
        return ".vcxproj";
    }
};

//////////////////////////////////////////////////////////////////////////

#endif // POLICY_VC10_H
