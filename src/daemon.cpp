#include "daemon.h"
#include "model.h"
#include "iconprovider.h"
#include "config.h"
#include <QQmlContext>
#include <QQuickWindow>
#include <QLocalSocket>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileInfo>
#include <QTimer>
#include <QDebug>
#include <QDir>
#include <unistd.h>

Daemon::Daemon(const QVariantMap &config, const QString &themePath, QObject *parent)
    : QObject(parent), m_config(config), m_themePath(themePath),
      m_themeCacheLimit(config.value("max_theme_cache_size", 5).toInt())
{
    m_engine = new QQmlApplicationEngine(this);

    int iconCacheSize = config.value("max_icon_cache_size", 256).toInt();
    m_engine->addImageProvider("icons", new IconProvider(iconCacheSize));

    m_model = new LauncherModel(config, this);
    m_engine->rootContext()->setContextProperty("launcher", m_model);
    m_engine->rootContext()->setContextProperty("preload_mode", true);

    loadQml(m_themePath);

    QObject *root = m_engine->rootObjects().isEmpty() ? nullptr : m_engine->rootObjects().first();
    if (root) {
        QQuickWindow *window = qobject_cast<QQuickWindow*>(root);
        if (window) {
            connect(window, &QQuickWindow::frameSwapped, this, &Daemon::onFrameSwapped);
            // Show window hidden initially
            window->setVisible(true);
        }
    }
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
        QQuickWindow *window = qobject_cast<QQuickWindow*>(m_rootObject);
        if (window)
            disconnect(window, &QQuickWindow::frameSwapped, this, &Daemon::onFrameSwapped);
    }
    if (m_rootObject) {
        m_rootObject->setProperty("visible", false);
    }
    setupSocketServer();
}

void Daemon::setupSocketServer()
{
    QString socketPath = QString("/tmp/smilemenu-%1.sock").arg(getuid());
    QLocalServer::removeServer(socketPath);
    m_server = new QLocalServer(this);
    if (!m_server->listen(socketPath)) {
        qCritical() << "Failed to start socket server:" << m_server->errorString();
        return;
    }
    connect(m_server, &QLocalServer::newConnection, this, &Daemon::onNewConnection);
    qDebug() << "Daemon listening on" << socketPath;
}

void Daemon::loadQml(const QString &path)
{
    m_currentQmlPath = path;
    m_engine->load(QUrl::fromLocalFile(path));
    if (m_engine->rootObjects().isEmpty()) {
        qWarning() << "Failed to load QML:" << path;
        return;
    }
    m_rootObject = m_engine->rootObjects().first();
    QFileInfo info(path);
    if (info.exists())
        m_themeCache[path] = info.lastModified().toSecsSinceEpoch();
}

void Daemon::onNewConnection()
{
    QLocalSocket *socket = m_server->nextPendingConnection();
    if (!socket) return;
    socket->setParent(this);
    connect(socket, &QLocalSocket::readyRead, [this, socket]() {
        QByteArray data = socket->readAll();
        while (data.contains('\n')) {
            int idx = data.indexOf('\n');
            QByteArray line = data.left(idx);
            data.remove(0, idx + 1);
            processRequest(line);
        }
        socket->disconnectFromServer();
    });
    connect(socket, &QLocalSocket::disconnected, socket, &QLocalSocket::deleteLater);
}

void Daemon::processRequest(const QByteArray &data)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError) {
        qWarning() << "Invalid JSON request:" << data;
        return;
    }
    QJsonObject req = doc.object();
    if (req.value("action").toString() != "show")
        return;

    m_model->setPromptText("");
    m_model->setPromptPositionText("entry");
    m_model->setPlaceholder("Search...");
    m_model->setWindowWidth(m_config.value("window_width", 500).toInt());
    m_model->setMaxVisibleItems(m_config.value("max_visible_items", 6).toInt());
    m_model->setShowTextField(m_config.value("show_text_field", true).toBool());
    m_model->setProvider("");
    m_model->setFields(QStringList());

    QString requestedTheme = req.value("theme").toString();
    QString qmlPath = requestedTheme.isEmpty() ? m_themePath : requestedTheme;
    if (qmlPath != m_currentQmlPath) {
        if (!QFile::exists(qmlPath)) {
            qmlPath = ":/SmileMenu/Main.qml";
        }
        QFileInfo info(qmlPath);
        qint64 mtime = info.exists() ? info.lastModified().toSecsSinceEpoch() : 0;
        bool needReload = false;
        if (qmlPath != m_currentQmlPath) {
            needReload = true;
        } else if (m_themeCache.contains(qmlPath) && m_themeCache[qmlPath] != mtime) {
            needReload = true;
        }

        if (needReload) {
            if (!m_themeCache.contains(qmlPath) && m_themeCache.size() >= m_themeCacheLimit) {
                m_engine->clearComponentCache();
                m_themeCache.clear();
            }
            loadQml(qmlPath);
            if (!m_engine->rootObjects().isEmpty()) {
                m_rootObject = m_engine->rootObjects().first();
                m_currentQmlPath = qmlPath;
                m_themeCache[qmlPath] = mtime;
            }
        }
    }

    if (req.contains("prompt"))
        m_model->setPromptText(req["prompt"].toString());
    if (req.contains("prompt_position"))
        m_model->setPromptPositionText(req["prompt_position"].toString());
    if (req.contains("placeholder"))
        m_model->setPlaceholder(req["placeholder"].toString());
    if (req.contains("width"))
        m_model->setWindowWidth(req["width"].toInt());
    if (req.contains("max_items"))
        m_model->setMaxVisibleItems(req["max_items"].toInt());
    if (req.contains("no_text_field"))
        m_model->setShowTextField(!req["no_text_field"].toBool());
    if (req.contains("provider"))
        m_model->setProvider(req["provider"].toString());
    if (req.contains("fields") && req["fields"].isArray()) {
        QStringList fields;
        QJsonArray arr = req["fields"].toArray();
        for (const QJsonValue &v : arr)
            fields << v.toString();
        m_model->setFields(fields);
    }

    if (!m_model->provider().isEmpty())
        m_model->reloadProvider();
    else
        m_model->reload();

    showWindow();
}

void Daemon::showWindow()
{
    if (!m_rootObject) return;
    m_rootObject->setProperty("visible", true);
    m_rootObject->setProperty("closing", false);
    QMetaObject::invokeMethod(m_rootObject, "resetAnimation");
    QQuickWindow *window = qobject_cast<QQuickWindow*>(m_rootObject);
    if (window) {
        window->show();
        window->raise();
        window->requestActivate();
    }
}

void Daemon::hideWindow()
{
    if (m_rootObject) {
        m_rootObject->setProperty("visible", false);
    }
}

int Daemon::exec()
{
    return QGuiApplication::exec();
}

void Daemon::cleanup()
{
    if (m_server) {
        m_server->close();
        delete m_server;
        m_server = nullptr;
    }
    QString socketPath = QString("/tmp/smilemenu-%1.sock").arg(getuid());
    QLocalServer::removeServer(socketPath);
}
