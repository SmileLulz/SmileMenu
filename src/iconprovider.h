#pragma once
#include <QQuickImageProvider>
#include <QCache>
#include <QString>

class IconProvider : public QQuickImageProvider
{
public:
    explicit IconProvider(int maxCacheSize = 256);

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

private:
    QCache<QString, QImage> m_cache;
    int m_maxCacheSize;
};
