#include "vxwm.h"
#include "modules/fullscreen/fullscreen.h"

#if FULLSCREEN

void
fullscreen(const Arg *arg)
{
	Client *c = selmon->sel;

	(void)arg;
	if (!c)
		return;

	if (c->isfullscreen) {
		resize(c, c->oldx, c->oldy, c->oldw, c->oldh, 0);
		c->isfullscreen = 0;
		arrange(c->mon);
	} else {
		c->oldx = c->x;
		c->oldy = c->y;
		c->oldw = c->w;
		c->oldh = c->h;
		resize(c, c->mon->mx, c->mon->my, c->mon->mw, c->mon->mh, 0);
		c->isfullscreen = 1;
		XRaiseWindow(dpy, c->win);
	}
}

#endif /* FULLSCREEN */

/* Keep this translation unit non-empty when FULLSCREEN is disabled
 * (ISO C forbids an empty translation unit). */
typedef int vxwm_fullscreen_module_present;
