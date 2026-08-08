#include <stdlib.h>

#include "vxwm.h"
#include "modules/betterresize/betterresize.h"

#if BETTER_RESIZE

void
betterresize_resizemouse(const Arg *arg)
{
	Client *c;
	Monitor *m;
	XEvent ev;
	int orig_x, orig_y, orig_w, orig_h;
	int rx, ry, dx, dy, nx, ny, nw, nh, min_w, min_h, dx_final, dy_final;
	Window junkwin;
	int junk_signed;
	unsigned int junk;
	int left, right, top, bottom;
#if BR_CHANGE_CURSOR
	Cursor cur;
#endif
#if LOCK_MOVE_RESIZE_REFRESH_RATE
	Time lasttime = 0;
#endif

	(void)arg;
	if (!(c = selmon->sel) || c->isfullscreen)
		return;

	restack(selmon);

	orig_x = c->x;
	orig_y = c->y;
	orig_w = c->w;
	orig_h = c->h;

	if (!XQueryPointer(dpy, c->win, &junkwin, &junkwin, &junk_signed, &junk_signed, &rx, &ry, &junk))
		return;
	left   = rx < orig_w / 3;
	right  = rx > orig_w * 2 / 3;
	top    = ry < orig_h / 3;
	bottom = ry > orig_h * 2 / 3;
#if BR_CHANGE_CURSOR
	if (top && left)          cur = cursor[CurNW]->cursor;
	else if (top && right)    cur = cursor[CurNE]->cursor;
	else if (bottom && left)  cur = cursor[CurSW]->cursor;
	else if (bottom && right) cur = cursor[CurSE]->cursor;
	else if (top)             cur = cursor[CurN]->cursor;
	else if (bottom)          cur = cursor[CurS]->cursor;
	else if (left)            cur = cursor[CurW]->cursor;
	else if (right)           cur = cursor[CurE]->cursor;
	else                      cur = cursor[CurResize]->cursor; /* fallback */
#else
	Cursor cur = cursor[CurResize]->cursor;
#endif
	if (XGrabPointer(dpy, root, False, MOUSEMASK, GrabModeAsync, GrabModeAsync,
		None, cur, CurrentTime) != GrabSuccess)
		return;
	do {
		XMaskEvent(dpy, MOUSEMASK|ExposureMask|SubstructureRedirectMask, &ev);

		if (ev.type == MotionNotify) {
#if LOCK_MOVE_RESIZE_REFRESH_RATE
			if (ev.xmotion.time - lasttime <= (Time)(1000 / refreshrate))
				continue;
			lasttime = ev.xmotion.time;
#endif
			dx = ev.xmotion.x_root - (orig_x + rx);
			dy = ev.xmotion.y_root - (orig_y + ry);

			nx = orig_x;
			ny = orig_y;
			nw = orig_w;
			nh = orig_h;

			if (left)        nw = orig_w - dx;
			else if (right)  nw = orig_w + dx;

			if (top)         nh = orig_h - dy;
			else if (bottom) nh = orig_h + dy;

			min_w = MAX(1, c->minw);
			min_h = MAX(1, c->minh);

			if (nw < min_w) nw = min_w;
			if (nh < min_h) nh = min_h;

			if (left) nx = orig_x + (orig_w - nw);
			if (top)  ny = orig_y + (orig_h - nh);

			dx_final = nw - orig_w;
			dy_final = nh - orig_h;
#if RESIZING_WINDOWS_IN_ALL_LAYOUTS_FLOATS_THEM
			if (!c->isfloating && (abs(dx_final) > snap || abs(dy_final) > snap)) {
#else
			if (!c->isfloating && selmon->lt[selmon->sellt]->arrange &&
			  (abs(dx_final) > snap || abs(dy_final) > snap)) {
#endif
				if (nx >= selmon->wx && nx + nw <= selmon->wx + selmon->ww &&
					ny >= selmon->wy && ny + nh <= selmon->wy + selmon->wh) {

					togglefloating(NULL);

					orig_x = c->x;
					orig_y = c->y;
					orig_w = c->w;
					orig_h = c->h;
				}
			}
#if USE_RESIZECLIENT_FUNC
			resizeclient(c, nx, ny, nw, nh);
#else
			resize(c, nx, ny, nw, nh, 1);
#endif
			drawbar(selmon);
		}
	} while (ev.type != ButtonRelease);

	XUngrabPointer(dpy, CurrentTime);
	while (XCheckMaskEvent(dpy, EnterWindowMask, &ev));

	if ((m = recttomon(c->x, c->y, c->w, c->h)) != selmon) {
		sendmon(c, m);
		selmon = m;
		focus(NULL);
	}
#if ENHANCED_TOGGLE_FLOATING && RESTORE_SIZE_AND_POS_ETF
	c->wasmanuallyedited = 1;
	if (c->isfloating) {
		c->sfx = c->x;
		c->sfy = c->y;
		c->sfw = c->w;
		c->sfh = c->h;
	}
#endif
}

#endif /* BETTER_RESIZE */

/* Keep this translation unit non-empty when the module is disabled
 * (ISO C forbids an empty translation unit). */
typedef int vxwm_betterresize_module_present;
