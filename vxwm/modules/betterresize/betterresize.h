#pragma once

/* betterresize - resize a window from any edge or corner.
 *
 * Depends on:  BETTER_RESIZE
 * Hooks used:  none in the core; bind betterresize_resizemouse() in
 *              config.def.h buttons[] instead of resizemouse().
 *
 * The drag side is picked from where the pointer grabbed the window (three
 * zones per axis). Grabbing near an edge/corner changes the cursor to the
 * matching X shape (BR_CHANGE_CURSOR). Resizing a tiled window past `snap`
 * floats it (RESIZING_WINDOWS_IN_ALL_LAYOUTS_FLOATS_THEM controls whether
 * the window must stay inside the monitor work area for that to happen).
 * The loop is rate-limited to `refreshrate` updates/second when
 * LOCK_MOVE_RESIZE_REFRESH_RATE is on.
 */

void betterresize_resizemouse(const Arg *arg);
