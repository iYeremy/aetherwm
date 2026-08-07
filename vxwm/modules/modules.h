#pragma once

/* vxwm compile-time feature switches.
 *
 * Each switch gates one module in modules/<name>/. Enabled modules are compiled
 * as their own translation unit and linked with the core; disabled modules
 * reduce to an empty translation unit, so the Makefile always builds every
 * module and never needs editing.
 *
 * This file is the single source of truth for the switches. `vxwm.h` includes
 * it so that the core, drw and every module see the same set of macros.
 *
 * Roadmap (docs/ANALISIS.md):
 *   Phase 2 - core modules (these): windowmap, gaps, fullscreen, xrdb, etf,
 *             ewmh_tags, externalbars.
 *   Phase 3 - infinitetags.
 *   Phase 4 - moveresizekbd, directionalfocus, directionalmove, betterresize,
 *             zoom, warp, autostart.
 */

/* --- Tagging ----------------------------------------------------------- */

#define WINDOWMAP 1
/* Windows are mapped/unmapped in X11 instead of being moved off-screen. Fixes
 * apps that lose input after a tag change and lets compositors fade windows
 * in/out on tag switch. Adds `int ismapped;` to Client. */

/* --- Eye candy --------------------------------------------------------- */

#define GAPS 1
/* Gaps between tiled windows. Adds `int gappx;` to Monitor and provides
 * `gaps_tile()` (bind it in config.h `layouts[]` instead of `tile`) plus the
 * `setgaps` keybind. */

#define XRDB 1
/* Load colors from the X resource database (`dwm.color0/6`) at startup and
 * reload them on demand (keybind `xrdb`). Switches config.h color arrays to
 * non-const (`MAYBE_CONST`) and `drw_scm_create()` accordingly. */

/* --- Floating ---------------------------------------------------------- */

#define FLOATING_LAYOUT_FLOATS_WINDOWS 1
/* In the floating layout windows are truly floating: on manage() they are
 * flagged `isfloating`, so switching to a tiled layout re-tiles them. */

#define ENHANCED_TOGGLE_FLOATING 1
/* "Smart" toggle floating: remembers size/position (Client.sfx/sfy/sfw/sfh)
 * and snaps back to the previous size on re-toggle. Requires
 * FLOATING_LAYOUT_FLOATS_WINDOWS (forced automatically). */

#define RESTORE_SIZE_AND_POS_ETF 1
/* When toggling floating, restore the previous size and position. */

#define FULLSCREEN 1
/* Fullscreen the selected client on its own monitor (MOD+Shift+F). The core
 * already handles _NET_WM_STATE_FULLSCREEN via setfullscreen(); this module
 * adds the classic single-monitor fullscreen() keybind. */

/* --- Bar / EWMH -------------------------------------------------------- */

#define EWMH_TAGS 1
/* Publish _NET_NUMBER_OF_DESKTOPS, _NET_CURRENT_DESKTOP, _NET_DESKTOP_NAMES,
 * _NET_DESKTOP_VIEWPORT and _NET_WM_DESKTOP so external bars can show tags. */

#define EXTERNAL_BARS 1
/* Reserve screen area for external bars that set _NET_WM_STRUT(_PARTIAL).
 * Adds `strut_*` fields to Monitor. */

/* --- Dependency rules -------------------------------------------------- */
#if ENHANCED_TOGGLE_FLOATING && !FLOATING_LAYOUT_FLOATS_WINDOWS
#undef FLOATING_LAYOUT_FLOATS_WINDOWS
#define FLOATING_LAYOUT_FLOATS_WINDOWS 1
#endif
