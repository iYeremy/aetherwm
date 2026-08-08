#pragma once

/* zoom - adapt move/resize speed to a compositor zoom factor.
 *
 * Depends on:  ZOOM
 * Hooks used:  none; zoom_value() is called by the core movemouse() and by the
 *              infinitetags movecanvasmouse() loops when ZOOM is enabled.
 *
 * While the compositor is zoomed in, mouse deltas are divided by the current
 * zoom factor so 1:1 screen movement maps to the same canvas movement. The
 * factor is read from vcompmgr's socket file (/tmp/vcompmgr_$DISPLAY.sock.zoom).
 * When vcompmgr is not installed/running, zoom_value() returns 1.0 and every
 * zoomed code path behaves exactly as before, so the switch is a safe no-op.
 */

float zoom_value(void);
