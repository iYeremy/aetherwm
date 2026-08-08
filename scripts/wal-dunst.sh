#!/bin/bash
# wal-dunst.sh — regenerate dunstrc from the pywal template (the single source
# of truth, kept in aetherwm/config/wal/templates/dunst) and restart dunst.
# Run automatically by wallpaper.sh after a palette change.

set -e

TEMPLATE_OUT="$HOME/.cache/wal/dunst"
DUNSTRC="$HOME/.config/dunst/dunstrc"

if [ ! -f "$TEMPLATE_OUT" ]; then
    notify-send "wal-dunst" "No pywal dunst template found at $TEMPLATE_OUT"
    exit 1
fi

mkdir -p "$HOME/.config/dunst"
cp "$TEMPLATE_OUT" "$DUNSTRC"

killall dunst 2>/dev/null || true
dunst &
