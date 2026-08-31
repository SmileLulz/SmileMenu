#pragma once
#include <QList>
#include <QString>
#include <QStringList>
#include "appitem.h"

class DesktopCache
{
public:
    static QList<AppItem*> load(const QStringList &directories, bool &valid);
    static void save(const QStringList &directories, const QList<AppItem*> &items);
    static QString cacheFilePath();
};
