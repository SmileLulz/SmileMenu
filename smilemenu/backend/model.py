from PySide6.QtCore import QObject, Property, Signal, Slot, QProcess
from .launcher import load_applications
from .history import load_history, save_history
from .providers import provider_item
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


def match_app(query, app, config=None):
    if not query:
        return True
    use_fuzzy = True
    if config and isinstance(config, dict):
        use_fuzzy = config.get("fuzzy_search", True)

    if use_fuzzy:
        if fuzzy_match(query, app["name"]):
            return True
        for category in app.get("categories", []):
            if fuzzy_match(query, category):
                return True
    else:
        query_lower = query.lower()
        if query_lower in app["name"].lower():
            return True
        for category in app.get("categories", []):
            if query_lower in category.lower():
                return True
    return False


class LauncherModel(QObject):
    appsChanged = Signal()
    promptChanged = Signal()
    promptPositionChanged = Signal()
    placeholderChanged = Signal()
    windowWidthChanged = Signal()
    maxVisibleItemsChanged = Signal()
    showTextFieldChanged = Signal()

    def __init__(self, prompt="", prompt_position="entry", provider=None,
                 fields=None, history_limit=3, config=None):
        super().__init__()
        self._prompt = prompt
        self._prompt_position = prompt_position
        self._provider = provider
        self._fields = fields or []
        self.config = config or {}
        self._history_limit = self.config.get("history_limit", history_limit)
        self.history = load_history()

        self._all_apps = []
        self._apps = []
        self._search_text = ""
        self._placeholder = "Search..."
        self._window_width = self.config.get("window_width", 500)
        self._show_text_field = self.config.get("show_text_field", True)

        self._provider_process = QProcess(self)
        self._provider_process.finished.connect(self._on_provider_finished)

        if provider:
            self.reload_provider()
        else:
            self.reload()

    @Property(str, notify=promptChanged)
    def prompt_text(self):
        return self._prompt

    @Property(str, notify=promptPositionChanged)
    def prompt_position_text(self):
        return self._prompt_position

    @Property(str, notify=placeholderChanged)
    def placeholder(self):
        return self._placeholder

    @Property(int, notify=windowWidthChanged)
    def window_width(self):
        return self._window_width

    @Property(bool, notify=showTextFieldChanged)
    def show_text_field(self):
        return self._show_text_field

    @Property(int, constant=True)
    def history_limit(self):
        return self._history_limit

    @Property(int, constant=True)
    def min_visible_items(self):
        return self.config.get("min_visible_items", 1)

    @Property(int, notify=maxVisibleItemsChanged)
    def max_visible_items(self):
        return self.config.get("max_visible_items", 6)

    @Property("QVariantList", notify=appsChanged)
    def apps(self):
        return self._apps

    def setPrompt(self, text):
        if self._prompt != text:
            self._prompt = text
            self.promptChanged.emit()

    def setPromptPosition(self, pos):
        if self._prompt_position != pos:
            self._prompt_position = pos
            self.promptPositionChanged.emit()

    def setPlaceholder(self, text):
        if self._placeholder != text:
            self._placeholder = text
            self.placeholderChanged.emit()

    def setWindowWidth(self, width):
        if self._window_width != width:
            self._window_width = width
            self.windowWidthChanged.emit()

    def setMaxVisibleItems(self, value):
        if self.config.get("max_visible_items") != value:
            self.config["max_visible_items"] = value
            self.maxVisibleItemsChanged.emit()

    def setShowTextField(self, show):
        if self._show_text_field != show:
            self._show_text_field = show
            self.showTextFieldChanged.emit()

    def setProvider(self, provider):
        self._provider = provider

    def setFields(self, fields):
        self._fields = fields

    def apply_history(self, apps):
        frequent, others = [], []
        for app in apps:
            score = self.history.get(app["command"], 0)
            if score > 0:
                frequent.append((score, app))
            else:
                others.append(app)
        frequent.sort(key=lambda x: x[0], reverse=True)
        top = [app for _, app in frequent[:self._history_limit]]
        rest = [app for _, app in frequent[self._history_limit:]]
        rest_sorted = sorted(rest + others, key=lambda x: x["name"].lower())
        return top + rest_sorted

    def _filter_apps(self):
        if self._search_text:
            self._apps = [app for app in self._all_apps if match_app(self._search_text, app, self.config)]
        else:
            self._apps = self._all_apps.copy()
        self.appsChanged.emit()

    @Slot()
    def reload(self):
        apps = load_applications()
        self._all_apps = [
            {
                "name": a.name,
                "command": a.command,
                "icon": a.icon,
                "description": getattr(a, "description", ""),
                "categories": getattr(a, "categories", []),
            }
            for a in apps
        ]
        self._all_apps.sort(key=lambda x: x["name"].lower())
        self._all_apps = self.apply_history(self._all_apps)
        self._filter_apps()

    @Slot()
    def reload_provider(self):
        if not self._provider:
            return

        if self._provider_process.state() != QProcess.NotRunning:
            self._provider_process.kill()
            self._provider_process.waitForFinished(100)

        self._provider_process.start(self._provider, ["list"])

    def _on_provider_finished(self, exit_code, exit_status):
        if exit_code != 0:
            return

        output = self._provider_process.readAllStandardOutput().data().decode("utf-8")
        self._all_apps = []

        for line in output.splitlines():
            line = line.strip()
            if not line:
                continue

            item = provider_item(line, self._fields)
            self._all_apps.append({
                "name": item.name,
                "command": item.command,
                "icon": item.icon,
                "description": item.description,
                "categories": [],
            })

        self._filter_apps()

    def update_history(self, command):
        self.history[command] = self.history.get(command, 0) + 1
        save_history(self.history)

    @Slot(str)
    def search(self, text):
        self._search_text = text
        self._filter_apps()

    @Slot(str)
    def launch(self, command):
        if self._provider:
            QProcess.startDetached(self._provider, ["run", command])
            return
        self.update_history(command)
        program, args = build_command(command)
        if program:
            QProcess.startDetached(program, args)
