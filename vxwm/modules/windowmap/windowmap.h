#pragma once

/* windowmap - map/unmap windows in X11 instead of moving them off-screen.
 *
 * Depends on:  WINDOWMAP
 * Adds to Client: int ismapped;
 * Hooks used:  core showhide()/arrange()/manage() (guarded by #if WINDOWMAP)
 *
 * window_map() shows a window (optionally resetting its WM_STATE to Normal),
 * window_unmap() hides it while suppressing the resulting UnmapNotify /
 * DestroyNotify storm, and window_set_state() writes the ICCCM WM_STATE.
 */

void window_set_state(Client *c, long state);
void window_map(Client *c, int deiconify);
void window_unmap(Client *c, int iconify);
