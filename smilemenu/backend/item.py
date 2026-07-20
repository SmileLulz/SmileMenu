class LauncherItem:

    def __init__(
        self,
        name,
        command,
        icon="",
        description="",
        categories=None
    ):
        self.name = name
        self.command = command
        self.icon = icon
        self.description = description
        self.categories = categories if categories is not None else []