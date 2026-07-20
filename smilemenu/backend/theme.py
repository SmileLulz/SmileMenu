import json
from pathlib import Path


THEME_FILE = Path.home() / ".config/smilemenu/theme.json"


DEFAULT_THEME = {
    "window_radius": 40,
    "background_color": "#303446",

    "item_spacing": 0,
    "item_radius_high": 28,
    "item_radius_low": 0,
    "item_color": "transparent",
    "item_hover_color": "#626880",
    "item_container_color": "#414559",
    "item_height": 50,
    "item_height_description": 65,

    "content_margins": 28,
    "content_spacing": 10,

    "text_color": "#c6d0f5",
    "text_color_secondary": "#949cbb",
    "font_size": 16,
    "font_size_secondary": 12,
    "font_bold": False,

    "text_field_placeholder_color": "#737994",
    "text_field_color": "transparent",
    "text_field_border_color": "transparent",
    "text_field_border_width": 0,
    "text_field_radius": 0
}


def load_theme():
    if not THEME_FILE.exists():
        return DEFAULT_THEME.copy()

    try:
        with THEME_FILE.open("r", encoding="utf-8") as file:
            data = json.load(file)

        theme = DEFAULT_THEME.copy()
        theme.update(data)
        return theme

    except Exception:
        return DEFAULT_THEME.copy()


def save_theme(theme):
    THEME_FILE.parent.mkdir(parents=True, exist_ok=True)

    with THEME_FILE.open("w", encoding="utf-8") as file:
        json.dump(theme, file, indent=4)