#include "config.h"

#include <QDebug>

#include "config_base.h"
#include "config_ini.h"
#include "config_js.h"

//////////////////////////////////////////////////////////////////////////

/*
    Test Config
*/

void test(const Config* p)
{
    qDebug() << "";
    qDebug() << p->solution().name_;
    qDebug() << "";
    foreach (QString var, p->solution().include_paths_)
    {
        qDebug() << var;
    }
    qDebug() << p->solution().tmp_path_;
    qDebug() << p->solution().out_path_;
    foreach (QString var, p->solution().DEFINES_)
    {
        qDebug() << var;
    }
    qDebug() << p->solution().CFLAGS_;
    qDebug() << p->solution().LFLAGS_;
    for(int i = 0; i < p->projects().count(); ++i)
    {
        qDebug() << "";
        qDebug() << p->projects()[i].name_;
        qDebug() << p->projects()[i].type_;
        foreach(QString file, p->projects()[i].depends_)
        {
            qDebug() << file;
        }
        foreach(QString file, p->projects()[i].heads_)
        {
            qDebug() << file;
        }
        foreach(QString file, p->projects()[i].sources_)
        {
            qDebug() << file;
        }
    }
}

//////////////////////////////////////////////////////////////////////////

struct Config::Impl_
{
    ConfigBase* cfg_;
    QString     plat_, cc_;
    bool        is_valid_;

    Impl_(const QString& script_path, const QString& plat, const QString& cc)
        : cfg_(NULL), plat_(plat), cc_(cc), is_valid_(false)
    {
        QString fmt(script_path);
        fmt = fmt.mid(fmt.lastIndexOf('.'));

        if (fmt == ".ini")
            cfg_ = new ConfigINI;
        else if (fmt == ".js")
            cfg_ = new ConfigJS(plat, cc);

        if (cfg_) is_valid_ = cfg_->load(script_path);
    }

    ~Impl_()
    {
        if (cfg_) delete cfg_;
    }
};

//////////////////////////////////////////////////////////////////////////

Config::Config(const QString& script_path, const QString& plat, const QString& cc)
    : pimpl_(new Impl_(script_path, plat, cc))
{
    if (!isValid())
    {
        setMessage("Configuration script parsing error.");
    }
}

Config::~Config()
{
    if (pimpl_) delete pimpl_;
}

//////////////////////////////////////////////////////////////////////////

const QString& Config::plat(void) const
{
    Q_ASSERT(pimpl_);
    return pimpl_->plat_;
}

const QString& Config::cc(void) const
{
    Q_ASSERT(pimpl_);
    return pimpl_->cc_;
}

bool Config::isValid(void)
{
    Q_ASSERT(pimpl_);
    return pimpl_->is_valid_;
}

solution_t& Config::solution(void)
{
    static solution_t sln;
    return sln;
}

projects_t& Config::projects(void)
{
    static projects_t pjs;
    return pjs;
}

void Config::setMessage(const QString& msg)
{
    err_msg_ = msg;
}

const QString& Config::getMessage(void) const
{
    return err_msg_;
}
