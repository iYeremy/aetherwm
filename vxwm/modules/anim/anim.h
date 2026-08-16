#pragma once

/* anim - smooth window resize animation.
 *
 * Interpolates a window's geometry (position and size) from its current
 * on-screen state to the requested target with an ease-out cubic curve, so
 * windows glide into place instead of jumping in one step. This is what keeps
 * compositor-heavy sessions (blur, transparent terminals, ...) from looking
 * choppy while windows are resized or re-arranged.
 *
 * Implementation notes:
 *  - The Client struct keeps the TARGET geometry (c->x/y/w/h) immediately, so
 *    the layout math (arrange(), bsp, ...) always sees the final values; only
 *    the real X window is moved towards the target by the animation tick.
 *  - New targets retarget the animation from the current interpolated
 *    position, so rapid consecutive resizes (mouse drags) stay smooth instead
 *    of lagging behind or stuttering.
 *  - Position-only changes (e.g. dragging a floating window) are applied
 *    instantly so moving windows stays perfectly responsive; only size
 *    changes are animated.
 *  - anim_tick() is driven from the core run() loop, which polls with
 *    nanosleep(ANIM_TICK_MS) so animations advance while the WM is idle.
 *
 * Depends on:  SMOOTH_RESIZE
 * Adds to Client: anim (the running animation, if any).
 * Core hooks used:
 *   - resizeclient() routes through anim_resizeclient().
 *   - run() calls anim_tick() every poll cycle.
 *   - unmanage() calls anim_cancel() before the Client is freed.
 */

/* --- Module-local tunables ------------------------------------------- */

#define ANIM_DURATION_MS 120   /* length of one window animation (ms) */
#define ANIM_TICK_MS     8     /* animation frame interval (~125 fps) */

struct Client;

void anim_resizeclient(struct Client *c, int x, int y, int w, int h);
void anim_tick(void);
void anim_cancel(struct Client *c);
