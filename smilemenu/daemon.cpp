#include "daemon.h"
#include "model.h"
#include "iconprovider.h"
#include "config.h"
#include <QQmlContext>
#include <QQuickWindow>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileInfo>
#include <QTimer>
#include <QDebug>
#include <QDir>
#include <QUrl>
#include <QCoreApplication>
#include <unistd.h>

namespace {
constexpr qsizetype kMaxRequestSize = 1024 * 1024;
constexpr qint64 kMissingMtime = -1;
}

Daemon::Daemon(const QVariantMap &config, const QString &themePath, QObject *parent)
    : QObject(parent),
      m_config(config),
      m_themePath(themePath.isEmpty() ? Config::bundledThemePath() : themePath)
{
    m_engine = new QQmlApplicationEngine(this);

    const int iconCacheSize = qMax(1, config.value("max_icon_cache_size", 256).toInt());
    m_engine->addImageProvider("icons", new IconProvider(iconCacheSize));

    m_model = new LauncherModel(config, this);
    m_engine->rootContext()->setContextProperty("launcher", m_model);
    m_engine->rootContext()->setContextProperty("preload_mode", true);

    loadBundledOrLocalQml(m_themePath);

    if (!m_rootObject) {
        qCritical() << "SmileMenu cannot start because the QML root failed to load:" << m_themePath;
        return;
    }

    if (QQuickWindow *window = qobject_cast<QQuickWindow *>(m_rootObject)) {
        connect(window, &QQuickWindow::frameSwapped, this, &Daemon::onFrameSwapped);
        window->show();
    }

    setupSocketServer();
}

Daemon::~Daemon()
{
    cleanup();
}

void Daemon::onFrameSwapped()
{
    if (m_warmupDone)
        return;

    m_warmupDone = true;
    if (m_rootObject) {
        if (QQuickWindow *window = qobject_cast<QQuickWindow *>(m_rootObject)) {
            disconnect(window, &QQuickWindow::frameSwapped, this, &Daemon::onFrameSwapped);
            window->hide();
        } else {
            m_rootObject->setProperty("visible", false);
        }
    }
}

void Daemon::setupSocketServer()
{
    const QString socketPath = Config::runtimeSocketPath();
    QLocalServer::removeServer(socketPath);

    QDir().mkpath(QFileInfo(socketPath).path());
    m_server = new QLocalServer(this);
    m_server->setSocketOptions(QLocalServer::UserAccessOption);
    if (!m_server->listen(socketPath)) {
        qCritical() << "Failed to start socket server:" << socketPath << m_server->errorString();
        m_server = nullptr;
        return;
    }

    connect(m_server, &QLocalServer::newConnection, this, &Daemon::onNewConnection);
    qDebug() << "Daemon listening on" << socketPath;
}

void Daemon::loadBundledOrLocalQml(const QString &path)
{
    const QString normalizedPath = path.isEmpty() ? Config::bundledThemePath() : path;
    const bool isResource = normalizedPath.startsWith(":/") || normalizedPath.startsWith("qrc:/");
    const QUrl url = isResource ? QUrl(normalizedPath) : QUrl::fromLocalFile(QFileInfo(normalizedPath).absoluteFilePath());

    m_currentQmlPath = normalizedPath;
    m_engine->load(url);
    if (m_engine->rootObjects().isEmpty()) {
        qWarning() << "Failed to load QML:" << normalizedPath << url;
        m_rootObject = nullptr;
        return;
    }

    m_rootObject = m_engine->rootObjects().last();

    if (!isResource) {
        const QFileInfo info(normalizedPath);
        if (info.exists())
            m_themeCache[normalizedPath] = info.lastModified().toMSecsSinceEpoch();
    }
}

void Daemon::onNewConnection()
{
    while (m_server && m_server->hasPendingConnections()) {
        QLocalSocket *socket = m_server->nextPendingConnection();
        if (!socket)
            continue;

        socket->setParent(this);
        m_socketBuffers.insert(socket, QByteArray());

        connect(socket, &QLocalSocket::readyRead, this, [this, socket]() {
            QByteArray &buffer = m_socketBuffers[socket];
            buffer += socket->readAll();

            if (buffer.size() > kMaxRequestSize) {
                qWarning() << "Rejected oversized IPC request";
                socket->abort();
                return;
            }

            while (true) {
                const qsizetype newline = buffer.indexOf('\n');
                if (newline < 0)
                    break;

                const QByteArray line = buffer.left(newline).trimmed();
                buffer.remove(0, newline + 1);
                if (!line.isEmpty())
                    processRequest(line);

                socket->disconnectFromServer();
                break;
            }
        });

        connect(socket, &QLocalSocket::disconnected, this, [this, socket]() {
            m_socketBuffers.remove(socket);
            socket->deleteLater();
        });
    }
}

void Daemon::processRequest(const QByteArray &data)
{
    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        qWarning() << "Invalid JSON request:" << err.errorString();
        return;
    }

    const QJsonObject req = doc.object();
    if (req.value("action").toString() != "show")
        return;

    m_model->setPromptText(req.value("prompt").toString());
    m_model->setPromptPositionText(req.value("prompt_position").toString());
    m_model->setPlaceholder(req.contains("placeholder") ? req.value("placeholder").toString() : QStringLiteral("Search..."));
    m_model->setWindowWidth(req.contains("width") ? req.value("width").toInt() : m_config.value("window_width", 500).toInt());
    m_model->setMaxVisibleItems(req.contains("max_items") ? req.value("max_items").toInt() : m_config.value("max_visible_items", 6).toInt());
    m_model->setShowTextField(!req.value("no_text_field").toBool(false));

    m_model->setProvider(req.value("provider").toString());
    m_model->setProviderWorkingDirectory(req.value("provider_cwd").toString());

    QStringList fields;
    if (req.value("fields").isArray()) {
        const QJsonArray arr = req.value("fields").toArray();
        fields.reserve(arr.size());
        for (const QJsonValue &v : arr) {
            if (v.isString())
                fields.append(v.toString());
        }
    }
    m_model->setFields(fields);

    QString requestedTheme = req.value("theme").toString();
    QString qmlPath = requestedTheme.isEmpty() ? m_themePath : requestedTheme;
    if (qmlPath.isEmpty())
        qmlPath = Config::bundledThemePath();

    bool exists = true;
    qint64 mtime = kMissingMtime;
    const bool isResource = qmlPath.startsWith(":/") || qmlPath.startsWith("qrc:/");
    if (!isResource) {
        const QFileInfo info(qmlPath);
        exists = info.exists() && info.isFile();
        if (exists)
            mtime = info.lastModified().toMSecsSinceEpoch();
    }

    if (!exists) {
        qWarning() << "Theme does not exist, using bundled theme:" << qmlPath;
        qmlPath = Config::bundledThemePath();
        mtime = kMissingMtime;
    }

    const bool themeChanged = qmlPath != m_currentQmlPath;
    const bool themeModified = !isResource && !themeChanged && m_themeCache.value(qmlPath, kMissingMtime) != mtime;

    if (themeChanged || themeModified) {
        if (m_rootObject) {
            m_rootObject->setProperty("visible", false);
            delete m_rootObject;
            m_rootObject = nullptr;
        }

        m_engine->clearComponentCache();
        loadBundledOrLocalQml(qmlPath);
        if (!m_rootObject) {
            qWarning() << "Theme reload failed; falling back to bundled theme";
            m_themeCache.clear();
            loadBundledOrLocalQml(Config::bundledThemePath());
        }
    }

    if (!m_model->provider().isEmpty())
        m_model->reloadProvider();
    else
        m_model->reload();

    showWindow();
}

void Daemon::showWindow()
{
    if (!m_rootObject)
        return;

    resetWindowForShow();
    m_rootObject->setProperty("visible", true);

    if (QQuickWindow *window = qobject_cast<QQuickWindow *>(m_rootObject)) {
        window->show();
        window->raise();
        window->requestActivate();
    }
}

void Daemon::resetWindowForShow()
{
    if (!m_rootObject)
        return;

    m_rootObject->setProperty("closing", false);
    QMetaObject::invokeMethod(m_rootObject, "resetAnimation", Qt::DirectConnection);
}

bool Daemon::isReady() const
{
    return m_rootObject != nullptr && !m_server.isNull() && m_server->isListening();
}

int Daemon::exec()
{
    if (!isReady())
        return 1;
    return QGuiApplication::exec();
}

void Daemon::cleanup()
{
    m_socketBuffers.clear();
    if (m_server) {
        m_server->close();
        m_server->deleteLater();
        m_server = nullptr;
    }

    const QString socketPath = Config::runtimeSocketPath();
    QLocalServer::removeServer(socketPath);
}
