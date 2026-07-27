import subprocess
from pathlib import Path
from .item import LauncherItem


def load_stdin(display_columns=None):
    import sys
    items = []

    for line in sys.stdin:
        line = line.strip()
        if not line:
            continue
        items.append(LauncherItem(name=get_display_name(line, display_columns), command=line))

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

    columns = value.split("\t") if "\t" in value else [value]

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
            else:
                try:
                    col_index = int(mode) - 1
                    if 0 <= col_index < len(columns):
                        data[key] = columns[col_index]
                except ValueError:
                    pass

    return LauncherItem(
        name=data["name"],
        command=data["command"],
        icon=data["icon"],
        description=data["description"]
    )

def get_display_name(value, display_columns=None):
    if display_columns is None:
        return value

    columns = value.split("\t")

    selected = []

    for column in display_columns.split(","):
        try:
            index = int(column.strip()) - 1
            if 0 <= index < len(columns):
                selected.append(columns[index])
        except ValueError:
            continue

    return "\t".join(selected)