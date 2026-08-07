#pragma once

/* gaps - gaps between tiled windows.
 *
 * Depends on:  GAPS
 * Adds to Monitor: int gappx;
 * Hooks used:  core createmon() initialises m->gappx from config `gappx`;
 *              core calls setgaps via the keybinds in config.h.
 *
 * setgaps() changes the gap size of the selected monitor (arg.i adds/removes
 * pixels; arg.i == 0 resets to 0).
 *
 * NOTE: bind `gaps_tile` in config.h `layouts[]` (instead of `tile`) to make
 * the tiled layout use gaps.
 */

void setgaps(const Arg *arg);
void gaps_tile(Monitor *m);
