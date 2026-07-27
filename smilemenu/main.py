#!/usr/bin/env python3

import sys
import os
import argparse
import json
import socket
from pathlib import Path

from PySide6.QtCore import QObject, Signal, Slot, QSocketNotifier
from PySide6.QtGui import QGuiApplication
from PySide6.QtQml import QQmlApplicationEngine
from PySide6.QtCore import QUrl

from .backend.config import load_config, DEFAULT_CONFIG, CONFIG_FILE
from .backend.theme import load_theme, THEME_FILE
from .backend.icon_provider import IconProvider
from .backend.model import LauncherModel
from .backend.lock import SingleInstanceLock

SOCKET_PATH = f"/tmp/smilemenu-{os.getuid()}.sock"


def try_connect_to_daemon():
    sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    try:
        sock.connect(SOCKET_PATH)
        return sock
    except (FileNotFoundError, ConnectionRefusedError, OSError):
        return None


def send_request(sock, request):
    sock.sendall((json.dumps(request) + "\n").encode("utf-8"))


def handle_daemon_request(sock, args):
    request = {
        "action": "show",
        "prompt": args.prompt,
        "prompt_position": args.prompt_position,
        "placeholder": args.placeholder,
        "provider": args.provider,
        "fields": args.field,
        "width": args.width,
        "max_items": args.max_items,
        "no_text_field": args.no_text_field,
    }
    send_request(sock, request)
    sock.close()
    return 0


def run_daemon(args, config, theme, lock):
    app = QGuiApplication(sys.argv)
    app.setApplicationName("smilemenu")
    app.setQuitOnLastWindowClosed(False)

    engine = QQmlApplicationEngine()
    engine.addImageProvider("icons", IconProvider())

    default_width = config.get("window_width", 500)
    default_max_items = config.get("max_visible_items", 6)
    default_show_text_field = config.get("show_text_field", True)

    launcher = LauncherModel(
        prompt="",
        prompt_position="entry",
        provider=None,
        fields=[],
        history_limit=config.get("history_limit", 3),
        config=config,
    )
    engine.rootContext().setContextProperty("launcher", launcher)
    engine.rootContext().setContextProperty("theme", theme)
    engine.rootContext().setContextProperty("preload_mode", True)

    qml_file = Path(__file__).parent / "qml" / "Main.qml"
    engine.load(QUrl.fromLocalFile(str(qml_file)))
    if not engine.rootObjects():
        return 1

    root = engine.rootObjects()[0]

    server_sock = None
    notifier = None

    def on_server_ready():
        client_sock, _ = server_sock.accept()
        data = b""
        while b"\n" not in data:
            chunk = client_sock.recv(4096)
            if not chunk:
                break
            data += chunk
        if not data:
            client_sock.close()
            return
        try:
            req = json.loads(data.decode("utf-8").strip())
        except json.JSONDecodeError:
            client_sock.close()
            return

        if req.get("action") != "show":
            client_sock.close()
            return

        # Reset to defaults
        launcher.setPrompt("")
        launcher.setPromptPosition("entry")
        launcher.setPlaceholder("Search...")
        launcher.setWindowWidth(default_width)
        launcher.setMaxVisibleItems(default_max_items)
        launcher.setShowTextField(default_show_text_field)
        launcher.setProvider(None)
        launcher.setFields([])

        # Apply user overrides
        launcher.setPrompt(req.get("prompt", ""))
        launcher.setPromptPosition(req.get("prompt_position", "entry"))
        launcher.setPlaceholder(req.get("placeholder", "Search..."))
        launcher.setProvider(req.get("provider"))
        launcher.setFields(req.get("fields", []))
        if req.get("width") is not None:
            launcher.setWindowWidth(req["width"])
        if req.get("max_items") is not None:
            launcher.setMaxVisibleItems(req["max_items"])
        if req.get("no_text_field") is not None:
            launcher.setShowTextField(not req["no_text_field"])

        launcher.search("")

        if req.get("provider"):
            launcher.reload_provider()
        else:
            launcher.reload()

        root.setProperty("visible", True)
        root.setProperty("closing", False)
        root.resetAnimation()
        root.show()
        root.raise_()
        root.requestActivate()

        client_sock.close()

    def start_accepting_requests():
        nonlocal server_sock, notifier
        if os.path.exists(SOCKET_PATH):
            os.unlink(SOCKET_PATH)

        server_sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        server_sock.bind(SOCKET_PATH)
        server_sock.listen(5)

        notifier = QSocketNotifier(server_sock.fileno(), QSocketNotifier.Read)
        notifier.activated.connect(on_server_ready)

    warmup_done = {"flag": False}

    def on_warmup_frame():
        if warmup_done["flag"]:
            return
        warmup_done["flag"] = True
        try:
            root.frameSwapped.disconnect(on_warmup_frame)
        except (TypeError, RuntimeError):
            pass
        root.setProperty("visible", False)
        start_accepting_requests()

    root.frameSwapped.connect(on_warmup_frame)
    root.setProperty("visible", True)
    root.show()

    def cleanup():
        if server_sock:
            server_sock.close()
        if os.path.exists(SOCKET_PATH):
            os.unlink(SOCKET_PATH)
        lock.release()

    app.aboutToQuit.connect(cleanup)

    return app.exec()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--daemon", action="store_true", help="Run the daemon")
    parser.add_argument("-p", "--prompt", default="", help="Set a custom prompt")
    parser.add_argument("-pp", "--prompt-position", choices=["top", "entry", "hidden"], default="entry", help="Set custom prompt position")
    parser.add_argument("-ph", "--placeholder", default="Search...", help="Set a custom text field placeholder text")
    parser.add_argument("--provider", help="Use a provider script")
    parser.add_argument("--field", action="append", default=[], help="Set fields as custom modes")
    parser.add_argument("-w", "--width", type=int, default=None, help="Set custom window width")
    parser.add_argument("-ntf", "--no-text-field", action="store_true", help="Hide the text field")
    parser.add_argument("-mi", "--max-items", type=int, default=None, help="Set custom max visible items (list height)")
    parser.add_argument("-gc", "--gen-config", action="store_true", help="Generate config file")
    parser.add_argument("-gt", "--gen-theme", action="store_true", help="Generate theme file")
    parser.add_argument("-c", "--config", help="Pass a custom config file")
    parser.add_argument("-t", "--theme", help="Pass a custom theme file")
    parser.add_argument("--dcache", choices=["app", "all"], help="Delete cache; either app cache or all cache")
    args = parser.parse_args()

    config_path = Path(args.config).expanduser().resolve() if args.config else CONFIG_FILE
    theme_path = Path(args.theme).expanduser().resolve() if args.theme else THEME_FILE

    if args.gen_config:
        if config_path.exists():
            print("Config already exists:", config_path)
            return 0
        from .backend.config import save_config, DEFAULT_CONFIG
        save_config(DEFAULT_CONFIG, config_path)
        print("Generated config:", config_path)
        return 0

    if args.gen_theme:
        if theme_path.exists():
            print("Theme already exists:", theme_path)
            return 0
        from .backend.theme import save_theme, DEFAULT_THEME
        save_theme(DEFAULT_THEME, theme_path)
        print("Generated theme:", theme_path)
        return 0
    
    if args.dcache:
        cache_dir = Path.home() / ".cache/smilemenu"
        if args.dcache == "app":
            cache_file = cache_dir / "apps_cache.json"
            if cache_file.exists():
                cache_file.unlink()
                print("Removed app cache:", cache_file)
            else:
                print("No app cache found")
        elif args.dcache == "all":
            if cache_dir.exists():
                import shutil
                shutil.rmtree(cache_dir)
                print("Removed entire cache directory:", cache_dir)
            else:
                print("No cache directory found")
        return 0

    if args.daemon:
        lock = SingleInstanceLock()
        if not lock.try_lock():
            print("SmileMenu daemon is already running")
            return 0
        config = load_config(config_path)
        theme = load_theme(theme_path)
        return run_daemon(args, config, theme, lock)

    sock = try_connect_to_daemon()
    if sock is None:
        print("Error: daemon not running. Start it with 'smilemenu --daemon'")
        return 1

    return handle_daemon_request(sock, args)


if __name__ == "__main__":
    sys.exit(main())