#include "model.h"
#include "launcher.h"
#include "history.h"
#include "execparser.h"
#include "providers.h"
#include <QProcess>
#include <QDebug>
#include <QTimer>
#include <QDir>
#include <QtGlobal>
#include <unistd.h>
#include <algorithm>

QList<AppItem*> loadApplications();

LauncherModel::LauncherModel(const QVariantMap &config, QObject *parent)
    : QObject(parent), m_config(config),
      m_prompt(""), m_promptPosition("entry"), m_placeholder("Search..."),
      m_windowWidth(config.value("window_width", 500).toInt()),
      m_maxVisibleItems(config.value("max_visible_items", 6).toInt()),
      m_showTextField(config.value("show_text_field", true).toBool()),
      m_fuzzySearch(config.value("fuzzy_search", true).toBool()),
      m_historyLimit(config.value("history_limit", 3).toInt()),
      m_providerProcess(nullptr)
{
    m_history = History::load();
    reload();
}

LauncherModel::~LauncherModel()
{
    qDeleteAll(m_allApps);
    if (m_providerProcess) {
        m_providerProcess->kill();
        m_providerProcess->deleteLater();
    }
}

QString LauncherModel::promptText() const { return m_prompt; }
void LauncherModel::setPromptText(const QString &text) {
    if (m_prompt != text) { m_prompt = text; emit promptTextChanged(); }
}
QString LauncherModel::promptPositionText() const { return m_promptPosition; }
void LauncherModel::setPromptPositionText(const QString &pos) {
    if (m_promptPosition != pos) { m_promptPosition = pos; emit promptPositionTextChanged(); }
}
QString LauncherModel::placeholder() const { return m_placeholder; }
void LauncherModel::setPlaceholder(const QString &text) {
    if (m_placeholder != text) { m_placeholder = text; emit placeholderChanged(); }
}
int LauncherModel::windowWidth() const { return m_windowWidth; }
void LauncherModel::setWindowWidth(int w) {
    if (m_windowWidth != w) { m_windowWidth = w; emit windowWidthChanged(); }
}
int LauncherModel::maxVisibleItems() const { return m_maxVisibleItems; }
void LauncherModel::setMaxVisibleItems(int items) {
    if (m_maxVisibleItems != items) { m_maxVisibleItems = items; emit maxVisibleItemsChanged(); }
}
bool LauncherModel::showTextField() const { return m_showTextField; }
void LauncherModel::setShowTextField(bool show) {
    if (m_showTextField != show) { m_showTextField = show; emit showTextFieldChanged(); }
}
int LauncherModel::historyLimit() const { return m_historyLimit; }
int LauncherModel::minVisibleItems() const { return m_config.value("min_visible_items", 1).toInt(); }

QVariantList LauncherModel::apps() const {
    return m_appsVariant;
}

void LauncherModel::setProvider(const QString &provider)
{
    m_provider = provider;
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
    if (!m_searchText.isEmpty())
        filterApps(m_searchText);
}

void LauncherModel::reloadProvider()
{
    if (m_provider.isEmpty())
        return;

    if (m_providerProcess) {
        m_providerProcess->kill();
        m_providerProcess->deleteLater();
        m_providerProcess = nullptr;
    }

    m_providerProcess = new QProcess(this);
    connect(m_providerProcess, &QProcess::finished, this, &LauncherModel::onProviderFinished);
    m_providerProcess->start(m_provider, QStringList() << "list");
}

void LauncherModel::onProviderFinished(int exitCode, QProcess::ExitStatus status)
{
    if (exitCode != 0) {
        qWarning() << "Provider failed with code" << exitCode;
        return;
    }

    QByteArray output = m_providerProcess->readAllStandardOutput();
    QStringList lines = QString::fromUtf8(output).split('\n', Qt::SkipEmptyParts);
    qDeleteAll(m_allApps);
    m_allApps.clear();

    for (const QString &line : lines) {
        AppItem *item = Providers::fromLine(line, m_fields);
        if (item)
            m_allApps.append(item);
    }
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
        QProcess::startDetached(m_provider, QStringList() << "run" << command);
        return;
    }

    m_history[command] = m_history.value(command, 0) + 1;
    History::save(m_history);

    auto cmd = ExecParser::buildCommand(command);
    if (!cmd.first.isEmpty()) {
        QProcess::startDetached(cmd.first, cmd.second);
    }
}
