#ifndef SOLUTION_H
#define SOLUTION_H

#include <QString>
#include <QList>

//////////////////////////////////////////////////////////////////////////

struct Solution
{
    QString        name_, path_;
    QList<QString> include_paths_;
    QString        tmp_path_, out_path_;
    QList<QString> DEFINES_;
    QString        CFLAGS_, LFLAGS_;
};

struct Project
{
    QString name_, GUID_;
    QString type_, libs_;
    QList<QString> depends_;
    QList<QString> heads_;
    QList<QString> sources_;
};

typedef Solution        solution_t;
typedef QList<Project>  projects_t;

//////////////////////////////////////////////////////////////////////////

#endif // SOLUTION_H
