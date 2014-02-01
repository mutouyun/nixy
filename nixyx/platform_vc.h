#ifndef PLATFORM_VC_H
#define PLATFORM_VC_H

#include "platform_base.h"

#include <QFile>
#include <QTextStream>
#include <QDomDocument>

//////////////////////////////////////////////////////////////////////////

template <typename Policy_>
class PlatformVC : public PlatformBase, public Policy_
{
protected:

    /*
        Solution(".sln") Maker
    */

    bool makeSolution(Config& cfg)
    {
        solution_t& solution = cfg.solution();
        projects_t& projects = cfg.projects();

        QFile sln_file(solution.name_ + ".sln");
        if (!sln_file.open(QIODevice::WriteOnly |
                           QIODevice::Truncate)) return false;

        QString sln_GUID(Policy_::GUID());

        QTextStream stream(&sln_file);
        stream << "\n" << Policy_::version() << "\n";
        for(int i = 0; i < projects.count(); ++i)
        {
            if (projects[i].GUID_.isEmpty()) projects[i].GUID_ = Policy_::GUID();
            stream << "Project(\"" << sln_GUID << "\") = \""
                   << projects[i].name_ << "\", \""
                   << projects[i].name_ << Policy_::form() << "\", \""
                   << projects[i].GUID_ << "\"\n";
            foreach(QString con, projects[i].depends_)
            {
                foreach(const Project& dpj, projects)
                {
                    if (dpj.name_ == con)
                    {
                        stream << "\t" << "ProjectSection(ProjectDependencies) = postProject" << "\n"
                               << "\t" << "\t" << dpj.GUID_ << " = " << dpj.GUID_ << "\n"
                               << "\t" << "EndProjectSection" << "\n";
                    }
                }
            }
            stream << "EndProject" << "\n";
        }
        stream << "Global" << "\n"
               << "\t" << "GlobalSection(SolutionConfigurationPlatforms) = preSolution" << "\n"
               << "\t" << "\t" << "Debug|Win32 = Debug|Win32" << "\n"
               << "\t" << "\t" << "Debug|x64 = Debug|x64" << "\n"
               << "\t" << "\t" << "Release|Win32 = Release|Win32" << "\n"
               << "\t" << "\t" << "Release|x64 = Release|x64" << "\n"
               << "\t" << "EndGlobalSection" << "\n"
               << "\t" << "GlobalSection(ProjectConfigurationPlatforms) = postSolution" << "\n";
        foreach(const Project& prj, projects)
        {
            stream << "\t" << "\t" << prj.GUID_ << ".Debug|Win32.ActiveCfg = Debug|Win32" << "\n"
                   << "\t" << "\t" << prj.GUID_ << ".Debug|Win32.Build.0 = Debug|Win32" << "\n"
                   << "\t" << "\t" << prj.GUID_ << ".Debug|x64.ActiveCfg = Debug|x64" << "\n"
                   << "\t" << "\t" << prj.GUID_ << ".Debug|x64.Build.0 = Debug|x64" << "\n"
                   << "\t" << "\t" << prj.GUID_ << ".Release|Win32.ActiveCfg = Release|Win32" << "\n"
                   << "\t" << "\t" << prj.GUID_ << ".Release|Win32.Build.0 = Release|Win32" << "\n"
                   << "\t" << "\t" << prj.GUID_ << ".Release|x64.ActiveCfg = Release|x64" << "\n"
                   << "\t" << "\t" << prj.GUID_ << ".Release|x64.Build.0 = Release|x64" << "\n";
        }
        stream << "\t" << "EndGlobalSection" << "\n"
               << "\t" << "GlobalSection(SolutionProperties) = preSolution" << "\n"
               << "\t" << "\t" << "HideSolutionNode = FALSE" << "\n"
               << "\t" << "EndGlobalSection" << "\n"
               << "EndGlobal" << "\n";

        return true;
    }

    /*
        Project(".vcproj"/".vcxproj") Maker
    */

    bool makeProject(Config& cfg, const Project& prj)
    {
        QFile prj_file(prj.name_ + Policy_::form());
        if (!prj_file.open(QIODevice::WriteOnly |
                           QIODevice::Truncate)) return false;

        QDomDocument doc;
        doc.appendChild(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));
        doc.appendChild(Policy_::createRoot(cfg, prj, doc));

        QTextStream stream(&prj_file);
        doc.save(stream, 4);

        return true;
    }

public:
    bool make(Config& cfg)
    {
        if (!PlatformBase::make(cfg)) return false;

        if (!makeSolution(cfg)) return false;

        foreach(const Project& prj, cfg.projects())
        {
            if (!makeProject(cfg, prj)) return false;
        }

        return true;
    }
};

//////////////////////////////////////////////////////////////////////////

#endif // PLATFORM_VC_H
