#include "providers.h"
#include <QFileInfo>
#include <QDir>

namespace {
QString displayText(const QString &value)
{
    constexpr int kMaxDisplayChars = 4096;
    if (value.size() <= kMaxDisplayChars)
        return value;
    return value.left(kMaxDisplayChars) + QStringLiteral(" …");
}

void setField(AppItem *item, const QString &key, const QString &value)
{
    if (key == "name") item->setName(displayText(value));
    else if (key == "command") item->setCommand(value);
    else if (key == "icon") item->setIcon(value);
    else if (key == "description") item->setDescription(displayText(value));
}
}

AppItem* Providers::fromLine(const QString &line, const QStringList &fields)
{
    const QStringList columns = line.split('\t');
    const QFileInfo info(line);

    auto *item = new AppItem(
        columns.isEmpty() ? displayText(line) : displayText(columns.first()),
        line,
        info.isFile() || info.isDir() ? info.absoluteFilePath() : QString());

    if (fields.isEmpty())
        return item;

    for (const QString &field : fields) {
        const int separator = field.indexOf(':');
        if (separator <= 0)
            continue;

        const QString key = field.left(separator).trimmed().toLower();
        const QString mode = field.mid(separator + 1).trimmed();

        if (mode == "path") {
            if (key == "name") setField(item, key, info.fileName());
            else setField(item, key, line);
        } else if (mode == "text") {
            setField(item, key, line);
        } else if (mode == "none") {
            setField(item, key, QString());
        } else {
            bool ok = false;
            const int index = mode.toInt(&ok) - 1;
            if (ok && index >= 0 && index < columns.size())
                setField(item, key, columns.at(index));
        }
    }

    return item;
}

QString Providers::getDisplayName(const QString &value, const QString &displayColumns)
{
    if (displayColumns.isEmpty())
        return value;

    const QStringList columns = value.split('\t');
    QStringList result;
    for (const QString &idxStr : displayColumns.split(',')) {
        bool ok = false;
        const int idx = idxStr.trimmed().toInt(&ok) - 1;
        if (ok && idx >= 0 && idx < columns.size())
            result << columns.at(idx);
    }
    return result.join('\t');
}
