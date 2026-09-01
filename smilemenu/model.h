#pragma once
#include <QObject>
#include <QList>
#include <QString>
#include <QVariantMap>
#include <QProcess>
#include <QPointer>
#include <QTimer>
#include "appitem.h"

class LauncherModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString promptText READ promptText WRITE setPromptText NOTIFY promptTextChanged)
    Q_PROPERTY(QString promptPositionText READ promptPositionText WRITE setPromptPositionText NOTIFY promptPositionTextChanged)
    Q_PROPERTY(QString placeholder READ placeholder WRITE setPlaceholder NOTIFY placeholderChanged)
    Q_PROPERTY(int windowWidth READ windowWidth WRITE setWindowWidth NOTIFY windowWidthChanged)
    Q_PROPERTY(int maxVisibleItems READ maxVisibleItems WRITE setMaxVisibleItems NOTIFY maxVisibleItemsChanged)
    Q_PROPERTY(bool showTextField READ showTextField WRITE setShowTextField NOTIFY showTextFieldChanged)
    Q_PROPERTY(int historyLimit READ historyLimit CONSTANT)
    Q_PROPERTY(int minVisibleItems READ minVisibleItems CONSTANT)
    Q_PROPERTY(bool cycle READ cycle CONSTANT)
    Q_PROPERTY(QVariantList apps READ apps NOTIFY appsChanged)

public:
    explicit LauncherModel(const QVariantMap &config, QObject *parent = nullptr);
    ~LauncherModel();

    QString promptText() const;
    void setPromptText(const QString &text);

    QString promptPositionText() const;
    void setPromptPositionText(const QString &pos);

    QString placeholder() const;
    void setPlaceholder(const QString &text);

    int windowWidth() const;
    void setWindowWidth(int width);

    int maxVisibleItems() const;
    void setMaxVisibleItems(int items);

    bool showTextField() const;
    void setShowTextField(bool show);

    int historyLimit() const;
    int minVisibleItems() const;
    bool cycle() const;

    QVariantList apps() const;

    void setProvider(const QString &provider);
    void setProviderWorkingDirectory(const QString &directory);
    void setFields(const QStringList &fields);

    QString provider() const { return m_provider; }

public slots:
    void reload();
    void reloadProvider();
    void search(const QString &text);
    void launch(const QString &command);

signals:
    void promptTextChanged();
    void promptPositionTextChanged();
    void placeholderChanged();
    void windowWidthChanged();
    void maxVisibleItemsChanged();
    void showTextFieldChanged();
    void appsChanged();

private slots:
    void onProviderFinished(int exitCode, QProcess::ExitStatus status);

private:
    void loadAllApps();
    void filterApps(const QString &query);
    void applyHistory(QList<AppItem*> &apps);
    bool fuzzyMatch(const QString &query, const QString &text) const;
    bool matchApp(const QString &query, AppItem *app) const;

    QVariantMap m_config;
    QString m_prompt;
    QString m_promptPosition;
    QString m_placeholder;
    int m_windowWidth;
    int m_maxVisibleItems;
    bool m_showTextField;
    bool m_fuzzySearch;
    int m_historyLimit;

    QString m_provider;
    QString m_providerWorkingDirectory;
    QStringList m_fields;
    QPointer<QProcess> m_providerProcess;
    QPointer<QTimer> m_providerTimeout;
    QByteArray m_providerOutput;
    QByteArray m_providerErrorOutput;
    quint64 m_providerGeneration = 0;

    QList<AppItem*> m_allApps;
    QList<AppItem*> m_filtered;
    QVariantList m_appsVariant;

    QMap<QString, int> m_history;

    QString m_searchText;
};
