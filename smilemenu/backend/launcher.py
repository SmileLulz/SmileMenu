from pathlib import Path

from .desktop_entry import DesktopEntry


def parse_desktop(path):
    return DesktopEntry.from_file(path)


def load_applications():
    paths = [
        Path.home() / ".local/share/applications",
        Path("/usr/local/share/applications"),
        Path("/usr/share/applications"),
    ]

    apps = {}

    for directory in paths:
        if not directory.exists():
            continue

        for desktop in directory.glob("*.desktop"):
            item = parse_desktop(desktop)

            if not item:
                continue

            desktop_id = desktop.name
            apps[desktop_id] = item

    return list(apps.values())