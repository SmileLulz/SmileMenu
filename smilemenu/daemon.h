#pragma once
#include <QObject>
#include <QPointer>
#include <QHash>
#include <QLocalServer>
#include <QLocalSocket>
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
    bool isReady() const;

private slots:
    void onNewConnection();
    void onFrameSwapped();
    void cleanup();

private:
    void setupSocketServer();
    void processRequest(const QByteArray &data);
    void loadBundledOrLocalQml(const QString &path);
    void resetWindowForShow();
    void showWindow();

    QVariantMap m_config;
    QString m_themePath;
    SingleInstanceLock m_lock;

    QPointer<QLocalServer> m_server;
    QPointer<QQmlApplicationEngine> m_engine;
    QPointer<LauncherModel> m_model;
    QObject *m_rootObject = nullptr;
    QString m_currentQmlPath;
    QMap<QString, qint64> m_themeCache;
    QHash<QLocalSocket *, QByteArray> m_socketBuffers;

    bool m_warmupDone = false;
};
