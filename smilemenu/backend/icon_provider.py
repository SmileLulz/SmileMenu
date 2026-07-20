from PySide6.QtQuick import QQuickImageProvider
from PySide6.QtGui import QIcon


class IconProvider(QQuickImageProvider):

    def __init__(self):
        super().__init__(
            QQuickImageProvider.Image
        )

    def requestImage(
        self,
        icon_name,
        size,
        requested_size
    ):
        icon = QIcon.fromTheme(icon_name)

        if icon.isNull():
            return self.fallback()

        return icon.pixmap(
            64,
            64
        ).toImage()


    def fallback(self):
        icon = QIcon.fromTheme(
            "application-x-executable"
        )

        return icon.pixmap(
            64,
            64
        ).toImage()