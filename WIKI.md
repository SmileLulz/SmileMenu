# ⭐ Page Contents

- [Usage](#usage)
	- [-h, --help](#h-help)
	- [-p, --prompt](#p-prompt-prompt)
	- [-pp, --prompt-position](#pp-prompt-position-top-entry-hidden)
	- [-ph, --placeholder](#ph-placeholder-placeholder)
	- [--provider](#provider-script)
	- [--field](#field-field-mode)
	- [-d, --dmenu](#d-dmenu)
	- [-dc, --display-columns](#d-display-columns-column)
	- [-w, --width](#w-width-width)
	- [-ntf, --no-text-field](#ntf-no-text-field)
	- [-gc, --gen-config](#gc-gen-config)
	- [-gt, --gen-theme](#gt-gen-theme)
	- [-c, --config](#c-config-path)
	- [-t, --theme](#t-theme-path)
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


### 🧩 `--provider <SCRIPT>`

Providers are more advanced scripts.

A provider has two actions:

```
list
run
```

**Example:**

```sh
#!/usr/bin/env bash

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
#!/usr/bin/env bash

WALLPAPER_DIR="$HOME/.wallpapers"

case "$1" in

list)
    find "$WALLPAPER_DIR" \
        -maxdepth 1 \
        -type f \
        \( -iname "*.png" -o -iname "*.jpg" \) \
        -printf "%p\n"
;;

run)
    # awww img "$2" --transition-type any --transition-step 128 --transition-fps 75
    # notify-send -h boolean:transient:true "Theme applied" "Wallpaper and theme updated successfully!"
    echo "Selected: $2"
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
# Icon = full path (preview)
# Description = raw text (full path)

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


### 🧩 `-dc, --display-columns <COLUMN>`

Show only selected columns from multi-column input.

Example:

```sh
cliphist list | smilemenu --dmenu --display-columns 2 -ph 'Search clipboard...' | cliphist decode | wl-copy
```


### 🧩 `-w, --width <WIDTH>`

Overrides the window width.

Example:

```sh
smilemenu -w 700
```

### 🧩 `-ntf, --no-text-field`

Disables text field / search box.

Example:

```sh
smilemenu -ntf -p "Applications"
```


### 🧩 `-gc, --gen-config`

Generates default config file (`~/.config/smilemenu/config.json`).


### 🧩 `-gc, --gen-theme`

Generates default theme file (`~/.config/smilemenu/theme.json`).


### 🧩 `-c, --config <PATH>`

Overrides config file. Also can be used to generated config files with different names.

Examples:

```sh
# Uses a different config file
smilemenu -c ~/.config/smilemenu/other-config.json

# Generates new.json config file
smilemenu -gc -c ~/.config/smilemenu/new.json
```


### 🧩 `-t, --theme <PATH>`

Overrides theme file. Also can be used to generated theme files with different names.

Examples:

```sh
# Uses a different theme file
smilemenu -c ~/.config/smilemenu/other-theme.json

# Generates material.json theme file
smilemenu -gc -c ~/.config/smilemenu/material.json
```


---

## Keyboard Controls

| Key    | Action        |
| ------ | ------------- |
| Up     | Previous item |
| Down   | Next item     |
| Enter  | Launch        |
| Escape | Close         |
