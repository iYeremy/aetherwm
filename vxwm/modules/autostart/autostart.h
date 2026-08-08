#pragma once

/* autostart - run a list of shell commands when the WM starts.
 *
 * Depends on:  AUTOSTART
 * Hooks used:  core main() calls runautostart() unless `vxwm -ignoreautostart`
 *              was given (and advertises the flag in the usage string).
 *
 * The command list lives in autostart.c (module-local config), so enabling
 * this module never creates duplicate-symbol or ordering issues with the
 * user's config.h. Each entry is spawned as `sh -c <cmd>` in its own process
 * group; the WM detaches immediately and does not wait for them.
 */

void runautostart(void);
