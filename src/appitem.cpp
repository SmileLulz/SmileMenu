#include "appitem.h"

AppItem::AppItem(QObject *parent) : QObject(parent) {}

AppItem::AppItem(const QString &name, const QString &command, const QString &icon,
                 const QString &description, const QStringList &categories, QObject *parent)
    : QObject(parent), m_name(name), m_command(command), m_icon(icon),
      m_description(description), m_categories(categories) {}

QString AppItem::name() const { return m_name; }
void AppItem::setName(const QString &name) { if (m_name != name) { m_name = name; emit nameChanged(); } }

QString AppItem::command() const { return m_command; }
void AppItem::setCommand(const QString &cmd) { if (m_command != cmd) { m_command = cmd; emit commandChanged(); } }

QString AppItem::icon() const { return m_icon; }
void AppItem::setIcon(const QString &icon) { if (m_icon != icon) { m_icon = icon; emit iconChanged(); } }

QString AppItem::description() const { return m_description; }
void AppItem::setDescription(const QString &desc) { if (m_description != desc) { m_description = desc; emit descriptionChanged(); } }

QStringList AppItem::categories() const { return m_categories; }
void AppItem::setCategories(const QStringList &cats) { if (m_categories != cats) { m_categories = cats; emit categoriesChanged(); } }
