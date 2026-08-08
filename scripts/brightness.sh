#!/bin/bash
# brightness.sh — adjust backlight brightness via brightnessctl + dunst notification.

brightnessctl set "$1"

BRIGHT=$(brightnessctl i | grep -oP '\(\K[^)]+(?=\%)')

dunstify -r 9994 -a "Brightness" "Brightness: $BRIGHT%" -h int:value:"$BRIGHT" -h string:x-dunst-stack-tag:brightness
