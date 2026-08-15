#include "execparser.h"
#include <QRegularExpression>
#include <QProcess>

QPair<QString, QStringList> ExecParser::buildCommand(const QString &execLine)
{
    if (execLine.isEmpty())
        return {QString(), QStringList()};

    QStringList parts;
#if QT_VERSION >= QT_VERSION_CHECK(6, 3, 0)
    parts = QProcess::splitCommand(execLine);
#else
    // This is a simplified parser; may replace it with a proper parser in the future
    parts = execLine.split(QRegularExpression("\\s+(?=(?:[^\"]*\"[^\"]*\")*[^\"]*$)"));
#endif

    QString program;
    QStringList args;
    for (const QString &part : parts) {
        if (part == "%%") {
            args << "%";
            continue;
        }
        QString cleaned = part;
        cleaned.remove(QRegularExpression("%[fFuUiIckvm]"));
        if (!cleaned.isEmpty()) {
            if (program.isEmpty())
                program = cleaned;
            else
                args << cleaned;
        }
    }
    return {program, args};
}
