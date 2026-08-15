#pragma once
#include <QObject>
#include <QString>
#include <QStringList>

class AppItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString command READ command WRITE setCommand NOTIFY commandChanged)
    Q_PROPERTY(QString icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QStringList categories READ categories WRITE setCategories NOTIFY categoriesChanged)
public:
    explicit AppItem(QObject *parent = nullptr);
    AppItem(const QString &name, const QString &command, const QString &icon = QString(),
            const QString &description = QString(), const QStringList &categories = QStringList(), QObject *parent = nullptr);

    QString name() const;
    void setName(const QString &name);

    QString command() const;
    void setCommand(const QString &command);

    QString icon() const;
    void setIcon(const QString &icon);

    QString description() const;
    void setDescription(const QString &description);

    QStringList categories() const;
    void setCategories(const QStringList &categories);

signals:
    void nameChanged();
    void commandChanged();
    void iconChanged();
    void descriptionChanged();
    void categoriesChanged();

private:
    QString m_name;
    QString m_command;
    QString m_icon;
    QString m_description;
    QStringList m_categories;
};
