#ifndef CONFIG_INI_H
#define CONFIG_INI_H

#include "config_base.h"

#include <QSettings>
#include <QScopedPointer>

//////////////////////////////////////////////////////////////////////////

class ConfigINI: public ConfigBase
{
public:
    bool load(const QString& script_path)
    {
        QScopedPointer<QSettings> resolver(new QSettings(script_path, QSettings::IniFormat));
#define ini_value(key) resolver->value(key).toString()

        Config::solution().name_ = ini_value("/solution/name");

        QList<QString> projects;
        interceptList(ini_value("/solution/projects"), projects);
        for(int i = 0; i < projects.count(); ++i)
        {
            Project prj; prj.name_ = projects[i];
            Config::projects().append(prj);
        }

        PathAppendFile_ path_file;
        path_file.path_ = Config::solution().path_ = intercept(ini_value("/solution/project_path"));

        Config::solution().include_paths_.append(path_file.path_);
        interceptList(ini_value("/solution/include_path"),
                       Config::solution().include_paths_, path_file);

        Config::solution().tmp_path_ = intercept(ini_value("/solution/tmp_path"));
        Config::solution().out_path_ = intercept(ini_value("/solution/out_path"));

        interceptList               (ini_value("/solution/DEFINES"), Config::solution().DEFINES_);
        Config::solution().CFLAGS_ = ini_value("/solution/CFLAGS");
        Config::solution().LFLAGS_ = ini_value("/solution/LFLAGS");

        for(int i = 0; i < projects.count(); ++i)
        {
            QString prj_tag("/project."); prj_tag += projects[i];

            Config::projects()[i].GUID_ = ini_value(prj_tag + "/GUID");
            Config::projects()[i].type_ = ini_value(prj_tag + "/type");
            Config::projects()[i].libs_ = ini_value(prj_tag + "/libs");

            interceptList(ini_value(prj_tag + "/depends"),
                           Config::projects()[i].depends_);

            interceptList(ini_value(prj_tag + "/heads"),
                           Config::projects()[i].heads_, path_file);

            interceptList(ini_value(prj_tag + "/sources"),
                           Config::projects()[i].sources_, path_file);
        }

#undef ini_value
        return true;
    }
};

//////////////////////////////////////////////////////////////////////////

#endif // CONFIG_INI_H
