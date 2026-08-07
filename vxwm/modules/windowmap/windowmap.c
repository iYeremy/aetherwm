#include "vxwm.h"
#include "modules/windowmap/windowmap.h"

#if WINDOWMAP

void
window_set_state(Client *c, long state)
{
	long data[] = { state, None };

	XChangeProperty(dpy, c->win, wmatom[WMState], wmatom[WMState], 32,
		PropModeReplace, (unsigned char *)data, 2);
}

void
window_map(Client *c, int deiconify)
{
	if (deiconify)
		window_set_state(c, NormalState);

	XMoveResizeWindow(dpy, c->win, c->x, c->y, c->w, c->h);
	XMapWindow(dpy, c->win);
	c->ismapped = 1;
}

void
window_unmap(Client *c, int iconify)
{
	XWindowAttributes ca, ra;

	XGetWindowAttributes(dpy, root, &ra);
	XGetWindowAttributes(dpy, c->win, &ca);

	/* Prevent the UnmapNotify events this unmap would trigger */
	XSelectInput(dpy, root, ra.your_event_mask & ~SubstructureNotifyMask);
	XSelectInput(dpy, c->win, ca.your_event_mask & ~StructureNotifyMask);

	XUnmapWindow(dpy, c->win);
	c->ismapped = 0;
	if (iconify)
		window_set_state(c, IconicState);

	XSelectInput(dpy, root, ra.your_event_mask);
	XSelectInput(dpy, c->win, ca.your_event_mask);
}

#endif /* WINDOWMAP */

/* Keep this translation unit non-empty when the module is disabled
 * (ISO C forbids an empty translation unit). */
typedef int vxwm_windowmap_module_present;
