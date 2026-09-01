#include "model.h"
#include "launcher.h"
#include "history.h"
#include "execparser.h"
#include "providers.h"
#include <QProcess>
#include <QDebug>
#include <QTimer>
#include <QDir>
#include <QFileInfo>
#include <QtGlobal>
#include <unistd.h>
#include <algorithm>

QList<AppItem*> loadApplications();

LauncherModel::LauncherModel(const QVariantMap &config, QObject *parent)
    : QObject(parent), m_config(config),
      m_prompt(""), m_promptPosition("entry"), m_placeholder("Search..."),
      m_windowWidth(qBound(200, config.value("window_width", 500).toInt(), 4096)),
      m_maxVisibleItems(qBound(1, config.value("max_visible_items", 6).toInt(), 100)),
      m_showTextField(config.value("show_text_field", true).toBool()),
      m_fuzzySearch(config.value("fuzzy_search", true).toBool()),
      m_historyLimit(qMax(0, config.value("history_limit", 3).toInt())),
      m_providerProcess(nullptr), m_providerTimeout(nullptr)
{
    m_history = History::load();
    reload();
}

LauncherModel::~LauncherModel()
{
    qDeleteAll(m_allApps);
    if (m_providerTimeout) {
        m_providerTimeout->stop();
        m_providerTimeout->deleteLater();
        m_providerTimeout = nullptr;
    }
    if (m_providerProcess) {
        QObject::disconnect(m_providerProcess, nullptr, this, nullptr);
        if (m_providerProcess->state() != QProcess::NotRunning)
            m_providerProcess->kill();
        m_providerProcess->deleteLater();
        m_providerProcess = nullptr;
    }
}

QString LauncherModel::promptText() const { return m_prompt; }
void LauncherModel::setPromptText(const QString &text) {
    if (m_prompt != text) { m_prompt = text; emit promptTextChanged(); }
}
QString LauncherModel::promptPositionText() const { return m_promptPosition; }
void LauncherModel::setPromptPositionText(const QString &pos) {
    QString normalized = pos.trimmed().toLower();
    if (normalized != "top" && normalized != "entry" && normalized != "hidden")
        normalized = "entry";
    if (m_promptPosition != normalized) { m_promptPosition = normalized; emit promptPositionTextChanged(); }
}
QString LauncherModel::placeholder() const { return m_placeholder; }
void LauncherModel::setPlaceholder(const QString &text) {
    if (m_placeholder != text) { m_placeholder = text; emit placeholderChanged(); }
}
int LauncherModel::windowWidth() const { return m_windowWidth; }
void LauncherModel::setWindowWidth(int w) {
    w = qBound(200, w, 4096);
    if (m_windowWidth != w) { m_windowWidth = w; emit windowWidthChanged(); }
}
int LauncherModel::maxVisibleItems() const { return m_maxVisibleItems; }
void LauncherModel::setMaxVisibleItems(int items) {
    items = qBound(1, items, 100);
    if (m_maxVisibleItems != items) { m_maxVisibleItems = items; emit maxVisibleItemsChanged(); }
}
bool LauncherModel::showTextField() const { return m_showTextField; }
void LauncherModel::setShowTextField(bool show) {
    if (m_showTextField != show) { m_showTextField = show; emit showTextFieldChanged(); }
}
int LauncherModel::historyLimit() const { return m_historyLimit; }
int LauncherModel::minVisibleItems() const { return qBound(0, m_config.value("min_visible_items", 1).toInt(), 100); }
bool LauncherModel::cycle() const { return m_config.value("cycle", false).toBool(); }

QVariantList LauncherModel::apps() const {
    return m_appsVariant;
}

void LauncherModel::setProvider(const QString &provider)
{
    const QString normalized = provider.trimmed();
    if (m_provider == normalized)
        return;

    ++m_providerGeneration;
    if (m_providerTimeout) {
        m_providerTimeout->stop();
        m_providerTimeout->deleteLater();
        m_providerTimeout = nullptr;
    }
    if (m_providerProcess) {
        QObject::disconnect(m_providerProcess, nullptr, this, nullptr);
        if (m_providerProcess->state() != QProcess::NotRunning)
            m_providerProcess->kill();
        m_providerProcess->deleteLater();
        m_providerProcess = nullptr;
    }

    m_providerOutput.clear();
    m_providerErrorOutput.clear();
    m_provider = normalized;
}

void LauncherModel::setProviderWorkingDirectory(const QString &directory)
{
    QString normalized = directory.trimmed();
    if (normalized.isEmpty())
        normalized = QDir::homePath();

    const QFileInfo info(normalized);
    if (info.exists() && info.isDir())
        m_providerWorkingDirectory = info.absoluteFilePath();
    else
        m_providerWorkingDirectory = QDir::homePath();
}

void LauncherModel::setFields(const QStringList &fields)
{
    m_fields = fields;
}

bool LauncherModel::fuzzyMatch(const QString &query, const QString &text) const
{
    if (query.isEmpty()) return true;
    QString q = query.toLower();
    QString t = text.toLower();
    int idx = 0;
    for (QChar ch : t) {
        if (idx < q.length() && ch == q[idx])
            idx++;
    }
    return idx == q.length();
}

bool LauncherModel::matchApp(const QString &query, AppItem *app) const
{
    if (query.isEmpty()) return true;
    if (m_fuzzySearch) {
        if (fuzzyMatch(query, app->name())) return true;
        for (const QString &cat : app->categories()) {
            if (fuzzyMatch(query, cat)) return true;
        }
    } else {
        QString q = query.toLower();
        if (app->name().toLower().contains(q)) return true;
        for (const QString &cat : app->categories()) {
            if (cat.toLower().contains(q)) return true;
        }
    }
    return false;
}

void LauncherModel::applyHistory(QList<AppItem*> &apps)
{
    QList<QPair<int, AppItem*>> scored;
    QList<AppItem*> others;
    for (AppItem *app : apps) {
        int score = m_history.value(app->command(), 0);
        if (score > 0)
            scored.append({score, app});
        else
            others.append(app);
    }
    std::sort(scored.begin(), scored.end(),
              [](const QPair<int, AppItem*> &a, const QPair<int, AppItem*> &b) {
                  return a.first > b.first;
              });

    QList<AppItem*> top, rest;
    int count = qMin(m_historyLimit, static_cast<int>(scored.size()));
    for (int i = 0; i < count; ++i)
        top.append(scored[i].second);
    for (int i = count; i < scored.size(); ++i)
        rest.append(scored[i].second);

    std::sort(rest.begin(), rest.end(),
              [](AppItem *a, AppItem *b) { return a->name().toLower() < b->name().toLower(); });
    std::sort(others.begin(), others.end(),
              [](AppItem *a, AppItem *b) { return a->name().toLower() < b->name().toLower(); });

    apps.clear();
    apps.append(top);
    apps.append(rest);
    apps.append(others);
}

void LauncherModel::loadAllApps()
{
    qDeleteAll(m_allApps);
    m_allApps.clear();

    if (!m_provider.isEmpty()) {
        return;
    }

    QList<AppItem*> apps = loadApplications();
    m_allApps = apps;
    applyHistory(m_allApps);
    filterApps(m_searchText);
}

void LauncherModel::filterApps(const QString &query)
{
    m_filtered.clear();
    for (AppItem *app : m_allApps) {
        if (matchApp(query, app))
            m_filtered.append(app);
    }

    QVariantList list;
    for (AppItem *app : m_filtered) {
        list.append(QVariant::fromValue(static_cast<QObject*>(app)));
    }
    m_appsVariant = list;
    emit appsChanged();
}

void LauncherModel::reload()
{
    loadAllApps();
}

void LauncherModel::reloadProvider()
{
    if (m_provider.isEmpty())
        return;

    ++m_providerGeneration;
    const quint64 generation = m_providerGeneration;

    if (m_providerTimeout) {
        m_providerTimeout->stop();
        m_providerTimeout->deleteLater();
        m_providerTimeout = nullptr;
    }

    if (m_providerProcess) {
        QObject::disconnect(m_providerProcess, nullptr, this, nullptr);
        if (m_providerProcess->state() != QProcess::NotRunning)
            m_providerProcess->kill();
        m_providerProcess->deleteLater();
        m_providerProcess = nullptr;
    }

    m_providerOutput.clear();
    m_providerErrorOutput.clear();
    qDeleteAll(m_allApps);
    m_allApps.clear();
    filterApps(m_searchText);

    auto *process = new QProcess(this);
    m_providerProcess = process;
    process->setProcessChannelMode(QProcess::SeparateChannels);
    process->setWorkingDirectory(m_providerWorkingDirectory);

    connect(process, &QProcess::readyReadStandardOutput, this,
            [this, process, generation]() {
        if (generation != m_providerGeneration || process != m_providerProcess)
            return;

        m_providerOutput += process->readAllStandardOutput();
        constexpr qsizetype kMaxProviderOutput = 16 * 1024 * 1024;
        if (m_providerOutput.size() > kMaxProviderOutput) {
            qWarning() << "Provider output exceeded 16 MiB; terminating provider:" << m_provider;
            process->kill();
        }
    });

    connect(process, &QProcess::readyReadStandardError, this,
            [this, process, generation]() {
        if (generation != m_providerGeneration || process != m_providerProcess)
            return;

        constexpr qsizetype kMaxProviderError = 1024 * 1024;
        m_providerErrorOutput += process->readAllStandardError();
        if (m_providerErrorOutput.size() > kMaxProviderError)
            m_providerErrorOutput = m_providerErrorOutput.left(kMaxProviderError);
    });

    connect(process, &QProcess::errorOccurred, this,
            [this, process, generation](QProcess::ProcessError error) {
        if (generation != m_providerGeneration || process != m_providerProcess)
            return;

        qWarning() << "Provider process error:" << error
                   << process->errorString()
                   << "provider:" << m_provider;

        if (error == QProcess::FailedToStart) {
            if (m_providerTimeout) {
                m_providerTimeout->stop();
                m_providerTimeout->deleteLater();
                m_providerTimeout = nullptr;
            }

            process->deleteLater();
            m_providerProcess = nullptr;
            m_providerOutput.clear();
            m_providerErrorOutput.clear();
            qDeleteAll(m_allApps);
            m_allApps.clear();
            filterApps(m_searchText);
        }
    });

    connect(process, qOverload<int, QProcess::ExitStatus>(&QProcess::finished),
            this, &LauncherModel::onProviderFinished);

    process->start(m_provider, {QStringLiteral("list")});

    auto *timeout = new QTimer(this);
    m_providerTimeout = timeout;
    timeout->setSingleShot(true);
    timeout->setInterval(10000);

    connect(timeout, &QTimer::timeout, this,
            [this, process, generation]() {
        if (generation != m_providerGeneration || process != m_providerProcess)
            return;

        if (process->state() != QProcess::NotRunning) {
            qWarning() << "Provider timed out after 10 seconds; terminating:" << m_provider;
            process->kill();
        }
    });

    timeout->start();
}

void LauncherModel::onProviderFinished(int exitCode, QProcess::ExitStatus status)
{
    auto *process = qobject_cast<QProcess *>(sender());
    if (!process || process != m_providerProcess)
        return;

    if (m_providerTimeout)
        m_providerTimeout->stop();

    m_providerOutput += process->readAllStandardOutput();
    m_providerErrorOutput += process->readAllStandardError();
    const QByteArray errorOutput = m_providerErrorOutput;

    if (status != QProcess::NormalExit || exitCode != 0) {
        qWarning() << "Provider failed with code" << exitCode << "status" << status
                   << "provider:" << m_provider;
        if (!errorOutput.isEmpty())
            qWarning().noquote() << QString::fromLocal8Bit(errorOutput.left(4096));

        process->deleteLater();
        m_providerProcess = nullptr;
        m_providerOutput.clear();
        m_providerErrorOutput.clear();
        qDeleteAll(m_allApps);
        m_allApps.clear();
        filterApps(m_searchText);
        return;
    }

    qDeleteAll(m_allApps);
    m_allApps.clear();

    const QList<QByteArray> rawLines = m_providerOutput.split('\n');
    constexpr int kMaxProviderItems = 10000;
    int itemCount = 0;

    for (QByteArray rawLine : rawLines) {
        if (rawLine.endsWith('\r'))
            rawLine.chop(1);

        if (rawLine.isEmpty())
            continue;

        if (itemCount++ >= kMaxProviderItems) {
            qWarning() << "Provider returned more than 10000 items; remaining entries were ignored";
            break;
        }

        const QString line = QString::fromUtf8(rawLine);
        AppItem *item = Providers::fromLine(line, m_fields);
        if (item && !item->command().isEmpty())
            m_allApps.append(item);
        else
            delete item;
    }

    m_providerProcess->deleteLater();
    m_providerProcess = nullptr;
    filterApps(m_searchText);
}

void LauncherModel::search(const QString &text)
{
    m_searchText = text;
    filterApps(text);
}

void LauncherModel::launch(const QString &command)
{
    if (!m_provider.isEmpty()) {
        qint64 pid = 0;
        const bool started = QProcess::startDetached(
            m_provider,
            {QStringLiteral("run"), command},
            m_providerWorkingDirectory,
            &pid);

        if (!started) {
            qWarning() << "Failed to launch provider run:" << m_provider
                       << "working directory:" << m_providerWorkingDirectory;
        } else {
            qDebug() << "Provider run started:" << m_provider
                     << "pid:" << pid;
        }
        return;
    }

    m_history[command] = m_history.value(command, 0) + 1;
    History::save(m_history);

    auto cmd = ExecParser::buildCommand(command);
    if (!cmd.first.isEmpty()) {
        if (!QProcess::startDetached(cmd.first, cmd.second))
            qWarning() << "Failed to launch application:" << cmd.first;
    } else {
        qWarning() << "Invalid desktop Exec command:" << command;
    }
}
