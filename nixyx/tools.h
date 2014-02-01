#ifndef TOOLS_H
#define TOOLS_H

#include <QString>

//////////////////////////////////////////////////////////////////////////

#define DEF_SOL(...)        #__VA_ARGS__
#define DEF_STR(...)        DEF_SOL(__VA_ARGS__)

#define DEF_CAT(x, ...)     x##__VA_ARGS__
#define DEF_JOIN(x, ...)    DEF_CAT(x, __VA_ARGS__)

#define DEF_ARGS(...)      __VA_ARGS__

//////////////////////////////////////////////////////////////////////////

/*
    Intercept string from contents
*/

inline QString intercept(QString& cons, char c = '\'')
{
    QString ret;
    int n = cons.indexOf(c);
    int m = cons.indexOf(c, n + 1);
    if (n < 0)
    {
        ret = cons;
        cons.clear();
    }
    else if (m < 0)
    {
        ret = cons.right(n);
        cons.clear();
    }
    else
    {
        ret  = cons.mid(n + 1, m - n - 1);
        cons = cons.right(cons.length() - m - 1);
    }
    return ret.trimmed();
}

inline QString intercept(const QString& cons, char c = '\'')
{
    return intercept(const_cast<QString&>(cons), c);
}

inline void interceptList(QString cons, QList<QString>& rets, char c = '\'')
{
    while(!cons.isEmpty())
    {
        rets.append(intercept(cons, c));
    }
}

template <class Format_>
inline void interceptList(QString cons, QList<QString>& rets, Format_& format, char c = '\'')
{
    while(!cons.isEmpty())
    {
        rets.append(format(intercept(cons, c)));
    }
}

struct PathAppendFile_
{
    QString path_;
    QString operator()(const QString& file)
    {
        return path_ + file;
    }
};

//////////////////////////////////////////////////////////////////////////

#endif // TOOLS_H
