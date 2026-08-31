#include "execparser.h"
#include <QProcess>
#include <QRegularExpression>

QPair<QString, QStringList> ExecParser::buildCommand(const QString &execLine)
{
    if (execLine.trimmed().isEmpty())
        return {};

    const QStringList parts = QProcess::splitCommand(execLine);
    if (parts.isEmpty())
        return {};

    QStringList cleanedParts;
    cleanedParts.reserve(parts.size());

    static const QRegularExpression fieldCode(QStringLiteral("%(?:[fFuUickvm])"));
    for (const QString &part : parts) {
        if (part == QStringLiteral("%%")) {
            cleanedParts.append(QStringLiteral("%"));
            continue;
        }

        QString cleaned = part;
        cleaned.replace(fieldCode, QString());
        if (!cleaned.isEmpty())
            cleanedParts.append(cleaned);
    }

    if (cleanedParts.isEmpty())
        return {};

    const QString program = cleanedParts.takeFirst();
    return {program, cleanedParts};
}
