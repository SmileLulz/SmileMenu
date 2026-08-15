#include "config.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QDebug>

QString Config::defaultPath()
{
    return QDir::homePath() + "/.config/smilemenu/config.json";
}

QVariantMap Config::defaultConfig()
{
    QVariantMap cfg;
    cfg["history_limit"] = 3;
    cfg["prompt_position"] = "entry";
    cfg["window_width"] = 500;
    cfg["min_visible_items"] = 1;
    cfg["max_visible_items"] = 6;
    cfg["fuzzy_search"] = true;
    cfg["show_text_field"] = true;
    cfg["max_icon_cache_size"] = 256;
    cfg["max_theme_cache_size"] = 5;
    return cfg;
}

QVariantMap Config::load(const QString &path)
{
    QString p = path.isEmpty() ? defaultPath() : path;
    if (!QFile::exists(p))
        return defaultConfig();

    QFile file(p);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open config file" << p;
        return defaultConfig();
    }

    QByteArray data = file.readAll();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse config JSON:" << err.errorString();
        return defaultConfig();
    }

    QVariantMap config = defaultConfig();
    QVariantMap loaded = doc.object().toVariantMap();
    for (auto it = loaded.begin(); it != loaded.end(); ++it) {
        config[it.key()] = it.value();
    }
    return config;
}

void Config::save(const QVariantMap &config, const QString &path)
{
    QString p = path.isEmpty() ? defaultPath() : path;
    QDir().mkpath(QFileInfo(p).path());
    QFile file(p);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to write config to" << p;
        return;
    }
    QJsonObject obj = QJsonObject::fromVariantMap(config);
    file.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
}

void Config::saveDefault(const QString &path)
{
    save(defaultConfig(), path);
}
