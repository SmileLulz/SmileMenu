#include "providers.h"

#include <QFileInfo>

namespace {
constexpr int kMaxDisplayChars = 4096;

QString displayText(const QString &value)
{
    if (value.size() <= kMaxDisplayChars)
        return value;
    return value.left(kMaxDisplayChars) + QStringLiteral(" …");
}

}

AppItem *Providers::fromLine(const QString &line, const QStringList &fields)
{
    const QStringList columns = line.split(QChar('\t'), Qt::KeepEmptyParts);
    const QFileInfo info(line);

    QString name = columns.isEmpty() ? displayText(line) : displayText(columns.first());
    QString icon;
    QString description;

    if (info.exists())
        icon = info.absoluteFilePath();

    for (const QString &field : fields) {
        const qsizetype separator = field.indexOf(QChar(':'));
        if (separator <= 0)
            continue;

        const QString key = field.left(separator).trimmed().toLower();
        const QString mode = field.mid(separator + 1).trimmed();

        if (key != QStringLiteral("name") &&
            key != QStringLiteral("icon") &&
            key != QStringLiteral("description")) {
            continue;
        }

        if (mode == QStringLiteral("path")) {
            const QString value = (key == QStringLiteral("name"))
                ? info.fileName()
                : line;
            if (key == QStringLiteral("name"))
                name = displayText(value);
            else if (key == QStringLiteral("icon"))
                icon = value;
            else
                description = displayText(value);
        } else if (mode == QStringLiteral("text")) {
            if (key == QStringLiteral("name"))
                name = displayText(line);
            else if (key == QStringLiteral("icon"))
                icon = line;
            else
                description = displayText(line);
        } else if (mode == QStringLiteral("none")) {
            if (key == QStringLiteral("name"))
                name.clear();
            else if (key == QStringLiteral("icon"))
                icon.clear();
            else
                description.clear();
        } else {
            bool ok = false;
            const int column = mode.toInt(&ok) - 1;
            if (!ok || column < 0 || column >= columns.size())
                continue;

            const QString value = columns.at(column);
            if (key == QStringLiteral("name"))
                name = displayText(value);
            else if (key == QStringLiteral("icon"))
                icon = value;
            else
                description = displayText(value);
        }
    }

    return new AppItem(name, line, icon, description);
}

QString Providers::getDisplayName(const QString &value, const QString &displayColumns)
{
    if (displayColumns.trimmed().isEmpty())
        return value;

    const QStringList columns = value.split(QChar('\t'), Qt::KeepEmptyParts);
    QStringList result;

    for (const QString &idxStr : displayColumns.split(',')) {
        bool ok = false;
        const int index = idxStr.trimmed().toInt(&ok) - 1;
        if (ok && index >= 0 && index < columns.size())
            result << columns.at(index);
    }

    return result.join(QChar('\t'));
}
