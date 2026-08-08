#!/bin/bash
# screenshot.sh — interactive (selection) screenshot via scrot + notification.
# Bound to the Print key in vxwm config.def.h.

set -e

DIR="$HOME/Pictures/Screenshot"
mkdir -p "$DIR"

FILE="$DIR/$(date +%d-%m-%Y_%H-%M-%S).png"

scrot -s "$FILE"

if [ -f "$FILE" ]; then
    notify-send -a "Screenshot" -i "$FILE" "Screenshot Taken" "Saved to: $(basename "$FILE")"
fi
