## ⚠️ Note

If newly installed or modified applications are not executing/launching or not launching correctly, just delete the cache file (`rm ~/.cache/smilemenu/apps_cache.json`).


# ⭐ Page Contents

- [Usage](#usage)
    - [First steps](#start-the-daemon)
	- [-h, --help](#h-help)
	- [--daemon](#daemon)
	- [-p, --prompt](#p-prompt-prompt)
	- [--pp, --prompt-position](#pp-prompt-position-top-entry-hidden)
	- [--ph, --placeholder](#ph-placeholder-placeholder)
	- [--provider](#provider-script)
	- [--field](#field-field-mode)
	- [-w, --width](#w-width-width)
	- [--ntf, --no-text-field](#ntf-no-text-field)
	- [--mi, --max-items](#mi-max-items-count)
	- [--gc, --gen-config](#gc-gen-config)
	- [--gt, --gen-theme](#gt-gen-theme)
	- [-c, --config](#c-config-path)
	- [-t, --theme](#t-theme-path)
	- [--dcache](#dcache-app-all)
- [Keyboard Controls](#keyboard-controls)


# ⭐ Usage


### 🧩 Start the daemon

```sh
smilemenu --daemon # OR --daemon &
```


### 🧩 Normal application launcher

```sh
smilemenu
```


### 🧩 `-h, --help`

Shows help message.


### 🧩 `--daemon`

Start the daemon.

```sh
smilemenu --daemon # OR --daemon &
```


### 🧩 `-p, --prompt <PROMPT>`

Show a prompt text next to or above the search field.

Example:

```sh
smilemenu -p "Apps"
```


### 🧩 `--pp, --prompt-position {top,entry,hidden}`

Positions the prompt text. Entry by default.

Examples:

```sh
smilemenu -p "Apps" --prompt-position top

smilemenu -p "Apps" --prompt-position entry
```


### 🧩 `--ph, --placeholder <PLACEHOLDER>`

Overrides the default placeholder text.

Example:

```sh
smilemenu --placeholder "Search Apps..."
```


### 🧩 `--provider <SCRIPT>`

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

| Mode       | Description                                            | Example                    |
| ---------- | -----------------------------------------------------  | -------------------------- |
| `<column>` | Uses a specific column number from the provider output | `--field description:2`    |
| `path`     | Uses the file/directory path as the value              | `--field icon:path`        |
| `text`     | Uses the raw text/value as-is                          | `--field name:text`        |
| `none`     | Sets the field to empty string                         | `--field description:none` |

Example 1:

```sh
# Name = filename from path
# Icon = full path (preview)
# Description = raw text (full path)

smilemenu --provider ~/.local/bin/wallpaper-provider.sh --field name:path --field icon:path --field description:text
```

Example 2:

```sh
# Name = 1st column
# Description = 2nd column

smilemenu --provider ~/cliphist-provider.sh --field name:1 --field description:2
```


### 🧩 `-w, --width <WIDTH>`

Overrides the window width.

Example:

```sh
smilemenu -w 700
```


### 🧩 `--ntf, --no-text-field`

Disables text field / search box.


### 🧩 `--mi, --max-items <COUNT>`

Set custom max visible items (list height).

Example:

```sh
smilemenu --max-items 3
```


### 🧩 `--gc, --gen-config`

Generates default config file (`~/.config/smilemenu/config.json`).


### 🧩 `--gt, --gen-theme`

Generates default theme file (`~/.config/smilemenu/theme/Main.qml`).


### 🧩 `-c, --config <PATH>`

Overrides config file. Can also be use to generate custom config files with different names and in different path.

Examples:

```sh
# Uses a different config file
smilemenu --config ~/.config/smilemenu/new-config.json

# Generates a custom config file
smilemenu --gen-config --config ~/Documents/another.json
```


### 🧩 `-t, --theme <PATH>`

Overrides theme file. Can also be use to generate custom theme files with different names and in different path.

Examples:

```sh
# Uses a different theme file
smilemenu --theme ~/.config/smilemenu/theme/new-theme.qml

# Generates a custom theme file
smilemenu --gen-theme --theme ~/Documents/other-theme.qml
```


### 🧩 `--dcache {app,all}`

Delete cache; either app cache or all cache.

Examples:

```sh
# Deletes just the app cache
smilemenu --dcache app

# Deletes the entire cache directory (~/.cache/smilemenu/)
smilemenu --dcache all
```


# ⭐ Keyboard Controls

| Key    | Action        |
| ------ | ------------- |
| Up     | Previous item |
| Down   | Next item     |
| Enter  | Execute       |
| Escape | Close         |
