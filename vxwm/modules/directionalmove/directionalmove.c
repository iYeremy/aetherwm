#include <stdlib.h>

#include "vxwm.h"
#include "modules/directionalmove/directionalmove.h"
#if WARP_TO_CLIENT && WARP_TO_CENTER_OF_WINDOW_AFFECTED_BY_FOCUSSTACK
#include "modules/warptoclient/warptoclient.h"
#endif

#if DIRECTIONAL_MOVE

void
movedir(const Arg *arg)
{
	Client *s = selmon->sel, *f = NULL, *c, *next;
	unsigned int score = -1;
	unsigned int client_score;
	int dist;
	int dirweight = 20;
	int isfloating;

	if (!s)
		return;
	if (!selmon->lt[selmon->sellt]->arrange)
		return;

	isfloating = s->isfloating;
	next = s->next;
	if (!next)
		next = s->mon->clients;
	for (c = next; c != s; c = next) {
		next = c->next;
		if (!next)
			next = s->mon->clients;
		if (!ISVISIBLE(c) || c->isfloating != isfloating)
			continue;
		switch (arg->i) {
		case 0: /* left */
			dist = s->x - c->x - c->w;
			client_score =
				dirweight * MIN(abs(dist), abs(dist + s->mon->ww)) +
				abs(s->y - c->y);
			break;
		case 1: /* right */
			dist = c->x - s->x - s->w;
			client_score =
				dirweight * MIN(abs(dist), abs(dist + s->mon->ww)) +
				abs(c->y - s->y);
			break;
		case 2: /* up */
			dist = s->y - c->y - c->h;
			client_score =
				dirweight * MIN(abs(dist), abs(dist + s->mon->wh)) +
				abs(s->x - c->x);
			break;
		default:
		case 3: /* down */
			dist = c->y - s->y - s->h;
			client_score =
				dirweight * MIN(abs(dist), abs(dist + s->mon->wh)) +
				abs(c->x - s->x);
			break;
		}
		if (((arg->i == 0 || arg->i == 2) && client_score <= score) || client_score < score) {
			score = client_score;
			f = c;
		}
	}
	if (!f || f == s)
		return;

	swaptile(s, f);

	focus(s);
#if WARP_TO_CLIENT && WARP_TO_CENTER_OF_WINDOW_AFFECTED_BY_FOCUSSTACK
	warptoclient(f);
#endif
	arrange(s->mon);
}

#endif /* DIRECTIONAL_MOVE */

/* Keep this translation unit non-empty when the module is disabled
 * (ISO C forbids an empty translation unit). */
typedef int vxwm_directionalmove_module_present;
