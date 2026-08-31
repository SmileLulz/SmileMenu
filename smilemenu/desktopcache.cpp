#include "desktopcache.h"
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QSaveFile>
#include <QDebug>

QString DesktopCache::cacheFilePath()
{
    return QDir::homePath() + "/.cache/smilemenu/apps_cache.json";
}

static QJsonObject collectFileMetadata(const QStringList &directories)
{
    QJsonObject files;
    for (const QString &dirPath : directories) {
        QDir dir(dirPath);
        if (!dir.exists())
            continue;
        const QFileInfoList entries = dir.entryInfoList({"*.desktop"}, QDir::Files | QDir::Readable, QDir::Name);
        for (const QFileInfo &info : entries) {
            QJsonObject meta;
            meta["mtime"] = info.lastModified().toMSecsSinceEpoch();
            meta["size"] = static_cast<qint64>(info.size());
            files[info.absoluteFilePath()] = meta;
        }
    }
    return files;
}

QList<AppItem*> DesktopCache::load(const QStringList &directories, bool &valid)
{
    valid = false;
    QFile file(cacheFilePath());
    if (!file.open(QIODevice::ReadOnly))
        return {};

    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject())
        return {};

    const QJsonObject obj = doc.object();
    if (obj.value("version").toInt() != 2)
        return {};

    const QJsonObject expectedFiles = collectFileMetadata(directories);
    if (obj.value("files").toObject() != expectedFiles)
        return {};

    const QJsonArray itemsArr = obj.value("items").toArray();
    QList<AppItem*> items;
    items.reserve(itemsArr.size());
    for (const QJsonValue &v : itemsArr) {
        if (!v.isObject())
            continue;
        const QJsonObject itemObj = v.toObject();
        const QString name = itemObj.value("name").toString();
        const QString command = itemObj.value("command").toString();
        if (name.isEmpty() || command.isEmpty())
            continue;
        items.append(new AppItem(
            name,
            command,
            itemObj.value("icon").toString(),
            itemObj.value("description").toString(),
            itemObj.value("categories").toVariant().toStringList()));
    }

    valid = true;
    return items;
}

void DesktopCache::save(const QStringList &directories, const QList<AppItem*> &items)
{
    QJsonObject obj;
    obj["version"] = 2;
    obj["files"] = collectFileMetadata(directories);

    QJsonArray itemsArr;
    for (AppItem *item : items) {
        if (!item)
            continue;
        QJsonObject itemObj;
        itemObj["name"] = item->name();
        itemObj["command"] = item->command();
        itemObj["icon"] = item->icon();
        itemObj["description"] = item->description();
        itemObj["categories"] = QJsonArray::fromStringList(item->categories());
        itemsArr.append(itemObj);
    }
    obj["items"] = itemsArr;

    const QString path = cacheFilePath();
    QDir().mkpath(QFileInfo(path).path());
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly))
        return;
    const QByteArray data = QJsonDocument(obj).toJson(QJsonDocument::Indented);
    if (file.write(data) != data.size() || !file.commit())
        qWarning() << "Failed to save application cache";
}
