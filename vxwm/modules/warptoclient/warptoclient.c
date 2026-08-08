#include "vxwm.h"
#include "modules/warptoclient/warptoclient.h"

#if WARP_TO_CLIENT

void
warptoclient(Client *c)
{
	int x, y;

	if (!c) {
		XWarpPointer(dpy, None, root, 0, 0, 0, 0,
			selmon->wx + selmon->ww / 2, selmon->wy + selmon->wh / 2);
		return;
	}

	x = c->x + WIDTH(c) / 2;
	y = c->y + HEIGHT(c) / 2;

	XWarpPointer(dpy, None, root, 0, 0, 0, 0, x, y);
}

#endif /* WARP_TO_CLIENT */

/* Keep this translation unit non-empty when the module is disabled
 * (ISO C forbids an empty translation unit). */
typedef int vxwm_warptoclient_module_present;
