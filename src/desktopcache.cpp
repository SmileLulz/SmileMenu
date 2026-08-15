#include "desktopcache.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QDebug>

QString DesktopCache::cacheFilePath()
{
    return QDir::homePath() + "/.cache/smilemenu/apps_cache.json";
}

QList<AppItem*> DesktopCache::load(const QStringList &directories, bool &valid)
{
    valid = false;
    QFile file(cacheFilePath());
    if (!file.open(QIODevice::ReadOnly))
        return {};

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
    if (err.error != QJsonParseError::NoError)
        return {};

    QJsonObject obj = doc.object();
    if (obj.value("version").toInt() != 1)
        return {};

    QJsonObject dirsObj = obj.value("directories").toObject();
    for (const QString &dir : directories) {
        QFileInfo info(dir);
        if (!info.exists()) {
            // directory doesn't exist; skipping, but mtime check will fail
        }
        qint64 mtime = info.lastModified().toSecsSinceEpoch();
        if (dirsObj.value(dir).toVariant().toLongLong() != mtime) {
            return {};
        }
    }

    QJsonArray itemsArr = obj.value("items").toArray();
    QList<AppItem*> items;
    for (const QJsonValue &v : itemsArr) {
        QJsonObject itemObj = v.toObject();
        AppItem *item = new AppItem(
            itemObj.value("name").toString(),
            itemObj.value("command").toString(),
            itemObj.value("icon").toString(),
            itemObj.value("description").toString(),
            itemObj.value("categories").toVariant().toStringList()
        );
        items.append(item);
    }
    valid = true;
    return items;
}

void DesktopCache::save(const QStringList &directories, const QList<AppItem*> &items)
{
    QJsonObject obj;
    obj["version"] = 1;

    QJsonObject dirsObj;
    for (const QString &dir : directories) {
        QFileInfo info(dir);
        if (info.exists())
            dirsObj[dir] = info.lastModified().toSecsSinceEpoch();
    }
    obj["directories"] = dirsObj;

    QJsonArray itemsArr;
    for (AppItem *item : items) {
        QJsonObject itemObj;
        itemObj["name"] = item->name();
        itemObj["command"] = item->command();
        itemObj["icon"] = item->icon();
        itemObj["description"] = item->description();
        itemObj["categories"] = QJsonArray::fromStringList(item->categories());
        itemsArr.append(itemObj);
    }
    obj["items"] = itemsArr;

    QFile file(cacheFilePath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
    }
}
