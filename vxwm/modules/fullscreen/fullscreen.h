#pragma once

/* fullscreen - fullscreen on a single monitor (instead of the virtual
 * screen used by the core's togglefullscr()).
 *
 * Depends on:  FULLSCREEN
 * Hooks used:  core keybind in config.h (MOD+F).
 *
 * fullscreen() maximises the selected client across its monitor. The client
 * keeps its normal geometry in `c->oldx/oldy/oldw/oldh` and the monitor's
 * state is restored by calling fullscreen() again or by core arrange().
 */

void fullscreen(const Arg *arg);
