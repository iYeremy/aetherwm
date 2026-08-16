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

#define INFINITE_TAGS 1
/* "Infinite canvas" per tag: instead of discrete pages, windows live on a
 * virtual plane and the viewport slides over it (floating layout). This is
 * vxwm's flagship feature. Requires WINDOWMAP (forced automatically).
 * Adds to Client: saved_cx/cy/cw/ch, was_on_canvas, is_pinned.
 * Adds to Monitor: `canvas[ntags]` of CanvasOffset.
 * Module-local tunables live in modules/infinitetags/infinitetags.h. */

#define IT_SHOW_COORDINATES_IN_BAR 1
/* Show the current canvas position in the bar (floating layout only).
 * COORDINATES_STYLE / COORDINATES_DIVISOR are configured in the
 * infinitetags module header. */

#define TAG_TO_TAG 0
/* Pressing the tag you are already viewing switches to the previously viewed
 * tag instead of doing nothing (the vanilla `view` behaviour). Off: pressing
 * the current workspace tag is a no-op (avoids accidental workspace jumps). */

#define MOVE_IN_TILED 1
/* Dragging a tiled window with the mouse reorders it among the tiled clients
 * (and moves it to another monitor when dragged across one), instead of
 * toggling it to floating. */

#define MOVE_RESIZE_WITH_KEYBOARD 1
/* Move and resize the selected window with the keyboard (arrows).
 * Step sizes are configured in config.def.h (MOVE/RESIZE_WITH_KEYBOARD_STEP).
 * In tiled layouts the arrow keys reorder clients when DIRECTIONAL_MOVE is
 * on; in the floating layout the infinite canvas is pushed when a window
 * hits the viewport edge (INFINITE_TAGS). */

#define DIRECTIONAL_FOCUS 1
/* Focus the tiled/floating client closest in a given direction
 * (ALT + arrows), using geometric scoring. */

#define DIRECTIONAL_MOVE 1
/* Move a tiled client in a direction by swapping it with the closest client
 * (used by the MOVE_RESIZE_WITH_KEYBOARD arrows and independently bindable). */

#define BSP_LAYOUT 1
/* Balanced binary-space-partition tiling: new windows split the leaf under
 * the mouse pointer, so the layout never collapses on one side and four
 * quadrants are reached naturally. Replaces the master+stack tile as the
 * default tiled layout. Adds `bspnode` to Client and `bsproot` to Monitor;
 * hooks core swaptile/setmfact/unmanage. */

/* --- Interaction ------------------------------------------------------- */

#define BETTER_RESIZE 1
/* 8-sided window resize (drag from the relevant edge/corner). Provides
 * `betterresize_resizemouse`; bind it in config.def.h buttons[] instead of
 * `resizemouse`. */

#define BR_CHANGE_CURSOR 1
/* Dynamic resize cursors for the 8 sides/corners when BETTER_RESIZE is on.
 * Adds 8 entries to the core cursor enum. */

#define RESIZING_WINDOWS_IN_ALL_LAYOUTS_FLOATS_THEM 1
/* BETTER_RESIZE: when resizing a tiled window past `snap`, float it instead
 * of only doing so while staying inside the monitor work area. */

#define USE_RESIZECLIENT_FUNC 0
/* BETTER_RESIZE: use resizeclient() instead of resize() while dragging, so
 * size-hint negotiation (applysizehints) is bypassed during the drag. */

#define LOCK_MOVE_RESIZE_REFRESH_RATE 1
/* Rate-limit mouse move/resize to `refreshrate` updates/second to spare
 * CPU-rendered clients. The core movemouse/resizemouse always rate-limit;
 * this switch also enables it in the module-driven loops (betterresize,
 * infinitetags movecanvasmouse). */

#define ZOOM 1
/* Optional vcompmgr zoom integration: mouse movement is divided by the
 * compositor's current zoom factor (read from a vcompmgr socket file).
 * Requires vcompmgr installed and running, otherwise it is a silent no-op.
 * Off by default; enable it and add `vcompmgr` to your dependencies. */

#define WARP_TO_CLIENT 0
/* Warp the cursor to the centre of a client. Enables the WARP_TO_* options
 * below; each is additionally gated by its own switch. Off by default. */

#define WARP_TO_CENTER_OF_NEW_WINDOW 0
#define WARP_TO_CENTER_OF_PREVIOUS_WINDOW 0
#define WARP_TO_CENTER_OF_SWAPMASTERED_WINDOW 0
#define WARP_TO_CENTER_OF_WINDOW_AFFECTED_BY_INCNMASTER 0
#define WARP_TO_CENTER_OF_WINDOW_AFFECTED_BY_ENHANCED_TOGGLE_FLOATING 0
#define WARP_TO_CENTER_OF_WINDOW_AFFECTED_BY_FOCUSSTACK 0
#define WARP_TO_CENTER_OF_WINDOW_MOVED_BY_KEYBOARD 0

/* --- Startup ----------------------------------------------------------- */

#define AUTOSTART 1
/* Run a list of shell commands at startup. The command list lives in
 * modules/autostart/autostart.c (module-local config). Start with
 * `vxwm -ignoreautostart` to skip it. */

/* --- Eye candy --------------------------------------------------------- */

#define GAPS 1
/* Gaps between tiled windows. Adds `int gappx;` to Monitor and provides
 * `gaps_tile()` (bind it in config.h `layouts[]` instead of `tile`) plus the
 * `setgaps` keybind. */

#define SMOOTH_RESIZE 1
/* Animate window geometry changes (resize/re-arrange) with an ease-out tween
 * instead of jumping in one step, so compositor-heavy sessions look smooth.
 * Position-only moves (floating window drags) stay instant. Adds `anim` to
 * Client; hooks resizeclient(), run() and unmanage(). */

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

#define BAR_HEIGHT 1
/* Override the bar height (set `user_bh` in config.def.h). Off = derive the
 * height from the font size. */

#define BAR_PADDING 1
/* Draw the bar content with horizontal/vertical insets (`left_sidepad`,
 * `right_sidepad`, `top_vertpad`, `bottom_vertpad` in config.def.h). With
 * large side pads this produces the "centered pill" bar. Off = full-width
 * bar content. */

#define EWMH_TAGS 1
/* Publish _NET_NUMBER_OF_DESKTOPS, _NET_CURRENT_DESKTOP, _NET_DESKTOP_NAMES,
 * _NET_DESKTOP_VIEWPORT and _NET_WM_DESKTOP so external bars can show tags. */

#define EXTERNAL_BARS 1
/* Reserve screen area for external bars that set _NET_WM_STRUT(_PARTIAL).
 * Adds `strut_*` fields to Monitor. */

/* --- Dependency rules -------------------------------------------------- */

/* INFINITE_TAGS requires WINDOWMAP (off-screen windows are hidden by
 * unmap/remap, which is what lets the canvas move windows around). */
#if INFINITE_TAGS && !WINDOWMAP
#undef WINDOWMAP
#define WINDOWMAP 1
#endif

#if ENHANCED_TOGGLE_FLOATING && !FLOATING_LAYOUT_FLOATS_WINDOWS
#undef FLOATING_LAYOUT_FLOATS_WINDOWS
#define FLOATING_LAYOUT_FLOATS_WINDOWS 1
#endif

/* The WARP_TO_* options are inert without WARP_TO_CLIENT. */
#if !WARP_TO_CLIENT
#undef WARP_TO_CENTER_OF_NEW_WINDOW
#define WARP_TO_CENTER_OF_NEW_WINDOW 0
#undef WARP_TO_CENTER_OF_PREVIOUS_WINDOW
#define WARP_TO_CENTER_OF_PREVIOUS_WINDOW 0
#undef WARP_TO_CENTER_OF_SWAPMASTERED_WINDOW
#define WARP_TO_CENTER_OF_SWAPMASTERED_WINDOW 0
#undef WARP_TO_CENTER_OF_WINDOW_AFFECTED_BY_INCNMASTER
#define WARP_TO_CENTER_OF_WINDOW_AFFECTED_BY_INCNMASTER 0
#undef WARP_TO_CENTER_OF_WINDOW_AFFECTED_BY_ENHANCED_TOGGLE_FLOATING
#define WARP_TO_CENTER_OF_WINDOW_AFFECTED_BY_ENHANCED_TOGGLE_FLOATING 0
#undef WARP_TO_CENTER_OF_WINDOW_AFFECTED_BY_FOCUSSTACK
#define WARP_TO_CENTER_OF_WINDOW_AFFECTED_BY_FOCUSSTACK 0
#undef WARP_TO_CENTER_OF_WINDOW_MOVED_BY_KEYBOARD
#define WARP_TO_CENTER_OF_WINDOW_MOVED_BY_KEYBOARD 0
#endif
