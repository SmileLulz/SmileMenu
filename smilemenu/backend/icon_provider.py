from PySide6.QtQuick import QQuickImageProvider
from PySide6.QtGui import QIcon, QPixmap
from pathlib import Path


class IconProvider(QQuickImageProvider):

    def __init__(self):
        super().__init__(QQuickImageProvider.Image)
        paths = QIcon.themeSearchPaths()
        paths.append(str(Path.home() / ".local/share/icons"))
        QIcon.setThemeSearchPaths(paths)

    def requestImage(self, icon_name, size, requested_size):
        if not icon_name:
            return QIcon().pixmap(64, 64).toImage()
        
        for ext in ["", ".png", ".svg", ".jpg", ".jpeg", ".xpm"]:
            icon_path = Path.home() / ".local/share/icons" / f"{icon_name}{ext}"
            if icon_path.exists():
                pixmap = QPixmap(str(icon_path))
                if not pixmap.isNull():
                    return pixmap.scaled(64, 64).toImage()
        
        icon = QIcon.fromTheme(icon_name)
        if not icon.isNull():
            return icon.pixmap(64, 64).toImage()
        
        icon = QIcon.fromTheme("application-x-executable")
        if icon.isNull():
            return QIcon().pixmap(64, 64).toImage()
        return icon.pixmap(64, 64).toImage()