#include "providers.h"
#include <QFileInfo>
#include <QDebug>

AppItem* Providers::fromLine(const QString &line, const QStringList &fields)
{
    QStringList columns = line.split('\t');
    QString value = line;

    QFileInfo info(value);
    QString name = info.fileName();
    QString icon = info.exists() ? value : "";
    QString desc;

    if (!fields.isEmpty()) {
        for (const QString &field : fields) {
            QStringList parts = field.split(':');
            if (parts.size() != 2) continue;
            QString key = parts[0];
            QString mode = parts[1];
            if (mode == "path") {
                if (key == "name")
                    name = info.fileName();
                else if (key == "icon")
                    icon = value;
            } else if (mode == "text") {
                if (key == "name")
                    name = value;
                else if (key == "icon")
                    icon = value;
            } else if (mode == "none") {
                if (key == "name")
                    name = "";
                else if (key == "icon")
                    icon = "";
            } else {
                bool ok;
                int idx = mode.toInt(&ok) - 1;
                if (ok && idx >= 0 && idx < columns.size()) {
                    if (key == "name")
                        name = columns[idx];
                    else if (key == "icon")
                        icon = columns[idx];
                    else if (key == "description")
                        desc = columns[idx];
                }
            }
        }
    }

    if (fields.isEmpty()) {
        name = columns.isEmpty() ? value : columns[0];
    }

    return new AppItem(name, value, icon, desc);
}

QString Providers::getDisplayName(const QString &value, const QString &displayColumns)
{
    if (displayColumns.isEmpty())
        return value;
    QStringList columns = value.split('\t');
    QStringList result;
    QStringList indices = displayColumns.split(',');
    for (const QString &idxStr : indices) {
        bool ok;
        int idx = idxStr.trimmed().toInt(&ok) - 1;
        if (ok && idx >= 0 && idx < columns.size())
            result << columns[idx];
    }
    return result.join('\t');
}
