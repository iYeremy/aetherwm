#include "vxwm.h"
#include "modules/gaps/gaps.h"

#if GAPS

void
setgaps(const Arg *arg)
{
	if ((arg->i == 0) || (selmon->gappx + arg->i < 0))
		selmon->gappx = 0;
	else
		selmon->gappx += arg->i;
	arrange(selmon);
}

void
gaps_tile(Monitor *m)
{
	unsigned int i, n;
	int h, mw, my, ty;
	Client *c;

	for (n = 0, c = nexttiled(m->clients); c; c = nexttiled(c->next), n++);
	if (n == 0)
		return;

	if (n > (unsigned int)m->nmaster)
		mw = m->nmaster ? m->ww * m->mfact : 0;
	else
		mw = m->ww;

	for (i = 0, my = ty = m->gappx, c = nexttiled(m->clients); c;
	     c = nexttiled(c->next), i++)
		if (i < (unsigned int)m->nmaster) {
			h = (m->wh - my) / (int)(MIN(n, (unsigned int)m->nmaster) - i) - m->gappx;
			resize(c, m->wx + m->gappx + c->bw, m->wy + my,
			    mw - 2 * c->bw - m->gappx, h - 2 * c->bw, 0);
			if (my + HEIGHT(c) + m->gappx < m->wh)
				my += HEIGHT(c) + m->gappx;
		} else {
			h = (m->wh - ty) / (int)(n - i) - m->gappx;
			resize(c, m->wx + mw + m->gappx + c->bw, m->wy + ty,
			    m->ww - mw - 2 * c->bw - 2 * m->gappx, h - 2 * c->bw, 0);
			if (ty + HEIGHT(c) + m->gappx < m->wh)
				ty += HEIGHT(c) + m->gappx;
		}
}

#endif /* GAPS */

/* Keep this translation unit non-empty when the module is disabled
 * (ISO C forbids an empty translation unit). */
typedef int vxwm_gaps_module_present;
