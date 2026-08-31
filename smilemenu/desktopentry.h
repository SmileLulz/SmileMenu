#pragma once
#include <QString>
#include <QStringList>
#include <QPointer>
#include "appitem.h"

class DesktopEntry
{
public:
    static AppItem* fromFile(const QString &path);
};
