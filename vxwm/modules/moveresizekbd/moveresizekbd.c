#include "vxwm.h"
#include "modules/moveresizekbd/moveresizekbd.h"
#if INFINITE_TAGS
#include "modules/infinitetags/infinitetags.h"
#endif
#if DIRECTIONAL_MOVE
#include "modules/directionalmove/directionalmove.h"
#endif
#if WARP_TO_CLIENT && WARP_TO_CENTER_OF_WINDOW_MOVED_BY_KEYBOARD
#include "modules/warptoclient/warptoclient.h"
#endif

#if MOVE_RESIZE_WITH_KEYBOARD

#if !INFINITE_TAGS

void
moveresize(const Arg *arg)
{
	XEvent ev;
	Monitor *m = selmon;

	if (m->sel && m->sel->isfullscreen)
		return;

	if (!(m->sel && arg && arg->v && m->sel->isfloating)) {
#if DIRECTIONAL_MOVE
		if (((int *)arg->v)[0] != 0 || ((int *)arg->v)[1] != 0) {
			if (((int *)arg->v)[1] > 0) movedir(&(Arg){.i = 3}); /* Down */
			if (((int *)arg->v)[1] < 0) movedir(&(Arg){.i = 2}); /* Up */
			if (((int *)arg->v)[0] > 0) movedir(&(Arg){.i = 1}); /* Right */
			if (((int *)arg->v)[0] < 0) movedir(&(Arg){.i = 0}); /* Left */
		}
#endif
		return;
	}

	XRaiseWindow(dpy, m->sel->win);

	resize(m->sel, m->sel->x + ((int *)arg->v)[0],
		m->sel->y + ((int *)arg->v)[1],
		m->sel->w + ((int *)arg->v)[2],
		m->sel->h + ((int *)arg->v)[3],
		True);

#if ENHANCED_TOGGLE_FLOATING && RESTORE_SIZE_AND_POS_ETF
	if (m->sel->isfloating) {
		m->sel->wasmanuallyedited = 1;
		m->sel->sfx = m->sel->x;
		m->sel->sfy = m->sel->y;
		m->sel->sfw = m->sel->w;
		m->sel->sfh = m->sel->h;
	}
#endif

	focus(m->sel);

#if WARP_TO_CLIENT && WARP_TO_CENTER_OF_WINDOW_MOVED_BY_KEYBOARD
	warptoclient(m->sel);
#endif

	while (XCheckMaskEvent(dpy, EnterWindowMask, &ev));
}

#else /* INFINITE_TAGS */

void
moveresize(const Arg *arg)
{
	int dx, dy, dw, dh, tagidx, nx, ny, nw, nh;
	int canvas_dx = 0, canvas_dy = 0;
	Client *c, *tmp;
	Monitor *m = selmon;
	XEvent ev;

	if (m->sel && m->sel->isfullscreen)
		return;
	if (!m->sel || !arg || !arg->v)
		return;

	dx = ((int *)arg->v)[0];
	dy = ((int *)arg->v)[1];
	dw = ((int *)arg->v)[2];
	dh = ((int *)arg->v)[3];

	if (!m->sel->isfloating) {
#if DIRECTIONAL_MOVE
		if (selmon->lt[selmon->sellt]->arrange != NULL && (dx || dy)) {
			if (dy > 0) movedir(&(Arg){.i = 3}); /* Down */
			if (dy < 0) movedir(&(Arg){.i = 2}); /* Up */
			if (dx > 0) movedir(&(Arg){.i = 1}); /* Right */
			if (dx < 0) movedir(&(Arg){.i = 0}); /* Left */
		}
#endif
		return;
	}

	c = m->sel;

	XRaiseWindow(dpy, c->win);

	if (selmon->lt[selmon->sellt]->arrange != NULL) {
		resize(c, c->x + dx, c->y + dy, c->w + dw, c->h + dh, True);
	} else {
		nx = c->x + dx;
		ny = c->y + dy;
		nw = c->w + dw;
		nh = c->h + dh;
		tagidx = getcurrenttag(m);

		if (dx || dy) {
			/* push the window against the viewport edge: drag the canvas
			 * along so the window stays on screen instead of sliding away */
			if (nx < m->wx) {
				canvas_dx = nx - m->wx;
				nx = m->wx;
			} else if (nx + nw + 2 * c->bw > m->wx + m->ww) {
				canvas_dx = (nx + nw + 2 * c->bw) - (m->wx + m->ww);
				nx = m->wx + m->ww - nw - 2 * c->bw;
			}

			if (ny < m->wy) {
				canvas_dy = ny - m->wy;
				ny = m->wy;
			} else if (ny + nh + 2 * c->bw > m->wy + m->wh) {
				canvas_dy = (ny + nh + 2 * c->bw) - (m->wy + m->wh);
				ny = m->wy + m->wh - nh - 2 * c->bw;
			}

			if (canvas_dx || canvas_dy) {
				m->canvas[tagidx].cx -= canvas_dx;
				m->canvas[tagidx].cy -= canvas_dy;

				for (tmp = m->clients; tmp; tmp = tmp->next) {
					if (ISVISIBLE(tmp) && tmp != c) {
						tmp->x -= canvas_dx;
						tmp->y -= canvas_dy;
						XMoveWindow(dpy, tmp->win, tmp->x, tmp->y);
					}
				}
			}
		}

		resize(c, nx, ny, nw, nh, True);
	}

#if ENHANCED_TOGGLE_FLOATING && RESTORE_SIZE_AND_POS_ETF
	if (c->isfloating) {
		c->wasmanuallyedited = 1;
		c->sfx = c->x;
		c->sfy = c->y;
		c->sfw = c->w;
		c->sfh = c->h;
	}
#endif

	focus(c);

#if WARP_TO_CLIENT && WARP_TO_CENTER_OF_WINDOW_MOVED_BY_KEYBOARD
	warptoclient(c);
#endif

	while (XCheckMaskEvent(dpy, EnterWindowMask, &ev));

#if IT_SHOW_COORDINATES_IN_BAR
	drawbar(m);
#endif
}

#endif /* INFINITE_TAGS */

#endif /* MOVE_RESIZE_WITH_KEYBOARD */

/* Keep this translation unit non-empty when the module is disabled
 * (ISO C forbids an empty translation unit). */
typedef int vxwm_moveresizekbd_module_present;
