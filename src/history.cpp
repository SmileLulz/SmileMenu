#include "history.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>

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
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
    if (err.error != QJsonParseError::NoError)
        return {};

    QMap<QString, int> history;
    QJsonObject obj = doc.object();
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        history[it.key()] = it.value().toInt();
    }
    return history;
}

void History::save(const QMap<QString, int> &history)
{
    QFile file(filePath());
    if (!file.open(QIODevice::WriteOnly)) {
        QDir().mkpath(QFileInfo(filePath()).path());
        if (!file.open(QIODevice::WriteOnly))
            return;
    }
    QJsonObject obj;
    for (auto it = history.begin(); it != history.end(); ++it)
        obj[it.key()] = it.value();
    file.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
}
