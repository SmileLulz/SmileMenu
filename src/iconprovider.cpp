#include "iconprovider.h"
#include <QIcon>
#include <QPixmap>
#include <QImage>
#include <QDir>
#include <QDebug>

IconProvider::IconProvider(int maxCacheSize)
    : QQuickImageProvider(QQuickImageProvider::Image), m_maxCacheSize(maxCacheSize)
{
    QStringList paths = QIcon::themeSearchPaths();
    QString userPath = QDir::homePath() + "/.local/share/icons";
    if (!paths.contains(userPath))
        paths.append(userPath);
    QIcon::setThemeSearchPaths(paths);
}

QImage IconProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    if (id.isEmpty()) {
        QIcon fallback = QIcon::fromTheme("application-x-executable");
        QPixmap pix = fallback.pixmap(64, 64);
        if (pix.isNull())
            pix = QPixmap(64, 64);
        return pix.toImage();
    }

    QString key = id;
    if (m_cache.contains(key)) {
        QImage *cached = m_cache[key];
        if (cached)
            return *cached;
    }

    QPixmap pix;
    QStringList extensions = {"", ".png", ".svg", ".jpg", ".jpeg", ".xpm"};
    for (const QString &ext : extensions) {
        QString path = QDir::homePath() + "/.local/share/icons/" + id + ext;
        if (QFile::exists(path)) {
            pix.load(path);
            if (!pix.isNull())
                break;
        }
    }

    if (pix.isNull()) {
        QIcon icon = QIcon::fromTheme(id);
        if (!icon.isNull())
            pix = icon.pixmap(64, 64);
    }

    if (pix.isNull()) {
        QIcon fallback = QIcon::fromTheme("application-x-executable");
        pix = fallback.pixmap(64, 64);
        if (pix.isNull())
            pix = QPixmap(64, 64);
    }

    QImage img = pix.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation).toImage();
    m_cache.insert(key, new QImage(img));
    return img;
}
