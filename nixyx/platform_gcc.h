#ifndef PLATFORM_GCC_H
#define PLATFORM_GCC_H

#include "platform_base.h"

#include <QFile>
#include <QTextStream>

//////////////////////////////////////////////////////////////////////////

class PlatformGCC : public PlatformBase
{
public:
    bool make(Config& cfg)
    {
        if (!PlatformBase::make(cfg)) return false;

        QFile make_file("Makefile");
        if (!make_file.open(QIODevice::WriteOnly |
                            QIODevice::Truncate)) return false;

        solution_t solution = cfg.solution();
        projects_t projects = cfg.projects();
        QString tmp_path(solution.tmp_path_);
        QString out_path(solution.out_path_);

        QString exe, dll(".so");
        QString rd("-rmdir "), md("-mkdir -p "), rm("-rm ");
        char slash[2] = { '\\', '/' };
        if (cfg.plat() == "win")
        {
            qSwap(slash[0], slash[1]);
            exe = ".exe";
            dll = ".dll";
            rd = "-rmdir /s /q ";
            md = "-mkdir ";
            rm = "-del /s /q /f ";
        }
        tmp_path.replace(slash[0], slash[1]);
        out_path.replace(slash[0], slash[1]);

        QTextStream stream(&make_file);
        stream << "# Compiler, tools and options" << "\n"
               << "\n"
               << "CC = gcc" << "\n"
               << "CX = g++" << "\n"
               << "LB = ar -csr" << "\n"
               << "DEFINES =";
        foreach(QString con, solution.DEFINES_)
        {
            stream << " -D" << con;
        }
        stream << "\n"
               << "CFLAGS = " << solution.CFLAGS_ << " -c\n"
               << "LFLAGS = " << solution.LFLAGS_ << "\n"
               << "INCPATH =";
        foreach(QString con, solution.include_paths_)
        {
            stream << " -I\"" << con << "\"";
        }
        stream << "\n\n"
               << "DEBUG = 0" << "\n"
               << "BITS = 32" << "\n"
               << "\n"
               << "ifeq ($(DEBUG), 0)" << "\n"
               << "\t" << "# release" << "\n"
               << "\t" << "CFLAGS += -O2 -DNDEBUG" << "\n"
               << "\t" << "Configuration = release" << "\n"
               << "else" << "\n"
               << "\t" << "# debug" << "\n"
               << "\t" << "CFLAGS += -g" << "\n"
               << "\t" << "Configuration = debug" << "\n"
               << "endif" << "\n"
               << "\n"
               << "ifeq ($(BITS), 32)" << "\n"
               << "\t" << "# 32-bit" << "\n"
               << "\t" << "CFLAGS += -m32" << "\n"
               << "\t" << "LFLAGS += -m32" << "\n"
               << "else" << "\n"
               << "\t" << "# 64-bit" << "\n"
               << "\t" << "CFLAGS += -m64" << "\n"
               << "\t" << "LFLAGS += -m64" << "\n"
               << "endif" << "\n"
               << "\n"
               << "# Output directory" << "\n"
               << "\n"
               << "OUT = " << out_path << "\n"
               << "TMP = " << tmp_path << "\n"
               << "\n"
               << "# Files" << "\n"
               << "\n";
        foreach(const Project& prj, projects)
        {
            if (prj.sources_.empty()) continue;
            stream << "SRC_" << prj.name_ << " =";
            foreach(QString con, prj.sources_)
            {
                stream << " \\\n\t\"" << con << "\"";
            }
            stream << "\n";
        }
        stream << "\n"
               << "# Build rules" << "\n"
               << "\n"
               << ".PHONY: all clean out";
        foreach(const Project& prj, projects)
        {
            if (prj.sources_.empty()) continue;
            stream << " tmp_" << prj.name_;
        }
        stream << "\n\n"
               << "all:";
        foreach(const Project& prj, projects)
        {
            if (prj.sources_.empty()) continue;
            stream << " " << prj.name_;
        }
        stream << "\n\n"
               << "clean:" << "\n";
        foreach(const Project& prj, projects)
        {
            if (prj.sources_.empty()) continue;
            stream << "\t" << rm << "$(TMP)" << slash[1] << prj.name_ << slash[1] << "*" << "\n"
                   << "\t" << rd << "$(TMP)" << slash[1] << prj.name_ << "\n";
        }
        stream << "\t" << rm << "$(OUT)" << slash[1] << "*" << "\n"
               << "\t" << rd << "$(OUT)" << "\n"
               << "\n"
               << "out:" << "\n"
               << "\t" << md << "$(OUT)" << "\n";
        foreach(const Project& prj, projects)
        {
            if (prj.sources_.empty()) continue;
            stream << "tmp_" << prj.name_ << ":" << "\n"
                   << "\t" << md << "$(TMP)" << slash[1] << prj.name_ << "\n";
        }
        stream << "\n"
               << "# Compile" << "\n"
               << "\n";
        QList<QString> obj_names;
        foreach(const Project& prj, projects)
        {
            if (prj.sources_.empty()) continue;
            QString names;
            foreach(QString con, prj.sources_)
            {
                QString obj_name(con);
                obj_name = obj_name.mid(obj_name.lastIndexOf('/') + 1);
                obj_name = obj_name.left(obj_name.lastIndexOf('.'));
                obj_name = QString("$(TMP)%1%2%3%4.o").arg(slash[1]).arg(prj.name_).arg(slash[1]).arg(obj_name);
                stream << obj_name << ": " << con << " | tmp_" << prj.name_ << "\n"
                       << "\t" << "$(CX) -o " << obj_name << " $(CFLAGS) $(INCPATH) " << con << "\n";
                names += " ";
                names += obj_name;
            }
            stream << "\n";
            obj_names.append(names);
        }
        stream << "\n";
        int i = 0;
        foreach(const Project& prj, projects)
        {
            if (prj.sources_.empty()) continue;
            stream << prj.name_ << ":" << obj_names[i] << " | out" << "\n";
            if (prj.type_ == "lib")
            {
                stream << "\t" << "$(LB) $(OUT)/lib" << prj.name_ << ".a" << obj_names[i];
            }
            else
            {
                if (prj.type_ == "console" && cfg.plat() == "win")
                {
                    stream << "\t" << "$(CX) -o $(OUT)/" << prj.name_ << exe
                           << " -Wl,-subsystem,console $(LFLAGS)" << obj_names[i];
                }
                else
                {
                    stream << "\t" << "$(CX) -o $(OUT)/" << prj.name_ << exe
                           << " $(LFLAGS)" << obj_names[i];
                }
                foreach(QString con, prj.depends_)
                {
                    stream << " $(OUT)/lib" << con << ".a";
                }
                stream << " " << prj.libs_;
            }
            stream << "\n";
            ++i;
        }

        return true;
    }
};

//////////////////////////////////////////////////////////////////////////

#endif // PLATFORM_GCC_H
