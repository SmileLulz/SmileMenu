#include "launcher.h"
// #include "desktopentry.h"
// #include "desktopcache.h"
#include <QDir>
#include <QDebug>

QList<AppItem*> loadApplications()
{
    QStringList directories = {
        QDir::homePath() + "/.local/share/applications",
        "/usr/local/share/applications",
        "/usr/share/applications",
        "/var/lib/flatpak/exports/share/applications"
    };

    bool valid;
    QList<AppItem*> apps = DesktopCache::load(directories, valid);
    if (valid && !apps.isEmpty())
        return apps;

    QMap<QString, AppItem*> appMap;
    for (const QString &dirPath : directories) {
        QDir dir(dirPath);
        if (!dir.exists())
            continue;
        QStringList filters;
        filters << "*.desktop";
        QFileInfoList entries = dir.entryInfoList(filters, QDir::Files);
        for (const QFileInfo &info : entries) {
            QString name = info.fileName();
            if (appMap.contains(name))
                continue;
            AppItem *item = DesktopEntry::fromFile(info.absoluteFilePath());
            if (item) {
                appMap[name] = item;
            }
        }
    }

    QList<AppItem*> result = appMap.values();
    DesktopCache::save(directories, result);
    return result;
}
