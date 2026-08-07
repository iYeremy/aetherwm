#pragma once

/* etf - enhanced toggle floating + floating layout floats windows.
 *
 * Depends on:  ENHANCED_TOGGLE_FLOATING  (and/or FLOATING_LAYOUT_FLOATS_WINDOWS)
 * Adds to Client: int isfloating; int wasfloating; int nfixed;
 * Hooks used:  core manage() applies rules and updates `nfixed`;
 *              core wmname/handler dispatch (handled in config.h keybinds and
 *              the core's arrange() logic via `isfloating`).
 *
 * togglefloat() switches the selected client between floating and tiled,
 * restoring any previously saved fixed geometry.
 */

void togglefloat(const Arg *arg);
