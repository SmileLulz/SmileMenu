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

# smilemenu --provider path/to/this/example-2.sh -p "Wallpaper"