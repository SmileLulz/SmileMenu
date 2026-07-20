import subprocess
from pathlib import Path

from .item import LauncherItem


def load_stdin():
    import sys
    items = []

    for line in sys.stdin:
        line = line.strip()
        if not line:
            continue
        items.append(LauncherItem(name=line, command=line))

    return items


def load_script(script):
    items = []

    try:
        result = subprocess.run([script], capture_output=True, text=True)
    except Exception:
        return items

    for line in result.stdout.splitlines():
        line = line.strip()
        if not line:
            continue
        items.append(LauncherItem(name=line, command=line))

    return items


def load_provider(provider, fields=None):
    items = []

    try:
        result = subprocess.run([provider, "list"], capture_output=True, text=True)
    except Exception as e:
        print(f"Provider failed: {provider}")
        print(e)
        return items

    if result.returncode != 0:
        print(f"Provider error: {provider}")
        if result.stderr:
            print(result.stderr.strip())
        return items

    for line in result.stdout.splitlines():
        line = line.strip()
        if not line:
            continue
        items.append(provider_item(line, fields))

    return items


def provider_item(value, fields=None):
    path = Path(value)

    data = {
        "name": path.name,
        "command": value,
        "icon": "",
        "description": ""
    }

    if path.exists():
        data["icon"] = str(path)

    if fields:
        for field in fields:
            try:
                key, mode = field.split(":", 1)
            except ValueError:
                continue

            if mode == "path":
                if key == "name":
                    data[key] = path.name
                elif key == "icon":
                    data[key] = str(path)
            elif mode == "text":
                data[key] = value
            elif mode == "none":
                data[key] = ""

    return LauncherItem(
        name=data["name"],
        command=data["command"],
        icon=data["icon"],
        description=data["description"]
    )