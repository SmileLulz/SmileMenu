from pathlib import Path
from .desktop_entry import DesktopEntry
from .desktop_cache import load_cache, save_cache

DIRECTORIES = [
    Path.home() / ".local/share/applications",
    Path("/usr/local/share/applications"),
    Path("/usr/share/applications"),
    Path("/var/lib/flatpak/exports/share/applications"),
]

def load_applications():
    cached, valid = load_cache(DIRECTORIES)
    if valid:
        return cached

    apps = {}
    for directory in DIRECTORIES:
        if not directory.exists():
            continue
        for desktop in directory.glob("*.desktop"):
            name = desktop.name
            if name in apps:
                continue
            item = DesktopEntry.from_file(desktop)
            if item:
                apps[name] = item

    all_items = list(apps.values())
    try:
        save_cache(DIRECTORIES, all_items)
    except Exception:
        pass
    return all_items