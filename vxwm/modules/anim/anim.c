#include <stdlib.h>
#include <time.h>

#include "vxwm.h"
#include "modules/anim/anim.h"

#if SMOOTH_RESIZE

struct Anim {
	int cx, cy, cw, ch;  /* current on-screen geometry (follows the tween) */
	int tx, ty, tw, th;  /* target geometry                                 */
	double start;        /* CLOCK_MONOTONIC seconds at the start of the tween */
	double dur;          /* animation duration in seconds                    */
};

static double
anim_now(void)
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
}

/* ease-out cubic: quick start, gentle landing */
static double
anim_ease(double t)
{
	double u = 1.0 - t;

	return 1.0 - u * u * u;
}

/* Put the X window at its target geometry immediately (no animation). */
static void
anim_apply_instant(Client *c, int x, int y, int w, int h)
{
	XWindowChanges wc;

	wc.x = x;
	wc.y = y;
	wc.width = w;
	wc.height = h;
	wc.border_width = c->bw;
	XConfigureWindow(dpy, c->win, CWX|CWY|CWWidth|CWHeight|CWBorderWidth, &wc);
	configure(c);
	XSync(dpy, False);
}

/* Advance a running animation one frame and free it when it lands. */
static void
anim_apply(Client *c)
{
	struct Anim *a = c->anim;
	XWindowChanges wc;
	double t, k;
	int x, y, w, h;

	t = (anim_now() - a->start) / a->dur;
	if (t < 0.0)
		t = 0.0;
	if (t >= 1.0) {
		x = a->tx;
		y = a->ty;
		w = a->tw;
		h = a->th;
	} else {
		k = anim_ease(t);
		x = a->cx + (int)((a->tx - a->cx) * k);
		y = a->cy + (int)((a->ty - a->cy) * k);
		w = a->cw + (int)((a->tw - a->cw) * k);
		h = a->ch + (int)((a->th - a->ch) * k);
	}
	wc.x = x;
	wc.y = y;
	wc.width = w;
	wc.height = h;
	wc.border_width = c->bw;
	XConfigureWindow(dpy, c->win, CWX|CWY|CWWidth|CWHeight|CWBorderWidth, &wc);
	XSync(dpy, False);
	a->cx = x;
	a->cy = y;
	a->cw = w;
	a->ch = h;
	if (t >= 1.0) {
		configure(c); /* tell the client its final geometry */
		free(a);
		c->anim = NULL;
	}
}

void
anim_resizeclient(Client *c, int x, int y, int w, int h)
{
	struct Anim *a = c->anim;

	if (!a) {
		/* moving only (size unchanged, e.g. dragging a floating window):
		 * keep it instant so moving stays perfectly responsive */
		if (w == c->oldw && h == c->oldh) {
			anim_apply_instant(c, x, y, w, h);
			return;
		}
		if (!(a = c->anim = calloc(1, sizeof(*a)))) {
			anim_apply_instant(c, x, y, w, h); /* no memory: just snap */
			return;
		}
		/* the window currently sits at its pre-resize geometry */
		a->cx = c->oldx;
		a->cy = c->oldy;
		a->cw = c->oldw;
		a->ch = c->oldh;
	} else if (a->tx == x && a->ty == y && a->tw == w && a->th == h) {
		return; /* already animating towards this exact target */
	}
	a->tx = x;
	a->ty = y;
	a->tw = w;
	a->th = h;
	a->start = anim_now();
	a->dur = (double)ANIM_DURATION_MS / 1000.0;
	anim_apply(c); /* render the first frame right away */
}

void
anim_tick(void)
{
	Client *c;
	Monitor *m;

	for (m = mons; m; m = m->next)
		for (c = m->clients; c; c = c->next)
			if (c->anim)
				anim_apply(c);
}

void
anim_cancel(Client *c)
{
	if (c && c->anim) {
		free(c->anim);
		c->anim = NULL;
	}
}

#endif /* SMOOTH_RESIZE */

/* Keep this translation unit non-empty when the module is disabled
 * (ISO C forbids an empty translation unit). */
typedef int vxwm_anim_module_present;
