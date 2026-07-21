import json
from pathlib import Path


CONFIG_FILE = Path.home() / ".config/smilemenu/config.json"


DEFAULT_CONFIG = {
    "history_limit": 3,
    "prompt_position": "entry",
    "window_width": 500,
    "min_visible_items": 1,
    "max_visible_items": 6,
    "fuzzy_search": True
}


def load_config():
    if not CONFIG_FILE.exists():
        return DEFAULT_CONFIG.copy()

    try:
        with CONFIG_FILE.open("r", encoding="utf-8") as file:
            data = json.load(file)

        config = DEFAULT_CONFIG.copy()
        config.update(data)
        return config

    except Exception:
        return DEFAULT_CONFIG.copy()


def save_config(config):
    CONFIG_FILE.parent.mkdir(parents=True, exist_ok=True)

    with CONFIG_FILE.open("w", encoding="utf-8") as file:
        json.dump(config, file, indent=4)