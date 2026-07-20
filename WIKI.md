# ⭐ Page Contents

- [Usage](#usage)
	- [-h, --help](#h-help)
	- [-p, --prompt](#p-prompt-prompt)
	- [-pp, --prompt-position](#pp-prompt-position-top-entry-hidden)
	- [-ph, --placeholder](#ph-placeholder-placeholder)
	- [-s, --script](#s-script)
	- [--provider](#provider)
	- [--field](#field)
	- [-d, --dmenu](#d-dmenu)
	- [-w, --width](#w-width)
	- [-gc, --gen-config](#gc-gen-config)
	- [-gt, --gen-theme](#gt-gen-theme)
- [Keyboard Controls](#keyboard-controls)


---

# ⭐ Usage

### 🧩 Normal application launcher

```sh
smilemenu
```


### 🧩 `-h, --help`

Shows help message.


### 🧩 `-p, --prompt <PROMPT>`

Show a prompt text next to or above the search field.

Example:

```sh
smilemenu -p "Apps"
```


### 🧩 `-pp, --prompt-position {top,entry,hidden}`

Positions the prompt text. Entry by default.

Examples:

```sh
smilemenu -p "Apps" --prompt-position top

smilemenu -p "Apps" --prompt-position entry
```


### 🧩 `-ph, --placeholder <PLACEHOLDER>`

Overrides the default placeholder text.

Example:

```sh
smilemenu -ph "Search Apps..."
```


### 🧩 `-s, --script <SCRIPT>`

Scripts can provide custom entries. Run a script and use its output as items. The script should output one item per line.

Example:

```sh
smilemenu --script ~/.local/bin/my-menu.sh
```

Script output:

```text
Option 1
Option 2
Option 3
```

Selecting an item runs:

```sh
my-menu.sh "Option 1"
```


### 🧩 `--provider <SCRIPT>`

Providers are more advanced scripts.

A provider has two actions:

```
list
run
```

**Example:**

```sh
#!/bin/bash

case "$1" in

list)
    echo "Firefox"
    echo "Steam"
    echo "Vesktop"
;;

run)
    case "$2" in

    Firefox)
        firefox
    ;;
    
    Steam)
        steam
    ;;
    
    Vesktop)
        vesktop
    ;;
    
    esac
;;

esac
```

Run:

```sh
smilemenu --provider ~/.local/bin/apps-provider.sh
```


**Wallpaper menu example:**

```sh
#!/bin/bash

WALLPAPER_DIR="$HOME/.wallpapers"

case "$1" in

list)
    find "$WALLPAPER_DIR" \
        -maxdepth 1 \
        -type f \
        \( -iname "*.png" -o -iname "*.jpg" \) \
        -printf "%f\n"
;;

run)
    awww img "$WALLPAPER_DIR/$2"
;;

esac
```

Run:

```sh
smilemenu --provider ~/.local/bin/wallpaper-provider.sh -p "Wallpaper"
```


### 🧩 `--field <FILELD>:<MODE>`

The `--field` option customizes how SmileMenu interprets the output of a provider.

Fields:

| Field         | Description                         |
| ------------- | ----------------------------------- |
| `name`        | Display name of the item            |
| `command`     | Command to execute when selected    |
| `icon`        | Icon path or name                   |
| `description` | Secondary text shown below the name |

Modes:

| Mode   | Description                               | Example                    |
| ------ | ----------------------------------------- | -------------------------- |
| `path` | Uses the file/directory path as the value | `--field icon:path`        |
| `text` | Uses the raw text/value as-is             | `--field name:text`        |
| `none` | Sets the field to empty string            | `--field description:none` |

Example:

```sh
# Name = filename from path
# Icon = full path
# Description = raw text

smilemenu --provider ~/.local/bin/wallpaper-provider.sh --field name:path --field icon:path --field description:text
```


### 🧩 `-d, --dmenu`

Reads a list of items from stdin and prints the selected item to stdout.

Examples:

```sh
echo -e "Firefox\nChrome\nBrave" | smilemenu --dmenu

cat ~/my-list.txt | smilemenu --dmenu

ls ~/Documents | smilemenu --dmenu
```


### 🧩 `-w, --width <SIZE>`

Overrides the window width.

Example:

```sh
smilemenu -w 700
```


### 🧩 `-gc, --gen-config`

Generates default config file (`~/.config/smilemenu/config.json`)


### 🧩 `-gc, --gen-theme`

Generates default theme file (`~/.config/smilemenu/theme.json`)


---

## Keyboard Controls

| Key    | Action        |
| ------ | ------------- |
| Up     | Previous item |
| Down   | Next item     |
| Enter  | Launch        |
| Escape | Close         |