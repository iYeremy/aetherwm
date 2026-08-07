#include <string.h>
#include <X11/Xresource.h>

#include "vxwm.h"
#include "modules/xrdb/xrdb.h"

#if XRDB

/* Map the 16-entry xterm palette onto the two core schemes. The inner arrays
 * are ordered fg, bg, border to match ColFg/ColBg/ColBorder. */
static const char *resmap[SchemeSel + 1][3] = {
	[SchemeNorm] = { "color7",  "color0",  "color8"  },
	[SchemeSel]  = { "color15", "color4",  "color6"  },
};

static int
loadcolor(XrmDatabase db, const char *name, char *dst, size_t size)
{
	XrmValue value;
	char *type = NULL;

	if (XrmGetResource(db, name, "Color", &type, &value) != True
	|| !value.addr || !*value.addr)
		return 0;
	strncpy(dst, value.addr, size - 1);
	dst[size - 1] = '\0';
	return 1;
}

/* Reload the xterm palette (color0..color15) from the display's resource
 * database. Schemes whose palette entries are missing keep the config
 * defaults. */
void
loadxrdb(const Arg *arg)
{
	XrmDatabase db;
	unsigned int i, j, ok;
	char buf[SchemeSel + 1][3][8];
	char *names[3];

	(void)arg;
	XrmInitialize();
	if (!(db = XrmGetDatabase(dpy)))
		return;

	for (i = SchemeNorm; i <= SchemeSel; i++) {
		ok = 1;
		for (j = 0; j < 3; j++) {
			if (!loadcolor(db, resmap[i][j], buf[i][j], sizeof buf[i][j])) {
				ok = 0;
				break;
			}
			names[j] = buf[i][j];
		}
		if (!ok)
			continue; /* keep the config default scheme */
		drw_scm_free(drw, scheme[i], 3);
		scheme[i] = drw_scm_create(drw, names, 3);
	}
	drawbars();
}

#endif /* XRDB */

/* Keep this translation unit non-empty when XRDB is disabled
 * (ISO C forbids an empty translation unit). */
typedef int vxwm_xrdb_module_present;
