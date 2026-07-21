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

# smilemenu --provider path/to/this/example-2.sh -p "Wallpaper"