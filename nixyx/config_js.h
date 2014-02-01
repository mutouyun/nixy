#ifndef CONFIG_JS_H
#define CONFIG_JS_H

#include "config.h"
#include "config_base.h"

#include <QObject>
#include <QtScript>
#include <QFile>
#include <QTextStream>

//////////////////////////////////////////////////////////////////////////

#define DEF_PROPERTY(name, prop, ...) \
    QString get##name(void)            { return (prop); } \
    void set##name(const QString& val) { (prop) = val; __VA_ARGS__ }

#define DEF_PROPLIST(name, prop, ...) \
    QString get##name(void) \
    { \
        QString cons; \
        foreach (QString con, prop) cons += "\'" + con + "\' "; \
        return cons; \
    } \
    void set##name(const QString& val) \
    { \
        prop.clear(); \
        interceptList(val, prop,##__VA_ARGS__); \
    }

//////////////////////////////////////////////////////////////////////////

class JSSolution : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name          READ getName           WRITE setName)
    Q_PROPERTY(QString project_path  READ getProjectPath    WRITE setProjectPath)
    Q_PROPERTY(QString include_path  READ getIncludePath    WRITE setIncludePath)
    Q_PROPERTY(QString tmp_path      READ getTmpPath        WRITE setTmpPath)
    Q_PROPERTY(QString out_path      READ getOutPath        WRITE setOutPath)
    Q_PROPERTY(QString DEFINES       READ getDEFINES        WRITE setDEFINES)
    Q_PROPERTY(QString CFLAGS        READ getCFLAGS         WRITE setCFLAGS)
    Q_PROPERTY(QString LFLAGS        READ getLFLAGS         WRITE setLFLAGS)

    DEF_PROPERTY(Name,        Config::solution().name_)
    DEF_PROPERTY(ProjectPath, Config::solution().path_,
        path_file_.path_ = val;
    )
    DEF_PROPERTY(TmpPath,     Config::solution().tmp_path_)
    DEF_PROPERTY(OutPath,     Config::solution().out_path_)
    DEF_PROPERTY(CFLAGS,      Config::solution().CFLAGS_)
    DEF_PROPERTY(LFLAGS,      Config::solution().LFLAGS_)

    QString getIncludePath(void)
    {
        QString cons;
        foreach (QString con, Config::solution().include_paths_)
            cons += "\'" + con + "\' ";
        return cons;
    }
    void setIncludePath(const QString& val)
    {
        Config::solution().include_paths_.clear();
        Config::solution().include_paths_.append(path_file_.path_);
        interceptList(val, Config::solution().include_paths_, path_file_);
    }

    DEF_PROPLIST(DEFINES, Config::solution().DEFINES_)

public:
    JSSolution(PathAppendFile_& path_file)
        : path_file_(path_file)
    {}

private:
    PathAppendFile_& path_file_;
};

//////////////////////////////////////////////////////////////////////////

class JSProject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name     READ getName     WRITE setName)
    Q_PROPERTY(QString GUID     READ getGUID     WRITE setGUID)
    Q_PROPERTY(QString type     READ getType     WRITE setType)
    Q_PROPERTY(QString libs     READ getLibs     WRITE setLibs)
    Q_PROPERTY(QString depends  READ getDepends  WRITE setDepends)
    Q_PROPERTY(QString heads    READ getHeads    WRITE setHeads)
    Q_PROPERTY(QString sources  READ getSources  WRITE setSources)

    DEF_PROPERTY(Name,    prj_.name_)
    DEF_PROPERTY(GUID,    prj_.GUID_)
    DEF_PROPERTY(Type,    prj_.type_)
    DEF_PROPERTY(Libs,    prj_.libs_)

    DEF_PROPLIST(Depends, prj_.depends_)
    DEF_PROPLIST(Heads,   prj_.heads_  , path_file_)
    DEF_PROPLIST(Sources, prj_.sources_, path_file_)

public:
    explicit JSProject(QObject* parent, Project& prj, PathAppendFile_& path_file)
        : QObject(parent), prj_(prj), path_file_(path_file)
    {}

private:
    Project& prj_;
    PathAppendFile_& path_file_;
};

class JSProjects : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE QScriptValue create(const QString& name)
    {
        Project prj; prj.name_ = name;
        Config::projects().append(prj);
        return resolver_.newQObject(new JSProject(this, Config::projects().back(), path_file_));
    }

public:
    explicit JSProjects(PathAppendFile_& path_file, QScriptEngine& resolver)
        : path_file_(path_file), resolver_(resolver)
    {}

private:
    PathAppendFile_& path_file_;
    QScriptEngine&   resolver_;
};

//////////////////////////////////////////////////////////////////////////

class JSDefines : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString PLAT READ getPlat)
    Q_PROPERTY(QString CC   READ getCC)

public:
    QString getPlat(void) { return plat_; }
    QString getCC(void)   { return cc_; }

public:
    QString plat_, cc_;
};

//////////////////////////////////////////////////////////////////////////

class ConfigJS: public ConfigBase
{
protected:
    QScriptEngine   resolver_;
    JSDefines       defines_;

public:
    ConfigJS(const QString& plat, const QString& cc)
    {
        defines_.plat_ = plat;
        defines_.cc_   = cc;
        resolver_.globalObject().setProperty("Make", resolver_.newQObject(&defines_));
    }

    bool load(const QString& script_path)
    {
        QFile script_file(script_path);
        if (!script_file.open(QIODevice::ReadOnly)) return false;
        QTextStream stream(&script_file);
        QString contents = stream.readAll();
        script_file.close();

        PathAppendFile_ path_file;
        JSSolution js_solution(path_file);
        JSProjects js_projects(path_file, resolver_);

        resolver_.globalObject().setProperty("Solution", resolver_.newQObject(&js_solution));
        resolver_.globalObject().setProperty("Projects", resolver_.newQObject(&js_projects));

        QScriptValue result = resolver_.evaluate(contents, script_path);
        if (result.isError())
        {
            QTextStream(stdout)
                << "\nScript Error:" << "\n"
                << script_path << ":"
                << result.property("lineNumber").toInt32() <<  " ->: "
                << result.toString() << "\n";
            return false;
        }
        return true;
    }
};

//////////////////////////////////////////////////////////////////////////

#endif // CONFIG_JS_H
