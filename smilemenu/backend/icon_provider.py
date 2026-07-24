from PySide6.QtQuick import QQuickImageProvider
from PySide6.QtGui import QIcon, QPixmap
from PySide6.QtCore import QSize
from pathlib import Path

class IconProvider(QQuickImageProvider):
    def __init__(self):
        super().__init__(QQuickImageProvider.Image)
        paths = QIcon.themeSearchPaths()
        paths.append(str(Path.home() / ".local/share/icons"))
        QIcon.setThemeSearchPaths(paths)
        self._cache = {}

    def requestImage(self, icon_name, size, requested_size):
        if not icon_name:
            return QIcon().pixmap(64, 64).toImage()

        key = (icon_name, 64, 64)
        if key in self._cache:
            return self._cache[key]

        for ext in ("", ".png", ".svg", ".jpg", ".jpeg", ".xpm"):
            icon_path = Path.home() / ".local/share/icons" / f"{icon_name}{ext}"
            if icon_path.exists():
                pixmap = QPixmap(str(icon_path))
                if not pixmap.isNull():
                    img = pixmap.scaled(64, 64).toImage()
                    self._cache[key] = img
                    return img

        icon = QIcon.fromTheme(icon_name)
        if not icon.isNull():
            pixmap = icon.pixmap(QSize(64, 64))
            if not pixmap.isNull():
                img = pixmap.toImage()
                self._cache[key] = img
                return img

        fallback = QIcon.fromTheme("application-x-executable")
        if fallback.isNull():
            fallback = QIcon()
        pixmap = fallback.pixmap(64, 64)
        img = pixmap.toImage()
        self._cache[key] = img
        return img