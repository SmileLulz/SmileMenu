#!/usr/bin/env python3

import sys
import argparse
from pathlib import Path

from PySide6.QtGui import QGuiApplication
from PySide6.QtQml import QQmlApplicationEngine
from PySide6.QtCore import QUrl

from .backend.config import (load_config, save_config, DEFAULT_CONFIG, CONFIG_FILE)
from .backend.theme import (load_theme, save_theme, DEFAULT_THEME, THEME_FILE)
from .backend.icon_provider import IconProvider
from .backend.model import LauncherModel


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("-s", "--script")
    parser.add_argument("-p", "--prompt", default="")
    parser.add_argument("-pp", "--prompt-position", choices=["top", "entry", "hidden"], default="entry")
    parser.add_argument("-ph", "--placeholder", default="Search...")
    parser.add_argument("--provider")
    parser.add_argument("--field", action="append", default=[])
    parser.add_argument("-d", "--dmenu", action="store_true")
    parser.add_argument("-w", "--width", type=int, default=None)
    parser.add_argument("-gc", "--gen-config", action="store_true")
    parser.add_argument("-gt", "--gen-theme", action="store_true")

    args = parser.parse_args()

    if args.gen_config:
        if CONFIG_FILE.exists():
            print("Config already exists:", CONFIG_FILE)
            return 0

        save_config(DEFAULT_CONFIG)
        print("Generated config:", CONFIG_FILE)
        return 0
    
    if args.gen_theme:
        if THEME_FILE.exists():
            print("Theme already exists:", THEME_FILE)
            return 0

        save_theme(DEFAULT_THEME)
        print("Generated theme:", THEME_FILE)
        return 0

    config = load_config()
    
    if args.width is not None:
        config["window_width"] = args.width

    theme = load_theme()

    app = QGuiApplication(sys.argv)
    app.setApplicationName("smilemenu")

    engine = QQmlApplicationEngine()
    engine.addImageProvider("icons", IconProvider())
    engine.rootContext().setContextProperty("theme", theme)

    launcher = LauncherModel(
        dmenu_mode=args.dmenu,
        script=args.script,
        provider=args.provider,
        fields=args.field,
        prompt=args.prompt,
        prompt_position=args.prompt_position,
        history_limit=config["history_limit"],
        config=config
    )

    engine.rootContext().setContextProperty("launcher", launcher)
    engine.rootContext().setContextProperty("customPlaceholder", args.placeholder)

    qml_file = Path(__file__).parent / "qml" / "Main.qml"
    engine.load(QUrl.fromLocalFile(str(qml_file)))

    if not engine.rootObjects():
        return 1

    return app.exec()


if __name__ == "__main__":
    sys.exit(main())