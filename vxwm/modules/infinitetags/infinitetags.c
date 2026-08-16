#include "vxwm.h"
#include "modules/infinitetags/infinitetags.h"
#if ZOOM
#include "modules/zoom/zoom.h"
#endif

#if INFINITE_TAGS

int
getcurrenttag(Monitor *m)
{
	unsigned int i;

	for (i = 0; i < ntags && !(m->tagset[m->seltags] & (1 << i)); i++);
	return i < ntags ? (int)i : 0;
}

void
homecanvas(const Arg *arg)
{
	Client *c;
	int tagidx = getcurrenttag(selmon);
	int cx = selmon->canvas[tagidx].cx;
	int cy = selmon->canvas[tagidx].cy;

	(void)arg;
	for (c = selmon->clients; c; c = c->next) {
		if ((c->tags & (1 << tagidx)) && !c->is_pinned) {
			c->x -= cx;
			c->y -= cy;
			XMoveWindow(dpy, c->win, c->x, c->y);
		}
	}

	selmon->canvas[tagidx].cx = 0;
	selmon->canvas[tagidx].cy = 0;
	drawbar(selmon);
	XFlush(dpy);
}

void
movecanvas(const Arg *arg)
{
	Client *c;
	int tagidx, dx = 0, dy = 0;

	if (selmon->lt[selmon->sellt]->arrange != NULL)
		return;
	if (selmon->sel && selmon->sel->isfullscreen)
		return;

	tagidx = getcurrenttag(selmon);

	switch (arg->i) {
	case 0: dx = -MOVE_CANVAS_STEP; break;
	case 1: dx =  MOVE_CANVAS_STEP; break;
	case 2: dy = -MOVE_CANVAS_STEP; break;
	case 3: dy =  MOVE_CANVAS_STEP; break;
	}

	selmon->canvas[tagidx].cx -= dx;
	selmon->canvas[tagidx].cy -= dy;

	for (c = selmon->clients; c; c = c->next) {
		if (ISVISIBLE(c) && !c->is_pinned) {
			c->x -= dx;
			c->y -= dy;
			XMoveWindow(dpy, c->win, c->x, c->y);
		}
	}

	drawbar(selmon);
}

void
movecanvasscroll(const Arg *arg)
{
	Client *c;
	int tagidx, dx = 0, dy = 0;

	if (selmon->lt[selmon->sellt]->arrange != NULL)
		return;
	if (selmon->sel && selmon->sel->isfullscreen)
		return;

	tagidx = getcurrenttag(selmon);

	/* Scroll events (mouse wheel / two-finger touchpad swipe) panned over the
	 * background navigate the infinite canvas in the floating layout, so the
	 * touchpad gesture mirrors dragging the background. The direction matches
	 * the movecanvas keybinds: Button4=up, Button5=down, Button6=left,
	 * Button7=right. */
	switch (arg->i) {
	case 0: dx = -MOVE_CANVAS_SCROLL_STEP; break; /* scroll left  */
	case 1: dx =  MOVE_CANVAS_SCROLL_STEP; break; /* scroll right */
	case 2: dy = -MOVE_CANVAS_SCROLL_STEP; break; /* scroll up    */
	case 3: dy =  MOVE_CANVAS_SCROLL_STEP; break; /* scroll down  */
	}

	selmon->canvas[tagidx].cx -= dx;
	selmon->canvas[tagidx].cy -= dy;

	for (c = selmon->clients; c; c = c->next) {
		if ((c->tags & (1 << tagidx)) && !c->is_pinned) {
			c->x -= dx;
			c->y -= dy;
			XMoveWindow(dpy, c->win, c->x, c->y);
		}
	}

	drawbar(selmon);
}

void
movecanvasmouse(const Arg *arg)
{
	int start_x, start_y, tagidx, dx, dy;
	Window dummy;
	int di;
	unsigned int dui;
	float multiplier = arg ? arg->f : 1.0f;
	float accum_x = 0.0f, accum_y = 0.0f;
	XEvent ev;
#if ZOOM
	float zoom_val = zoom_value();
#endif
#if LOCK_MOVE_RESIZE_REFRESH_RATE
	Time lasttime = 0;
#endif

	if (selmon->lt[selmon->sellt]->arrange != NULL)
		return;
	if (selmon->sel && selmon->sel->isfullscreen)
		return;

	tagidx = getcurrenttag(selmon);

	if (!XQueryPointer(dpy, root, &dummy, &dummy, &start_x, &start_y, &di, &di, &dui))
		return;

	if (XGrabPointer(dpy, root, False, MOUSEMASK, GrabModeAsync, GrabModeAsync,
		None, cursor[CurMove]->cursor, CurrentTime) != GrabSuccess)
		return;

	do {
		XMaskEvent(dpy, MOUSEMASK|ExposureMask|SubstructureRedirectMask, &ev);

		switch (ev.type) {
		case MotionNotify:
		{
#if LOCK_MOVE_RESIZE_REFRESH_RATE
			if ((ev.xmotion.time - lasttime) <= (Time)(1000 / refreshrate))
				continue;
			lasttime = ev.xmotion.time;
#endif
			int nx = ev.xmotion.x - start_x;
			int ny = ev.xmotion.y - start_y;

			/* Accumulate the subpixel remainder so fractional multipliers do
			 * not silently lose pixels: multiplier=0.5, nx=1 -> accum=0.5,
			 * dx=0 (skip); nx=1 again -> accum=1.0, dx=1 (move). */
#if ZOOM
			accum_x += nx * multiplier / zoom_val;
			accum_y += ny * multiplier / zoom_val;
#else
			accum_x += nx * multiplier;
			accum_y += ny * multiplier;
#endif

			dx = (int)accum_x;
			dy = (int)accum_y;

			accum_x -= dx;
			accum_y -= dy;

			for (Client *c = selmon->clients; c; c = c->next) {
				if ((c->tags & (1 << tagidx)) && !c->is_pinned) {
					c->x += dx;
					c->y += dy;
					XMoveWindow(dpy, c->win, c->x, c->y);
				}
			}

			selmon->canvas[tagidx].cx += dx;
			selmon->canvas[tagidx].cy += dy;
			drawbar(selmon);
			start_x = ev.xmotion.x;
			start_y = ev.xmotion.y;
		}	break;
		}
	} while (ev.type != ButtonRelease);

	XUngrabPointer(dpy, CurrentTime);
}

void
save_canvas_positions(Monitor *m)
{
	Client *c;
	int tagidx = getcurrenttag(m);

	m->canvas[tagidx].saved_cx = m->canvas[tagidx].cx;
	m->canvas[tagidx].saved_cy = m->canvas[tagidx].cy;

	for (c = m->clients; c; c = c->next) {
		if (ISVISIBLE(c)) {
			c->saved_cx = c->x + m->canvas[tagidx].cx;
			c->saved_cy = c->y + m->canvas[tagidx].cy;
			c->saved_cw = c->w;
			c->saved_ch = c->h;
			c->was_on_canvas = 1;
		}
	}
}

void
restore_canvas_positions(Monitor *m)
{
	Client *c;
	int tagidx = getcurrenttag(m);

	m->canvas[tagidx].cx = m->canvas[tagidx].saved_cx;
	m->canvas[tagidx].cy = m->canvas[tagidx].saved_cy;

	for (c = m->clients; c; c = c->next) {
		if (ISVISIBLE(c) && c->was_on_canvas) {
			int target_x, target_y;

			if (c->isfullscreen)
				continue;
			c->isfloating = 1;

			target_x = c->saved_cx - m->canvas[tagidx].cx;
			target_y = c->saved_cy - m->canvas[tagidx].cy;

			c->x = target_x;
			c->y = target_y;
			c->w = c->saved_cw;
			c->h = c->saved_ch;

			XMoveResizeWindow(dpy, c->win, target_x, target_y, c->w, c->h);

			configure(c);
		}
	}
	XSync(dpy, False);
}

void
centerwindow(const Arg *arg)
{
	Client *c = (arg && arg->v) ? (Client *)arg->v : selmon->sel;
	Client *tmp;
	Monitor *m;
	int tagidx, dx, dy;

	if (!c || !c->mon || c->mon->lt[c->mon->sellt]->arrange != NULL)
		return;

	if (c->is_pinned)
		return;

	m = c->mon;
	tagidx = getcurrenttag(m);

	dx = m->wx + (m->ww - WIDTH(c)) / 2 - c->x;
	dy = m->wy + (m->wh / 2) - (c->y + HEIGHT(c) / 2);

	if (dx == 0 && dy == 0)
		return;

	for (tmp = m->clients; tmp; tmp = tmp->next) {
		if (ISVISIBLE(tmp) && !tmp->is_pinned) {
			tmp->x += dx;
			tmp->y += dy;
			XMoveWindow(dpy, tmp->win, tmp->x, tmp->y);
		}
	}

	m->canvas[tagidx].cx += dx;
	m->canvas[tagidx].cy += dy;

	drawbar(m);
}

void
pinwindow(const Arg *arg)
{
	Client *c = selmon->sel;

	(void)arg;
	if (!c)
		return;
	c->is_pinned = !c->is_pinned;
	restack(selmon);
	drawbar(selmon);
}

#endif /* INFINITE_TAGS */

/* Keep this translation unit non-empty when the module is disabled
 * (ISO C forbids an empty translation unit). */
typedef int vxwm_infinitetags_module_present;
