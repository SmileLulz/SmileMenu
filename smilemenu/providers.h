#pragma once
#include <QString>
#include <QStringList>
#include "appitem.h"

class Providers
{
public:
    static AppItem* fromLine(const QString &line, const QStringList &fields);
    static QString getDisplayName(const QString &value, const QString &displayColumns);
};
