import json
from pathlib import Path
from .item import LauncherItem

CACHE_FILE = Path.home() / ".cache/smilemenu/apps_cache.json"
CACHE_VERSION = 1

def _dirs_mtime(dirs):
    return {str(d): d.stat().st_mtime for d in dirs if d.exists()}

def load_cache(directories):
    if not CACHE_FILE.exists():
        return None, False

    try:
        data = json.loads(CACHE_FILE.read_text())
        if data.get("version") != CACHE_VERSION:
            return None, False

        current_mtimes = _dirs_mtime(directories)
        if current_mtimes != data.get("directories", {}):
            return None, False

        items = [
            LauncherItem(
                name=i["name"],
                command=i["command"],
                icon=i.get("icon", ""),
                description=i.get("description", ""),
                categories=i.get("categories", [])
            )
            for i in data["items"]
        ]
        return items, True
    except Exception:
        return None, False

def save_cache(directories, items):
    data = {
        "version": CACHE_VERSION,
        "directories": _dirs_mtime(directories),
        "items": [
            {
                "name": item.name,
                "command": item.command,
                "icon": item.icon,
                "description": item.description,
                "categories": item.categories,
            }
            for item in items
        ],
    }
    CACHE_FILE.parent.mkdir(parents=True, exist_ok=True)
    CACHE_FILE.write_text(json.dumps(data, indent=2))