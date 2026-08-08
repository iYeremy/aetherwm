#!/bin/bash
# wallpaper.sh — pick a wallpaper with rofi, apply it with xwallpaper, then
# generate a pywal palette, reload Xresources and refresh vxwm colors (via a
# synthetic Super+F5 keypress only when xdotool is available).

set -e

WALL_DIR="$HOME/Pictures/Wallpaper"

mkdir -p "$WALL_DIR"

ROFI_INPUT=""
while read -r file; do
    if [ -n "$file" ]; then
        ROFI_INPUT+="${file}\0icon\x1f${WALL_DIR}/${file}\n"
    fi
done <<< "$(ls -1 "$WALL_DIR" | grep -E "\.(jpg|jpeg|png|webp)$")"

SELECTION=$(echo -e "$ROFI_INPUT" | rofi -show-icons -theme ~/.config/rofi/wall-changer.rasi -dmenu -i -p "Wallpaper")

if [ -z "$SELECTION" ]; then
    exit 1
fi

WALLPAPER="$WALL_DIR/$SELECTION"

xwallpaper --stretch "$WALLPAPER"

# -n: do not let pywal set the wallpaper itself (xwallpaper already did)
wal -i "$WALLPAPER" -n

# Merge the new palette into the Xresource db so the xrdb patch in vxwm
# picks it up when the colors are reloaded.
xrdb -merge "$HOME/.cache/wal/colors.Xresources"

# Ask vxwm to reload its colors (Simulate Mod+F5). Only when xdotool exists.
if command -v xdotool >/dev/null 2>&1; then
    xdotool key Super+F5
fi

if [ -f "$HOME/wal-dunst.sh" ]; then
    "$HOME/wal-dunst.sh"
elif [ -f "$HOME/.config/wal/wal-dunst.sh" ]; then
    "$HOME/.config/wal/wal-dunst.sh"
fi

notify-send "Wallpaper Updated" "System color theme has been synchronized!"
