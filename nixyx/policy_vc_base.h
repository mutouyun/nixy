#ifndef POLICY_VC_BASE_H
#define POLICY_VC_BASE_H

#include "config.h"
#include "solution.h"

#include <QFile>
#include <QTextStream>
#include <QDomDocument>
#include <QUuid>

//////////////////////////////////////////////////////////////////////////

struct PolicyVCBase
{
    /*
        The options of createRoot
    */

    virtual QDomElement makeRoot          (Config&, const Project&, QDomDocument&)               { return QDomElement(); }
    virtual QDomElement makePlatforms     (Config&, const Project&, QDomDocument&, QDomElement&) { return QDomElement(); }
    virtual QDomElement makeConfigurations(Config&, const Project&, QDomDocument&, QDomElement&) { return QDomElement(); }
    virtual QDomElement makeFilters       (Config&, const Project&, QDomDocument&, QDomElement&) { return QDomElement(); }

    /*
        Make the root element of project file
    */

    QDomElement createRoot(Config& cfg, const Project& prj, QDomDocument& doc)
    {
        QDomElement root = makeRoot(cfg, prj, doc);

        root.appendChild(makePlatforms     (cfg, prj, doc, root));
        root.appendChild(makeConfigurations(cfg, prj, doc, root));
        root.appendChild(makeFilters       (cfg, prj, doc, root));

        return root;
    }

    /*
        Create GUID
    */

    QString GUID(void)
    {
        return QUuid().createUuid().toString();
    }

    /*
        Get filter of the file
    */

    QString createFilterName(solution_t& solution, QString& file_path)
    {
        file_path.replace('/', '\\');
        QString file_orig = file_path.mid(solution.path_.length());

        if (file_orig.left(1) == "\\")
            file_orig = file_orig.mid(1);
        else if (file_orig.left(2) == ".\\")
            file_orig = file_orig.mid(2);

        int n = file_orig.indexOf('\\') + 1;
        int m = file_orig.indexOf('\\', n);
        return (m > 0) ? file_orig.mid(n, m - n) : QString();
    }

    /*
        AdditionalIncludeDirectories && PreprocessorDefinitions
    */

    QString createIncludePaths(solution_t& solution, const Project& /*prj*/)
    {
        // AdditionalIncludeDirectories
        QString path("$(SolutionDir)");
        foreach(QString con, solution.include_paths_)
        {
            path += ";$(SolutionDir)";
            path += con;
        }
        return path;
    }

    QString createDefines(solution_t& solution, const Project& prj)
    {
        // PreprocessorDefinitions
        QString cons;
        if (prj.type_ == "lib")
            cons = ";_LIB";
        else if (prj.type_ == "dll")
            cons = ";_USRDLL";
        else if (prj.type_ == "console")
            cons = ";_CONSOLE";
        foreach(QString con, solution.DEFINES_)
        {
            cons += ";";
            cons += con;
        }
        return cons;
    }
};

//////////////////////////////////////////////////////////////////////////

#endif // POLICY_VC_BASE_H
