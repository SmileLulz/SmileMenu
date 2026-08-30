#include "desktopentry.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QStandardPaths>
#include <QLocale>

AppItem* DesktopEntry::fromFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return nullptr;

    QTextStream in(&file);
    QString line;
    bool inEntry = false;
    QMap<QString, QString> data;
    while (in.readLineInto(&line)) {
        line = line.trimmed();
        if (line == "[Desktop Entry]") {
            inEntry = true;
            continue;
        }
        if (line.startsWith('[')) {
            inEntry = false;
            continue;
        }
        if (!inEntry)
            continue;
        int sep = line.indexOf('=');
        if (sep == -1)
            continue;
        QString key = line.left(sep).trimmed();
        QString value = line.mid(sep + 1).trimmed();
        data[key] = value;
    }

    if (data.isEmpty())
        return nullptr;

    if (data.value("Type") != "Application")
        return nullptr;
    if (data.value("Hidden").toLower() == "true")
        return nullptr;
    if (data.value("NoDisplay").toLower() == "true")
        return nullptr;

    const QString tryExec = data.value("TryExec").trimmed();
    if (!tryExec.isEmpty() && QStandardPaths::findExecutable(tryExec).isEmpty())
        return nullptr;

    QString name = data.value("Name");
    const QString locale = QLocale::system().name();
    const QString language = locale.section('_', 0, 0);
    if (data.contains("Name[" + locale + "]"))
        name = data.value("Name[" + locale + "]");
    else if (data.contains("Name[" + language + "]"))
        name = data.value("Name[" + language + "]");
    QString command = data.value("Exec");
    QString icon = data.value("Icon");
    QStringList categories = data.value("Categories").split(';', Qt::SkipEmptyParts);

    if (name.isEmpty() || command.isEmpty())
        return nullptr;

    return new AppItem(name, command, icon, QString(), categories);
}
