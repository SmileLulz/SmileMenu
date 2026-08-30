#include "history.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QSaveFile>

QString History::filePath()
{
    return QDir::homePath() + "/.local/state/smilemenu/history.json";
}

QMap<QString, int> History::load()
{
    QFile file(filePath());
    if (!file.open(QIODevice::ReadOnly))
        return {};

    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject())
        return {};

    QMap<QString, int> history;
    const QJsonObject obj = doc.object();
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        const int count = it.value().toInt();
        if (count > 0)
            history[it.key()] = count;
    }
    return history;
}

void History::save(const QMap<QString, int> &history)
{
    const QString path = filePath();
    QDir().mkpath(QFileInfo(path).path());

    QJsonObject obj;
    for (auto it = history.begin(); it != history.end(); ++it) {
        if (it.value() > 0)
            obj[it.key()] = it.value();
    }

    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly))
        return;

    const QByteArray data = QJsonDocument(obj).toJson(QJsonDocument::Indented);
    if (file.write(data) != data.size() || !file.commit())
        qWarning("Failed to atomically save SmileMenu history");
}
