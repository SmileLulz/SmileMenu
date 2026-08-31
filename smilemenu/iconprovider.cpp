#include "iconprovider.h"
#include <QIcon>
#include <QPixmap>
#include <QImage>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

IconProvider::IconProvider(int maxCacheSize)
    : QQuickImageProvider(QQuickImageProvider::Image),
      m_maxCacheSize(qMax(1, maxCacheSize))
{
    m_cache.setMaxCost(m_maxCacheSize);

    QStringList paths = QIcon::themeSearchPaths();
    const QString userPath = QDir::homePath() + "/.local/share/icons";
    if (!paths.contains(userPath))
        paths.append(userPath);
    QIcon::setThemeSearchPaths(paths);
}

QImage IconProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(requestedSize);

    const QSize fallbackSize(64, 64);
    auto fallback = [&]() {
        QIcon fallbackIcon = QIcon::fromTheme("application-x-executable");
        QPixmap pix = fallbackIcon.pixmap(fallbackSize);
        if (pix.isNull())
            pix = QPixmap(fallbackSize);
        return pix.toImage();
    };

    if (id.isEmpty()) {
        const QImage image = fallback();
        if (size)
            *size = image.size();
        return image;
    }

    const QString key = id;
    if (QImage *cached = m_cache.object(key)) {
        if (size)
            *size = cached->size();
        return *cached;
    }

    QPixmap pix;
    const QFileInfo idInfo(id);

    if (idInfo.isAbsolute() && idInfo.isFile())
        pix.load(idInfo.absoluteFilePath());

    if (pix.isNull()) {
        const QStringList extensions = {"", ".png", ".svg", ".jpg", ".jpeg", ".xpm"};
        for (const QString &ext : extensions) {
            const QString path = QDir::homePath() + "/.local/share/icons/" + id + ext;
            if (QFileInfo::exists(path) && pix.load(path))
                break;
        }
    }

    if (pix.isNull()) {
        const QIcon icon = QIcon::fromTheme(id);
        if (!icon.isNull())
            pix = icon.pixmap(fallbackSize);
    }

    if (pix.isNull())
        return fallback();

    const QImage image = pix.scaled(fallbackSize, Qt::KeepAspectRatio, Qt::SmoothTransformation).toImage();
    m_cache.insert(key, new QImage(image), 1);
    if (size)
        *size = image.size();
    return image;
}
