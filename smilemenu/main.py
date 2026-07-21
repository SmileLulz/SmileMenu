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
from .backend.lock import SingleInstanceLock


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument("-p", "--prompt", default="")
    parser.add_argument("-pp", "--prompt-position", choices=["top", "entry", "hidden"], default="entry")
    parser.add_argument("-ph", "--placeholder", default="Search...")
    parser.add_argument("--provider")
    parser.add_argument("--field", action="append", default=[])
    parser.add_argument("-d", "--dmenu", action="store_true")
    parser.add_argument("-dc", "--display-columns", type=str, default=None)
    parser.add_argument("-w", "--width", type=int, default=None)
    parser.add_argument("-ntf", "--no-text-field", action="store_true")
    parser.add_argument("-gc", "--gen-config", action="store_true")
    parser.add_argument("-gt", "--gen-theme", action="store_true")
    parser.add_argument("-c", "--config")
    parser.add_argument("-t", "--theme")

    args = parser.parse_args()

    config_path = (
        Path(args.config).expanduser().resolve()
        if args.config else CONFIG_FILE
    )

    theme_path = (
        Path(args.theme).expanduser().resolve()
        if args.theme else THEME_FILE
    )

    if args.gen_config:
        if config_path.exists():
            print("Config already exists:", CONFIG_FILE)
            return 0

        save_config(DEFAULT_CONFIG, config_path)
        print("Generated config:", config_path)
        return 0
    
    if args.gen_theme:
        if theme_path.exists():
            print("Theme already exists:", THEME_FILE)
            return 0

        save_theme(DEFAULT_THEME, theme_path)
        print("Generated theme:", theme_path)
        return 0

    lock = SingleInstanceLock()
    if not lock.try_lock():
        print("SmileMenu is already running")
        return 0

    config = load_config(config_path)
    
    if args.width is not None:
        config["window_width"] = args.width
    
    if args.no_text_field:
        config["show_text_field"] = False

    theme = load_theme(theme_path)

    app = QGuiApplication(sys.argv)
    app.setApplicationName("smilemenu")

    app.aboutToQuit.connect(lock.release)

    engine = QQmlApplicationEngine()
    engine.addImageProvider("icons", IconProvider())
    engine.rootContext().setContextProperty("theme", theme)

    launcher = LauncherModel(
        prompt=args.prompt,
        prompt_position=args.prompt_position,
        provider=args.provider,
        fields=args.field,
        dmenu_mode=args.dmenu,
        display_columns=args.display_columns,
        history_limit=config["history_limit"],
        config=config
    )

    engine.rootContext().setContextProperty("launcher", launcher)
    engine.rootContext().setContextProperty("customPlaceholder", args.placeholder)

    qml_file = Path(__file__).parent / "qml" / "Main.qml"
    engine.load(QUrl.fromLocalFile(str(qml_file)))

    if not engine.rootObjects():
        lock.release()
        return 1

    return app.exec()


if __name__ == "__main__":
    sys.exit(main())