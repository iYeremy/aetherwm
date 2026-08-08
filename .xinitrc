#!/bin/bash
# .xinitrc — start the aetherwm X session.

export XDG_SESSION_TYPE=x11
export XDG_CURRENT_DESKTOP=vxwm

if command -v dbus-launch >/dev/null 2>&1; then
    eval "$(dbus-launch --sh-syntax)"
    export DBUS_SESSION_BUS_ADDRESS
fi

WALLPAPER="$HOME/Pictures/Wallpaper/wallhaven-6lk75x.jpg"

if [ -f "$WALLPAPER" ]; then
    xwallpaper --stretch "$WALLPAPER" 2>/dev/null || true
    wal -i "$WALLPAPER" -n 2>/dev/null || true
else
    xwallpaper --stretch solid:#1e1e2e 2>/dev/null || true
fi

if command -v picom >/dev/null 2>&1; then
    picom -b 2>/dev/null || picom &
fi

exec vxwm
