#pragma once
#include <QVariantMap>
#include <QString>

class Config
{
public:
    static QString defaultPath();
    static QString runtimeSocketPath();
    static QString bundledThemePath();
    static QVariantMap load(const QString &path = QString());
    static void save(const QVariantMap &config, const QString &path = QString());
    static void saveDefault(const QString &path = QString());
    static QVariantMap defaultConfig();
};
