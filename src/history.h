#pragma once
#include <QMap>
#include <QString>

class History
{
public:
    static QMap<QString, int> load();
    static void save(const QMap<QString, int> &history);
    static QString filePath();
};
