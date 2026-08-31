#pragma once
#include <QString>
#include <QStringList>
#include <QPair>

class ExecParser
{
public:
    static QPair<QString, QStringList> buildCommand(const QString &execLine);
};
