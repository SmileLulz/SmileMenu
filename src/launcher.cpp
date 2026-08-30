#include "launcher.h"
#include "desktopentry.h"
#include "desktopcache.h"
#include <QDir>
#include <QDebug>
#include <QStandardPaths>

QList<AppItem*> loadApplications()
{
    QStringList directories = QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation);
    const QStringList extraDirectories = {
        QDir::homePath() + "/.local/share/flatpak/exports/share/applications",
        "/var/lib/flatpak/exports/share/applications",
        "/var/lib/snapd/desktop/applications"
    };
    for (const QString &dir : extraDirectories) {
        if (!directories.contains(dir))
            directories.append(dir);
    }

    directories.removeDuplicates();

    bool valid = false;
    const QList<AppItem*> cached = DesktopCache::load(directories, valid);
    if (valid && !cached.isEmpty())
        return cached;

    QMap<QString, AppItem*> appMap;
    for (const QString &dirPath : directories) {
        const QDir dir(dirPath);
        if (!dir.exists())
            continue;

        const QFileInfoList entries = dir.entryInfoList({"*.desktop"}, QDir::Files | QDir::Readable, QDir::Name);
        for (const QFileInfo &info : entries) {
            const QString fileName = info.fileName();
            if (appMap.contains(fileName))
                continue;

            if (AppItem *item = DesktopEntry::fromFile(info.absoluteFilePath()))
                appMap.insert(fileName, item);
        }
    }

    const QList<AppItem*> result = appMap.values();
    DesktopCache::save(directories, result);
    return result;
}
