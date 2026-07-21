import json
from pathlib import Path


CONFIG_FILE = Path.home() / ".config/smilemenu/config.json"


DEFAULT_CONFIG = {
    "history_limit": 3,
    "prompt_position": "entry",
    "window_width": 500,
    "min_visible_items": 1,
    "max_visible_items": 6,
    "fuzzy_search": True,
    "show_text_field": True
}


def load_config(path=None):
    path = Path(path).expanduser() if path else CONFIG_FILE

    if not path.exists():
        return DEFAULT_CONFIG.copy()

    try:
        with path.open("r", encoding="utf-8") as file:
            data = json.load(file)

        config = DEFAULT_CONFIG.copy()
        config.update(data)
        return config

    except Exception:
        return DEFAULT_CONFIG.copy()


def save_config(config, path=None):
    path = Path(path).expanduser() if path else CONFIG_FILE

    path.parent.mkdir(parents=True, exist_ok=True)

    with path.open("w", encoding="utf-8") as file:
        json.dump(config, file, indent=4)