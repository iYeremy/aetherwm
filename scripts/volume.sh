#!/bin/bash
# volume.sh — volume up/down/toggle via wpctl (PipeWire) + dunst notification.

if [ "$1" = "i" ]; then
    wpctl set-volume @DEFAULT_AUDIO_SINK@ 5%+
elif [ "$1" = "d" ]; then
    wpctl set-volume @DEFAULT_AUDIO_SINK@ 5%-
elif [ "$1" = "t" ]; then
    wpctl set-mute @DEFAULT_AUDIO_SINK@ toggle
fi

VOL=$(wpctl get-volume @DEFAULT_AUDIO_SINK@)

if echo "$VOL" | grep -q "MUTED"; then
    dunstify -r 9993 -a "Volume" "Volume: MUTED" -h string:x-dunst-stack-tag:volume
else
    VOL_NUM=$(echo "$VOL" | awk '{print int($2*100)}')
    dunstify -r 9993 -a "Volume" "Volume: $VOL_NUM%" -h int:value:"$VOL_NUM" -h string:x-dunst-stack-tag:volume
fi
