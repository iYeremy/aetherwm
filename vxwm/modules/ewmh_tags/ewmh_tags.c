#include <string.h>

#include "vxwm.h"
#include "modules/ewmh_tags/ewmh_tags.h"

#if EWMH_TAGS

static Atom net_desktop_names;
static Atom net_number_of_desktops;
static Atom net_desktop_viewport;
static Atom net_current_desktop;
static Atom net_wm_desktop;
static Atom utf8_string;

/* Index of the lowest set tag in `mask` (or 0 if none). */
static unsigned long
lowest_tag(unsigned int mask)
{
	unsigned long i;

	for (i = 0; i < ntags; i++)
		if (mask & 1UL << i)
			return i;
	return 0;
}

/* Publish the tag list as _NET_DESKTOP_NAMES (one NUL-separated entry per
 * tag) plus the number/current/viewport properties so EWMH-aware pagers can
 * show one viewport per tag. */
void
ewmh_setviewport(void)
{
	long *vp, current, ndesktops;
	size_t len, i, off;
	char *names;

	if (!ntags)
		return;

	len = 0;
	for (i = 0; i < ntags; i++)
		len += strlen(tags[i]) + 1;
	names = ecalloc(len ? len : 1, 1);
	for (i = 0, off = 0; i < ntags; i++) {
		strcpy(names + off, tags[i]);
		off += strlen(tags[i]) + 1;
	}

	vp = ecalloc(ntags * 2, sizeof *vp);
	current = (long)lowest_tag(selmon->tagset[selmon->seltags]);
	ndesktops = (long)ntags;

	XChangeProperty(dpy, root, net_number_of_desktops, XA_CARDINAL, 32,
			PropModeReplace, (unsigned char *)&ndesktops, 1);
	XChangeProperty(dpy, root, net_current_desktop, XA_CARDINAL, 32,
			PropModeReplace, (unsigned char *)&current, 1);
	XChangeProperty(dpy, root, net_desktop_viewport, XA_CARDINAL, 32,
			PropModeReplace, (unsigned char *)vp, (int)(ntags * 2));
	XChangeProperty(dpy, root, net_desktop_names, utf8_string, 8,
			PropModeReplace, (unsigned char *)names, (int)len);

	free(names);
	free(vp);
}

/* Tell a client which desktop it lives on (its first set tag). */
void
ewmh_setclientdesktop(Client *c)
{
	long desktop = (long)lowest_tag(c->tags);

	XChangeProperty(dpy, c->win, net_wm_desktop, XA_CARDINAL, 32,
			PropModeReplace, (unsigned char *)&desktop, 1);
}

void
ewmh_tags_init(void)
{
	net_desktop_names = XInternAtom(dpy, "_NET_DESKTOP_NAMES", False);
	net_number_of_desktops = XInternAtom(dpy, "_NET_NUMBER_OF_DESKTOPS", False);
	net_desktop_viewport = XInternAtom(dpy, "_NET_DESKTOP_VIEWPORT", False);
	net_current_desktop = XInternAtom(dpy, "_NET_CURRENT_DESKTOP", False);
	net_wm_desktop = XInternAtom(dpy, "_NET_WM_DESKTOP", False);
	utf8_string = XInternAtom(dpy, "UTF8_STRING", False);
	ewmh_setviewport();
}

#endif /* EWMH_TAGS */

/* Keep this translation unit non-empty when EWMH_TAGS is disabled
 * (ISO C forbids an empty translation unit). */
typedef int vxwm_ewmh_tags_module_present;
