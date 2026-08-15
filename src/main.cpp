#include <QGuiApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QDebug>
#include <QLocalSocket>
#include <QJsonArray>
#include <unistd.h>
#include "config.h"
#include "lock.h"
#include "daemon.h"
#include "desktopentry.h"

static const QString SOCKET_PATH = QString("/tmp/smilemenu-%1.sock").arg(getuid());

bool tryConnectToDaemon(QLocalSocket &socket)
{
    socket.connectToServer(SOCKET_PATH);
    return socket.waitForConnected(100);
}

void sendRequest(QLocalSocket &socket, const QJsonObject &request)
{
    QByteArray data = QJsonDocument(request).toJson(QJsonDocument::Compact) + '\n';
    socket.write(data);
    socket.flush();
    socket.waitForBytesWritten();
}

int handleDaemonRequest(const QCommandLineParser &parser)
{
    QLocalSocket socket;
    if (!tryConnectToDaemon(socket)) {
        qCritical() << "Error: daemon not running. Start it with 'smilemenu --daemon'";
        return 1;
    }

    QJsonObject req;
    req["action"] = "show";
    req["prompt"] = parser.value("prompt");
    req["prompt_position"] = parser.value("prompt-position");
    req["placeholder"] = parser.value("placeholder");
    req["provider"] = parser.value("provider");
    req["fields"] = QJsonArray::fromStringList(parser.values("field"));
    if (parser.isSet("width"))
        req["width"] = parser.value("width").toInt();
    if (parser.isSet("max-items"))
        req["max_items"] = parser.value("max-items").toInt();
    if (parser.isSet("no-text-field"))
        req["no_text_field"] = true;
    if (parser.isSet("theme"))
        req["theme"] = parser.value("theme");

    sendRequest(socket, req);
    socket.disconnectFromServer();
    return 0;
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("smilemenu");
    app.setOrganizationName("SmileLulz404");

    QCommandLineParser parser;
    parser.setApplicationDescription("SmileMenu launcher");
    parser.addHelpOption();

    QCommandLineOption daemonOption("daemon", "Run the daemon");
    parser.addOption(daemonOption);

    QCommandLineOption promptOption({"p", "prompt"}, "Set a custom prompt", "text");
    parser.addOption(promptOption);

    QCommandLineOption promptPosOption({"pp", "prompt-position"}, "Prompt position (top, entry, hidden)", "pos", "entry");
    parser.addOption(promptPosOption);

    QCommandLineOption placeholderOption({"ph", "placeholder"}, "Set placeholder text", "text", "Search...");
    parser.addOption(placeholderOption);

    QCommandLineOption providerOption("provider", "Use a provider script", "path");
    parser.addOption(providerOption);

    QCommandLineOption fieldOption("field", "Set fields as custom modes (can be repeated)", "field");
    parser.addOption(fieldOption);

    QCommandLineOption widthOption({"w", "width"}, "Set custom window width", "pixels");
    parser.addOption(widthOption);

    QCommandLineOption noTextFieldOption({"ntf", "no-text-field"}, "Hide the text field");
    parser.addOption(noTextFieldOption);

    QCommandLineOption maxItemsOption({"mi", "max-items"}, "Set max visible items", "count");
    parser.addOption(maxItemsOption);

    QCommandLineOption genConfigOption({"gc", "gen-config"}, "Generate config file");
    parser.addOption(genConfigOption);

    QCommandLineOption genThemeOption({"gt", "gen-theme"}, "Generate QML theme file");
    parser.addOption(genThemeOption);

    QCommandLineOption configOption({"c", "config"}, "Use custom config file", "file");
    parser.addOption(configOption);

    QCommandLineOption themeOption({"t", "theme"}, "Use custom QML theme file", "file");
    parser.addOption(themeOption);

    QCommandLineOption dcacheOption("dcache", "Delete cache: 'app' or 'all'", "type");
    parser.addOption(dcacheOption);

    parser.process(app);

    QString configPath = parser.value(configOption);
    if (configPath.isEmpty())
        configPath = Config::defaultPath();

    if (parser.isSet(genConfigOption)) {
        if (QFile::exists(configPath)) {
            qDebug() << "Config already exists:" << configPath;
            return 0;
        }
        Config::saveDefault(configPath);
        qDebug() << "Generated config:" << configPath;
        return 0;
    }

    if (parser.isSet(genThemeOption)) {
        QString themePath = parser.value(themeOption);
        if (themePath.isEmpty())
            themePath = QDir::homePath() + "/.config/smilemenu/theme/Main.qml";

        if (QFile::exists(themePath)) {
            qDebug() << "Theme file already exists:" << themePath;
            return 0;
        }

        QDir().mkpath(QFileInfo(themePath).path());

        QFile defaultQml(":/SmileMenu/qml/Main.qml");
        if (!defaultQml.open(QIODevice::ReadOnly)) {
            qCritical() << "Failed to open default QML resource. Make sure the QML module is built.";
            return 1;
        }

        QByteArray content = defaultQml.readAll();
        defaultQml.close();

        QFile out(themePath);
        if (!out.open(QIODevice::WriteOnly)) {
            qCritical() << "Failed to write theme file:" << themePath;
            return 1;
        }
        out.write(content);
        out.close();

        qDebug() << "Generated QML theme template:" << themePath;
        return 0;
    }

    if (parser.isSet(dcacheOption)) {
        QString type = parser.value(dcacheOption);
        QDir cacheDir(QDir::homePath() + "/.cache/smilemenu");
        if (type == "app") {
            QString cacheFile = cacheDir.filePath("apps_cache.json");
            if (QFile::remove(cacheFile))
                qDebug() << "Removed app cache:" << cacheFile;
            else
                qDebug() << "No app cache found";
        } else if (type == "all") {
            if (cacheDir.exists()) {
                if (cacheDir.removeRecursively())
                    qDebug() << "Removed entire cache directory:" << cacheDir.path();
                else
                    qDebug() << "Failed to remove cache directory";
            } else {
                qDebug() << "No cache directory found";
            }
        }
        return 0;
    }

    // --daemon mode
    if (parser.isSet(daemonOption)) {
        SingleInstanceLock lock("smilemenu");
        if (!lock.tryLock()) {
            qDebug() << "SmileMenu daemon is already running";
            return 0;
        }

        QVariantMap config = Config::load(configPath);

        QString themePath = parser.value(themeOption);
        if (themePath.isEmpty()) {
            QString userTheme = QDir::homePath() + "/.config/smilemenu/theme/Main.qml";
            if (QFile::exists(userTheme))
                themePath = userTheme;
            else
                themePath = ":/SmileMenu/qml/Main.qml";
        }

        Daemon daemon(config, themePath);
        return app.exec();
    }

    // Client mode: connect to daemon
    return handleDaemonRequest(parser);
}
