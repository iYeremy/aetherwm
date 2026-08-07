#include "vxwm.h"
#include "modules/externalbars/externalbars.h"

#if EXTERNAL_BARS

/* Interpret a 12-long _NET_WM_STRUT_PARTIAL array (the first four values are
 * the inset sizes, the remaining pairs give the partial edge extents in root
 * coordinates) and store the insets that intersect monitor `m`. */
static void
apply_struts(Monitor *m, long *s)
{
	long mx = m->mx, my = m->my;
	long sx = mx + m->mw, sy = my + m->mh;
	long left = 0, right = 0, top = 0, bottom = 0;

	if (s[0] > 0 && s[4] < sy && s[5] >= my)
		left = MAX(0, MIN(s[0], sx) - mx);
	if (s[1] > 0 && s[6] < sy && s[7] >= my)
		right = MAX(0, sx - MAX(mx, s[1]));
	if (s[2] > 0 && s[8] < sx && s[9] >= mx)
		top = MAX(0, MIN(s[2], sy) - my);
	if (s[3] > 0 && s[10] < sx && s[11] >= mx)
		bottom = MAX(0, sy - MAX(my, s[3]));

	m->strut_left = (int)left;
	m->strut_right = (int)right;
	m->strut_top = (int)top;
	m->strut_bottom = (int)bottom;
}

void
externalbars_update(Client *c)
{
	Atom actual;
	long *s = NULL, partial[12];
	unsigned long n, left;
	int format;

	if (!c)
		return;

	/* default: no reserved space */
	c->mon->strut_left = c->mon->strut_right = 0;
	c->mon->strut_top = c->mon->strut_bottom = 0;

	if (XGetWindowProperty(dpy, c->win, netatom[NetWMStrutPartial], 0L, 12L,
	    False, XA_CARDINAL, &actual, &format, &n, &left,
	    (unsigned char **)&s) == Success && n == 12 && format == 32 && s) {
		apply_struts(c->mon, s);
	} else if (s) {
		XFree(s);
		s = NULL;
	}

	if (!s) {
		if (XGetWindowProperty(dpy, c->win, netatom[NetWMStrut], 0L, 4L,
		    False, XA_CARDINAL, &actual, &format, &n, &left,
		    (unsigned char **)&s) == Success && n == 4 && format == 32 && s) {
			/* full-edge extents: the strut spans the whole screen */
			partial[0] = s[0]; partial[1] = s[1];
			partial[2] = s[2]; partial[3] = s[3];
			partial[4] = 0;    partial[5] = sh;
			partial[6] = 0;    partial[7] = sh;
			partial[8] = 0;    partial[9] = sw;
			partial[10] = 0;   partial[11] = sw;
			apply_struts(c->mon, partial);
		}
		if (s)
			XFree(s);
	}

	updatebarpos(c->mon);
	arrange(c->mon);
}

#endif /* EXTERNAL_BARS */

/* Keep this translation unit non-empty when the module is disabled
 * (ISO C forbids an empty translation unit). */
typedef int vxwm_externalbars_module_present;
