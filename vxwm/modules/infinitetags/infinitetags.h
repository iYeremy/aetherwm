#pragma once

/* infinitetags - one "infinite canvas" per tag, viewed through a viewport.
 *
 * Depends on:  INFINITE_TAGS (requires WINDOWMAP, forced by modules.h)
 * Adds to Client: saved_cx/cy/cw/ch, was_on_canvas, is_pinned.
 * Adds to Monitor: canvas[ntags] of CanvasOffset (allocated by core createmon,
 *              freed by core cleanupmon).
 * Hooks used:  core applysizehints() skips the interact screen clamp;
 *              core createmon()/cleanupmon() manage the canvas array;
 *              core drawbar() prints the current canvas coordinates;
 *              core focusstack() centres the newly focused window;
 *              core restack() keeps pinned windows above the others;
 *              core setlayout()/view()/tag() save and restore positions.
 *
 * Each tag owns a virtual plane. In the floating layout the viewport slides
 * over it (movecanvas, movecanvasmouse, homecanvas); windows on it are moved
 * in the opposite direction so they stay anchored to the canvas. When the
 * layout is tiled the viewport is parked and the client positions are saved;
 * switching back restores the plane exactly where you left it.
 */

/* Module-local tunables. They are plain macros so the core integration points
 * (drawbar, restack) can use them too; they compile out when the module is
 * off. */
#define MOVE_CANVAS_STEP 120          /* pixels per canvas key step */
#define COORDINATES_DIVISOR 10        /* drawbar coordinates are scaled by this */
#define COORDINATES_STYLE "[x%d y%d]" /* drawbar coordinate format string */
#define PINNED_WINDOWS_ALWAYS_ON_TOP 1 /* pinned windows stay above others */

/* guard the drawbar division against a zero divisor */
#if COORDINATES_DIVISOR <= 0
#undef COORDINATES_DIVISOR
#define COORDINATES_DIVISOR 1
#endif

int getcurrenttag(Monitor *m);
void homecanvas(const Arg *arg);
void movecanvas(const Arg *arg);
void movecanvasmouse(const Arg *arg);
void save_canvas_positions(Monitor *m);
void restore_canvas_positions(Monitor *m);
void centerwindow(const Arg *arg);
void pinwindow(const Arg *arg);
