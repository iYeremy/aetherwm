#include "vxwm.h"
#include "modules/etf/etf.h"

#if ENHANCED_TOGGLE_FLOATING || FLOATING_LAYOUT_FLOATS_WINDOWS

#if ENHANCED_TOGGLE_FLOATING
void
togglefloat(const Arg *arg)
{
	Client *c;

	(void)arg;
	if (!(c = selmon->sel) || c->nfixed)
		return;
	if (c->isfloating) {
		c->isfloating = 0;
		c->wasfloating = 0;
#if RESTORE_SIZE_AND_POS_ETF
		/* restore the tiled geometry unless the user moved/resized it */
		if (!c->wasmanuallyedited)
			resize(c, c->sfx, c->sfy, c->sfw, c->sfh, 1);
#endif
	} else {
		c->sfx = c->x;
		c->sfy = c->y;
		c->sfw = c->w;
		c->sfh = c->h;
		c->isfloating = 1;
		c->wasfloating = 1;
#if RESTORE_SIZE_AND_POS_ETF
		c->wasmanuallyedited = 0;
#endif
	}
	arrange(selmon);
}
#endif /* ENHANCED_TOGGLE_FLOATING */

#endif /* ENHANCED_TOGGLE_FLOATING || FLOATING_LAYOUT_FLOATS_WINDOWS */

/* Keep this translation unit non-empty when the module is disabled
 * (ISO C forbids an empty translation unit). */
typedef int vxwm_etf_module_present;
