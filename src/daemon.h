#pragma once
#include <QObject>
#include <QPointer>
#include <QLocalServer>
#include <QQmlApplicationEngine>
#include "model.h"
#include "lock.h"

class Daemon : public QObject
{
    Q_OBJECT
public:
    explicit Daemon(const QVariantMap &config, const QString &themePath, QObject *parent = nullptr);
    ~Daemon();

    int exec();

private slots:
    void onNewConnection();
    void onFrameSwapped();
    void cleanup();

private:
    void setupSocketServer();
    void loadQml(const QString &path);
    void processRequest(const QByteArray &data);
    void showWindow();
    void hideWindow();

    QVariantMap m_config;
    QString m_themePath;
    SingleInstanceLock m_lock;

    QPointer<QLocalServer> m_server;
    QPointer<QQmlApplicationEngine> m_engine;
    QPointer<LauncherModel> m_model;
    QObject *m_rootObject = nullptr;
    QString m_currentQmlPath;
    QMap<QString, qint64> m_themeCache;

    bool m_warmupDone = false;
    int m_themeCacheLimit;
};
