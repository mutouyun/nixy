#ifndef CONFIG_H
#define CONFIG_H

#include "solution.h"

#include <QString>

//////////////////////////////////////////////////////////////////////////

class Config
{
public:
    Config(const QString&, const QString&, const QString&);

    const QString& plat(void) const;
    const QString& cc(void) const;

    bool isValid(void);

    static solution_t& solution(void);
    static projects_t& projects(void);

private:
    struct Impl_;
    Impl_* pimpl_;

protected:
    QString err_msg_;

public:
    void setMessage(const QString&);
    const QString& getMessage(void) const;

public:
    virtual ~Config();
};

//////////////////////////////////////////////////////////////////////////

#endif // CONFIG_H
