from PySide6.QtCore import QObject, Property, Signal, Slot, QProcess
from .launcher import load_applications
from .history import load_history, save_history
from .providers import load_stdin, load_provider
from .exec_parser import build_command


def fuzzy_match(query, text):
    query = query.lower()
    text = text.lower()

    if not query:
        return True

    index = 0
    for char in text:
        if index < len(query) and char == query[index]:
            index += 1

    return index == len(query)


def match_app(query, app):
    if not query:
        return True
    
    if fuzzy_match(query, app["name"]):
        return True
    
    categories = app.get("categories", [])
    for category in categories:
        if fuzzy_match(query, category):
            return True
    
    return False


class LauncherModel(QObject):
    appsChanged = Signal()

    def __init__(
        self,
        prompt="",
        prompt_position="entry",
        provider=None,
        fields=None,
        dmenu_mode=False,
        display_columns=None,
        history_limit=3,
        config=None
    ):
        super().__init__()

        self.prompt = prompt
        self.prompt_position = prompt_position
        self.provider = provider
        self.fields = fields or []
        self.dmenu_mode = dmenu_mode
        self.display_columns = display_columns

        if config and "history_limit" in config:
            self._history_limit = config["history_limit"]
        else:
            self._history_limit = history_limit
        
        self.history = load_history()
        self.config = config or {}

        self._all_apps = []
        self._apps = []
        self._search_text = ""

        if provider:
            self.reload_provider()
        elif dmenu_mode:
            self.reload_stdin()
        else:
            self.reload()

    @Property("QVariantList", notify=appsChanged)
    def apps(self):
        return self._apps

    @Property(str, constant=True)
    def prompt_text(self):
        return self.prompt

    @Property(str, constant=True)
    def prompt_position_text(self):
        return self.prompt_position
    
    @Property(int, constant=True)
    def window_width(self):
        return self.config.get("window_width", 500)

    @Property(int, constant=True)
    def min_visible_items(self):
        return self.config.get("min_visible_items", 1)

    @Property(int, constant=True)
    def max_visible_items(self):
        return self.config.get("max_visible_items", 6)

    @Property(bool, constant=True)
    def show_text_field(self):
        return self.config.get("show_text_field", True)

    @Property(int, constant=True)
    def history_limit(self):
        return self._history_limit

    def apply_history(self, apps):
        frequent = []
        others = []

        for app in apps:
            score = self.history.get(app["command"], 0)
            if score > 0:
                frequent.append((score, app))
            else:
                others.append(app)

        frequent.sort(key=lambda x: x[0], reverse=True)
        top_frequent = [app for _, app in frequent[:self._history_limit]]
        remaining_frequent = [app for _, app in frequent[self._history_limit:]]
        all_others = sorted(remaining_frequent + others, key=lambda x: x["name"].lower())
        
        return top_frequent + all_others

    def _filter_apps(self):
        if self._search_text:
            self._apps = [
                app for app in self._all_apps
                if match_app(self._search_text, app)
            ]
        else:
            self._apps = self._all_apps.copy()
        
        self.appsChanged.emit()

    @Slot()
    def reload(self):
        self._all_apps = [
            {
                "name": app.name,
                "command": app.command,
                "icon": app.icon,
                "description": getattr(app, "description", ""),
                "categories": getattr(app, "categories", [])
            }
            for app in load_applications()
        ]

        self._all_apps.sort(key=lambda x: x["name"].lower())
        self._all_apps = self.apply_history(self._all_apps)
        self._filter_apps()

    @Slot()
    def reload_stdin(self):
        self._all_apps = [
            {
                "name": item.name,
                "command": item.command,
                "icon": item.icon,
                "description": item.description,
                "categories": []
            }
            for item in load_stdin(self.display_columns)
        ]

        self._apps = self._all_apps.copy()
        self.appsChanged.emit()

    @Slot()
    def reload_provider(self):
        self._all_apps = [
            {
                "name": item.name,
                "command": item.command,
                "icon": item.icon,
                "description": item.description,
                "categories": []
            }
            for item in load_provider(self.provider, self.fields)
        ]

        self._apps = self._all_apps.copy()
        self.appsChanged.emit()

    def update_history(self, command):
        self.history[command] = self.history.get(command, 0) + 1
        save_history(self.history)

    @Slot(str)
    def search(self, text):
        self._search_text = text
        self._filter_apps()

    @Slot(str)
    def launch(self, command):
        if self.provider:
            QProcess.startDetached(self.provider, ["run", command])
            return

        if self.dmenu_mode:
            print(command, flush=True)
            return

        self.update_history(command)

        program, args = build_command(command)

        if not program:
            return

        QProcess.startDetached(program, args)