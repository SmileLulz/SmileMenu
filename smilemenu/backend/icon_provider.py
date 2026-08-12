from pathlib import Path
from collections import OrderedDict
from PySide6.QtQuick import QQuickImageProvider
from PySide6.QtGui import QIcon, QPixmap
from PySide6.QtCore import QSize

class IconProvider(QQuickImageProvider):
    def __init__(self, max_cache_size=256):
        super().__init__(QQuickImageProvider.Image)
        self.max_cache_size = max_cache_size
        paths = QIcon.themeSearchPaths()
        user_icon_path = str(Path.home() / ".local/share/icons")
        if user_icon_path not in paths:
            paths.append(user_icon_path)
            QIcon.setThemeSearchPaths(paths)
        self._cache = OrderedDict()

    def _store_in_cache(self, key, img):
        if key in self._cache:
            self._cache.move_to_end(key)
        self._cache[key] = img
        if len(self._cache) > self.max_cache_size:
            self._cache.popitem(last=False)

    def requestImage(self, icon_name, size, requested_size):
        if not icon_name:
            return QIcon().pixmap(64, 64).toImage()

        key = (icon_name, 64, 64)
        if key in self._cache:
            self._cache.move_to_end(key)
            return self._cache[key]

        for ext in ("", ".png", ".svg", ".jpg", ".jpeg", ".xpm"):
            icon_path = Path.home() / ".local/share/icons" / f"{icon_name}{ext}"
            if icon_path.exists():
                pixmap = QPixmap(str(icon_path))
                if not pixmap.isNull():
                    img = pixmap.scaled(64, 64).toImage()
                    self._store_in_cache(key, img)
                    return img

        icon = QIcon.fromTheme(icon_name)
        if not icon.isNull():
            pixmap = icon.pixmap(QSize(64, 64))
            if not pixmap.isNull():
                img = pixmap.toImage()
                self._store_in_cache(key, img)
                return img

        fallback = QIcon.fromTheme("application-x-executable")
        pixmap = fallback.pixmap(64, 64) if not fallback.isNull() else QIcon().pixmap(64, 64)
        img = pixmap.toImage()
        self._store_in_cache(key, img)
        return img
