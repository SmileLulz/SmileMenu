#include <QGuiApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QSaveFile>
#include <QDebug>
#include <QLocalSocket>
#include <QJsonArray>
#include <functional>
#include "config.h"
#include "lock.h"
#include "daemon.h"
#include "desktopentry.h"

static const QString SOCKET_PATH = Config::runtimeSocketPath();

static QString resolveProvider(const QString &provider)
{
    const QString value = provider.trimmed();
    if (value.isEmpty())
        return {};

    const QDir cwd(QDir::currentPath());
    const QFileInfo input(value);

    if (value.contains('/')) {
        if (input.isAbsolute())
            return input.absoluteFilePath();
        return cwd.absoluteFilePath(value);
    }

    const QFileInfo local(cwd.filePath(value));
    if (local.exists() && local.isFile())
        return local.absoluteFilePath();

    const QString executable = QStandardPaths::findExecutable(value);
    if (!executable.isEmpty())
        return executable;

    return value;
}

bool tryConnectToDaemon(QLocalSocket &socket)
{
    socket.connectToServer(SOCKET_PATH);
    return socket.waitForConnected(1000);
}

bool sendRequest(QLocalSocket &socket, const QJsonObject &request)
{
    const QByteArray data = QJsonDocument(request).toJson(QJsonDocument::Compact) + '\n';
    if (socket.write(data) != data.size() || !socket.waitForBytesWritten(1000)) {
        qWarning() << "Failed to send request to daemon:" << socket.errorString();
        return false;
    }
    return true;
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
    req["provider"] = resolveProvider(parser.value("provider"));
    req["provider_cwd"] = QDir::currentPath();
    req["fields"] = QJsonArray::fromStringList(parser.values("field"));
    if (parser.isSet("width"))
        req["width"] = parser.value("width").toInt();
    if (parser.isSet("max-items"))
        req["max_items"] = parser.value("max-items").toInt();
    if (parser.isSet("no-text-field"))
        req["no_text_field"] = true;
    if (parser.isSet("theme"))
        req["theme"] = parser.value("theme");

    const bool sent = sendRequest(socket, req);
    socket.disconnectFromServer();
    return sent ? 0 : 1;
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("smilemenu");
    app.setOrganizationName("SmileLulz");
    QCoreApplication::setApplicationVersion(SMILEMENU_VERSION);

    QCommandLineParser parser;

    parser.setApplicationDescription("A fast and lightweight application launcher and utility menu");

    QCommandLineOption helpOption(QStringList() << "help" << "h", "Show help message");
    parser.addOption(helpOption);

    QCommandLineOption versionOption(QStringList() << "version" << "v", "Show current version");
    parser.addOption(versionOption);

    QCommandLineOption daemonOption({"daemon", "d"}, "Run the daemon");
    parser.addOption(daemonOption);

    QCommandLineOption promptOption({"prompt", "p"}, "Set a custom prompt", "text");
    parser.addOption(promptOption);

    QCommandLineOption promptPosOption({"prompt-position", "pp"}, "Prompt position (top, entry, hidden)", "pos", "entry");
    parser.addOption(promptPosOption);

    QCommandLineOption placeholderOption({"placeholder", "ph"}, "Set placeholder text", "text", "Search...");
    parser.addOption(placeholderOption);

    QCommandLineOption providerOption({"provider", "pv"}, "Use a provider script", "path");
    parser.addOption(providerOption);

    QCommandLineOption fieldOption({"field", "f"}, "Set provider presentation fields (name, icon, description; repeatable)", "field");
    parser.addOption(fieldOption);

    QCommandLineOption widthOption({"width", "w"}, "Set custom window width", "pixels");
    parser.addOption(widthOption);

    QCommandLineOption noTextFieldOption({"no-text-field", "ntf"}, "Hide the text field");
    parser.addOption(noTextFieldOption);

    QCommandLineOption maxItemsOption({"max-items", "mi"}, "Set max visible items", "count");
    parser.addOption(maxItemsOption);

    QCommandLineOption genConfigOption({"gen-config", "gc"}, "Generate config file");
    parser.addOption(genConfigOption);

    QCommandLineOption genThemeOption({"gen-theme", "gt"}, "Generate the default QML theme files");
    parser.addOption(genThemeOption);

    QCommandLineOption configOption({"config", "c"}, "Use custom config file", "file");
    parser.addOption(configOption);

    QCommandLineOption themeOption({"theme", "t"}, "Use custom QML theme file", "file");
    parser.addOption(themeOption);

    QCommandLineOption dcacheOption({"dcache", "dc"}, "Delete cache: 'app' or 'all'", "type");
    parser.addOption(dcacheOption);

    parser.process(app);

    if (parser.isSet(helpOption)) {
        parser.showHelp();
    }

    if (parser.isSet(versionOption)) {
        qInfo().noquote() << "SmileMenu" << QCoreApplication::applicationVersion();
        return 0;
    }

    if (parser.isSet(widthOption)) {
        bool ok = false;
        const int width = parser.value(widthOption).toInt(&ok);
        if (!ok || width < 200 || width > 4096) {
            qCritical() << "--width must be an integer between 200 and 4096";
            return 2;
        }
    }
    if (parser.isSet(maxItemsOption)) {
        bool ok = false;
        const int count = parser.value(maxItemsOption).toInt(&ok);
        if (!ok || count < 1 || count > 100) {
            qCritical() << "--max-items must be an integer between 1 and 100";
            return 2;
        }
    }

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
        QString themeDir = parser.value(themeOption);
        if (themeDir.isEmpty())
            themeDir = QDir::homePath() + "/.config/smilemenu/theme";
        else if (QFileInfo(themeDir).suffix() == QStringLiteral("qml"))
            themeDir = QFileInfo(themeDir).path();

        QDir dir(themeDir);
        if (dir.exists() && !dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries).isEmpty()) {
            qDebug() << "Theme directory is not empty:" << themeDir;
            return 0;
        }

        if (!dir.mkpath(QStringLiteral("."))) {
            qCritical() << "Failed to create theme directory:" << themeDir;
            return 1;
        }

        const QString resourceRoot = QStringLiteral(":/qt/qml/SmileMenu/qml");

        std::function<bool(const QString &, const QString &)> copyDirectory =
            [&](const QString &resourceDirPath, const QString &targetDirPath) -> bool {
                QDir resourceDir(resourceDirPath);
                if (!resourceDir.exists()) {
                    qCritical() << "Failed to open bundled QML directory:" << resourceDirPath;
                    return false;
                }

                QDir targetDir(targetDirPath);
                if (!targetDir.exists() && !targetDir.mkpath(QStringLiteral("."))) {
                    qCritical() << "Failed to create theme directory:" << targetDirPath;
                    return false;
                }

                const QFileInfoList entries = resourceDir.entryInfoList(
                    QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs,
                    QDir::DirsFirst | QDir::Name);

                for (const QFileInfo &entry : entries) {
                    const QString name = entry.fileName();
                    const QString resourcePath = resourceDir.filePath(name);
                    const QString targetPath = targetDir.filePath(name);

                    if (entry.isDir()) {
                        if (!copyDirectory(resourcePath, targetPath))
                            return false;
                        continue;
                    }

                    if (!name.endsWith(QStringLiteral(".qml")))
                        continue;

                    QFile input(resourcePath);
                    if (!input.open(QIODevice::ReadOnly)) {
                        qCritical() << "Failed to open default QML resource:" << resourcePath;
                        return false;
                    }

                    QSaveFile output(targetPath);
                    if (!output.open(QIODevice::WriteOnly)) {
                        qCritical() << "Failed to write theme file:" << targetPath;
                        return false;
                    }

                    const QByteArray content = input.readAll();
                    if (output.write(content) != content.size() || !output.commit()) {
                        qCritical() << "Failed to atomically write theme file:" << targetPath;
                        return false;
                    }
                }

                return true;
            };

        if (!copyDirectory(resourceRoot, themeDir))
            return 1;

        QFile qmldir(dir.filePath(QStringLiteral("qmldir")));
        if (!qmldir.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            qCritical() << "Failed to write theme qmldir:" << qmldir.fileName();
            return 1;
        }

        const QByteArray qmldirContent =
            "module SmileMenuTheme\n"
            "singleton Api 1.0 Api.qml\n"
            "Main 1.0 Main.qml\n"
            "SearchField 1.0 SearchField.qml\n"
            "ItemList 1.0 ItemList.qml\n"
            "MenuItem 1.0 MenuItem.qml\n";

        if (qmldir.write(qmldirContent) != qmldirContent.size()) {
            qCritical() << "Failed to write theme qmldir:" << qmldir.fileName();
            qmldir.close();
            return 1;
        }

        qmldir.close();

        qDebug() << "Generated QML theme directory:" << themeDir;
        return 0;
    }

    if (parser.isSet(dcacheOption)) {
        QString type = parser.value(dcacheOption);
        if (type != "app" && type != "all") {
            qCritical() << "Invalid --dcache value:" << type;
            return 2;
        }
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
                themePath = Config::bundledThemePath();
        }

        Daemon daemon(config, themePath);
        return daemon.exec();
    }

    return handleDaemonRequest(parser);
}
