#!/bin/bash
# Enable tap-to-click and natural scrolling for the touchpad in the current
# X session, without needing root. This is the user-space counterpart of
# config/xorg/30-touchpad.conf (which does the same thing for every X start
# once installed to /etc/X11/xorg.conf.d/); it is run from the vxwm autostart
# so the settings apply even if the Xorg file has not been deployed.
#
#  1 finger tap / press  = left click
#  2 finger tap / press  = right click
#  2 finger swipe = scroll (and pans the infinite canvas over the background)
# (This ELAN surface has a single physical left button; the two-finger press
# is the only way to right-click without tapping.)

TP="$(xinput list --name-only 2>/dev/null | grep -i touchpad | head -1)"
[ -z "$TP" ] && exit 0

xinput set-prop "$TP" "libinput Tapping Enabled" 1
# LRM tap map (already the driver default): 1 finger tap = left, 2 finger tap
# = right. Set explicitly; some drivers reject the write, hence `|| true`.
xinput set-prop "$TP" "libinput Tapping Button Mapping Enabled" 1 0 || true
# clickfinger: physical press with 1 finger = left, 2 fingers = right.
xinput set-prop "$TP" "libinput Click Method Enabled" 1 0 || true
xinput set-prop "$TP" "libinput Natural Scrolling Enabled" 1
xinput set-prop "$TP" "libinput Scroll Method Enabled" 1 0 0
