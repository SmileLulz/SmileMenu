from pathlib import Path

from .item import LauncherItem


class DesktopEntry:

    @classmethod
    def from_file(cls, path: Path):

        data = {}

        try:
            with path.open(
                "r",
                encoding="utf-8",
                errors="ignore"
            ) as file:

                in_entry = False

                for line in file:
                    line = line.strip()

                    if line == "[Desktop Entry]":
                        in_entry = True
                        continue

                    if line.startswith("["):
                        in_entry = False
                        continue

                    if not in_entry:
                        continue

                    if "=" not in line:
                        continue

                    key, value = line.split(
                        "=",
                        1
                    )

                    data[key] = value

        except Exception:
            return None

        if not data:
            return None

        if data.get(
            "Type",
            "Application"
        ) != "Application":
            return None

        if data.get(
            "Hidden",
            "false"
        ).lower() == "true":
            return None

        if data.get(
            "NoDisplay",
            "false"
        ).lower() == "true":
            return None

        name = data.get("Name")
        command = data.get("Exec")
        icon = data.get("Icon", "")
        
        categories_raw = data.get("Categories", "")
        categories = []
        if categories_raw:
            categories = [
                cat.strip() 
                for cat in categories_raw.split(";") 
                if cat.strip()
            ]

        if not name or not command:
            return None

        return LauncherItem(
            name=name,
            command=command,
            icon=icon,
            categories=categories
        )