#pragma once

/* moveresizekbd - move and resize the focused window with the keyboard.
 *
 * Depends on:  MOVE_RESIZE_WITH_KEYBOARD
 * Hooks used:  none in the core; bind moveresize() in config.def.h keys[].
 *
 * `moveresize` takes an Arg with four ints (dx, dy, dw, dh). On tiled clients
 * the arrows reorder them through movedir() (DIRECTIONAL_MOVE); on floating
 * clients they nudge the window. In the floating layout (INFINITE_TAGS) a
 * floating window pushed against the viewport edge drags the canvas instead,
 * so the window never leaves the screen. The window is moved/resized at
 * refreshrate when LOCK_MOVE_RESIZE_REFRESH_RATE is on.
 */

void moveresize(const Arg *arg);
