#include "vxwm.h"
#include "modules/bsp/bsp.h"

#if BSP_LAYOUT

enum { BSP_SPLIT_V, BSP_SPLIT_H };

struct BspNode {
	struct BspNode *parent;
	struct BspNode *left, *right;
	Client *c;
	int dir;    /* split direction for containers (BSP_SPLIT_V / BSP_SPLIT_H) */
	float ratio; /* split point inside the container (0.2 .. 0.8) */
	int x, y, w, h; /* last assigned geometry (root coordinates) */
};

static int
bsp_isleaf(BspNode *n)
{
	return n && !n->left && !n->right;
}

/* Does this subtree contain a window that is currently visible and tiled? */
static int
bsp_visible(BspNode *n, Monitor *m)
{
	if (!n)
		return 0;
	if (bsp_isleaf(n))
		return n->c && ISVISIBLE(n->c) && !n->c->isfloating
			&& !n->c->isfullscreen && n->c->mon == m;
	return bsp_visible(n->left, m) || bsp_visible(n->right, m);
}

static BspNode *
bsp_newleaf(Client *c)
{
	BspNode *n = ecalloc(1, sizeof(BspNode));

	n->c = c;
	n->dir = -1;
	n->ratio = 0.5f;
	return n;
}

/* Swap `node` with `rep` in the tree (handles the root case). */
static void
bsp_replace(Monitor *m, BspNode *node, BspNode *rep)
{
	BspNode *p = node->parent;

	rep->parent = p;
	if (!p) {
		m->bsproot = rep;
		return;
	}
	if (p->left == node)
		p->left = rep;
	else
		p->right = rep;
}

/* Remove a leaf from the tree, collapsing its container into the sibling. */
static void
bsp_remove_leaf(Monitor *m, BspNode *leaf)
{
	BspNode *p, *sib;

	if (!leaf)
		return;
	if (leaf->c && leaf->c->bspnode == leaf)
		leaf->c->bspnode = NULL;
	if (!leaf->parent) {
		m->bsproot = NULL;
		free(leaf);
		return;
	}
	p = leaf->parent;
	sib = (p->left == leaf) ? p->right : p->left;
	bsp_replace(m, p, sib);
	if (sib->c)
		sib->c->bspnode = sib;
	free(p);
	free(leaf);
}

static int
bsp_contains(BspNode *n, int x, int y)
{
	return n && x >= n->x && x < n->x + n->w && y >= n->y && y < n->y + n->h;
}

/* The visible leaf whose rectangle contains (x, y), if any. */
static BspNode *
bsp_leafat(BspNode *n, int x, int y, Monitor *m)
{
	BspNode *res;

	if (!n)
		return NULL;
	if (bsp_isleaf(n))
		return bsp_visible(n, m) && bsp_contains(n, x, y) ? n : NULL;
	if ((res = bsp_leafat(n->left, x, y, m)))
		return res;
	return bsp_leafat(n->right, x, y, m);
}

/* The first visible leaf in the tree (leftmost walk). */
static BspNode *
bsp_visibleleaf(BspNode *n, Monitor *m)
{
	if (!n)
		return NULL;
	if (bsp_isleaf(n))
		return bsp_visible(n, m) ? n : NULL;
	{
		BspNode *r = bsp_visibleleaf(n->left, m);
		return r ? r : bsp_visibleleaf(n->right, m);
	}
}

/* Insert a new client as a leaf, splitting the leaf under the pointer. */
static void
bsp_insert(Monitor *m, Client *c)
{
	BspNode *leaf, *container, *newleaf;
	int mx = 0, my = 0;

	if (!m->bsproot) {
		m->bsproot = bsp_newleaf(c);
		c->bspnode = m->bsproot;
		return;
	}

	leaf = NULL;
	if (getrootptr(&mx, &my))
		leaf = bsp_leafat(m->bsproot, mx, my, m);
	if (!leaf && m->sel && m->sel->bspnode)
		leaf = m->sel->bspnode;
	if (!leaf)
		leaf = bsp_visibleleaf(m->bsproot, m);
	if (!leaf) {
		leaf = m->bsproot;
		while (leaf && !bsp_isleaf(leaf))
			leaf = leaf->left;
	}
	if (!leaf)
		return;

	container = ecalloc(1, sizeof(BspNode));
	newleaf = bsp_newleaf(c);
	container->dir = (leaf->w >= leaf->h) ? BSP_SPLIT_V : BSP_SPLIT_H;
	container->ratio = 0.5f;
	/* replace the leaf with the new container in the parent's child slot,
	 * then link the old leaf and the new leaf under the container */
	bsp_replace(m, leaf, container);
	container->left = leaf;
	container->right = newleaf;
	leaf->parent = container;
	newleaf->parent = container;
	c->bspnode = newleaf;
}

/* Count leaves that must be removed (dead/floating/other-monitor clients). */
static int
bsp_count_removals(BspNode *n, Monitor *m)
{
	if (!n)
		return 0;
	if (bsp_isleaf(n))
		return (!n->c || n->c->mon != m || n->c->isfloating || n->c->isfullscreen) ? 1 : 0;
	return bsp_count_removals(n->left, m) + bsp_count_removals(n->right, m);
}

static void
bsp_fill_removals(BspNode *n, Monitor *m, BspNode **list, int *idx)
{
	if (!n)
		return;
	if (bsp_isleaf(n)) {
		if (!n->c || n->c->mon != m || n->c->isfloating || n->c->isfullscreen)
			list[(*idx)++] = n;
		return;
	}
	bsp_fill_removals(n->left, m, list, idx);
	bsp_fill_removals(n->right, m, list, idx);
}

static void
bsp_prune(Monitor *m)
{
	BspNode **list;
	int count, idx = 0, i;

	count = bsp_count_removals(m->bsproot, m);
	if (!count)
		return;
	list = ecalloc(count, sizeof(BspNode *));
	bsp_fill_removals(m->bsproot, m, list, &idx);
	for (i = 0; i < count; i++)
		bsp_remove_leaf(m, list[i]);
	free(list);
}

/* Assign geometry to the subtree. Containers whose sibling subtree has no
 * visible window collapse so visible windows always fill the screen. */
static void
bsp_layout(Monitor *m, BspNode *n, int x, int y, int w, int h)
{
	int g, lw, th;

	if (!n)
		return;
	n->x = x;
	n->y = y;
	n->w = w;
	n->h = h;
	if (bsp_isleaf(n))
		return;
	if (!bsp_visible(n->left, m)) {
		bsp_layout(m, n->right, x, y, w, h);
		return;
	}
	if (!bsp_visible(n->right, m)) {
		bsp_layout(m, n->left, x, y, w, h);
		return;
	}

	g = m->gappx;
	if (n->dir == BSP_SPLIT_V) {
		lw = (int)(w * n->ratio);
		if (lw < 1) lw = 1;
		if (lw > w - 1) lw = w - 1;
		if (w > 2 * g) {
			bsp_layout(m, n->left,  x, y, lw - g, h);
			bsp_layout(m, n->right, x + lw + g, y, w - lw - g, h);
		} else {
			bsp_layout(m, n->left,  x, y, lw, h);
			bsp_layout(m, n->right, x + lw, y, w - lw, h);
		}
	} else {
		th = (int)(h * n->ratio);
		if (th < 1) th = 1;
		if (th > h - 1) th = h - 1;
		if (h > 2 * g) {
			bsp_layout(m, n->left,  x, y, w, th - g);
			bsp_layout(m, n->right, x, y + th + g, w, h - th - g);
		} else {
			bsp_layout(m, n->left,  x, y, w, th);
			bsp_layout(m, n->right, x, y + th, w, h - th);
		}
	}
}

static void
bsp_apply(Monitor *m, BspNode *n)
{
	if (!n)
		return;
	if (bsp_isleaf(n)) {
		if (n->c && n->c->mon == m && ISVISIBLE(n->c)
		&& !n->c->isfloating && !n->c->isfullscreen)
			resize(n->c, n->x + n->c->bw, n->y + n->c->bw,
				n->w - 2 * n->c->bw, n->h - 2 * n->c->bw, 0);
		return;
	}
	bsp_apply(m, n->left);
	bsp_apply(m, n->right);
}

void
bsp_arrange(Monitor *m)
{
	Client *c;
	int g = m->gappx;
	int wx = m->wx + g, wy = m->wy + g;
	int ww = MAX(m->ww - 2 * g, 0), wh = MAX(m->wh - 2 * g, 0);

	bsp_prune(m);
	if (m->bsproot)
		bsp_layout(m, m->bsproot, wx, wy, ww, wh);

	/* insert every tiled window that does not have a leaf yet: the new
	 * window splits the leaf under the mouse (see bsp_insert) */
	for (c = m->clients; c; c = c->next) {
		if (c->mon == m && !c->isfloating && !c->isfullscreen && !c->bspnode) {
			bsp_insert(m, c);
			bsp_layout(m, m->bsproot, wx, wy, ww, wh);
		}
	}

	if (m->bsproot)
		bsp_apply(m, m->bsproot);
}

void
bsp_swap(Client *a, Client *b)
{
	BspNode *na, *nb;
	Client *tmp;

	if (!a || !b)
		return;
	na = a->bspnode;
	nb = b->bspnode;
	if (!na || !nb)
		return;
	tmp = na->c;
	na->c = nb->c;
	nb->c = tmp;
	a->bspnode = nb;
	b->bspnode = na;
}

void
bsp_setmfact(const Arg *arg)
{
	BspNode *n;
	float f;

	if (!selmon->sel || !selmon->sel->bspnode)
		return;
	n = selmon->sel->bspnode->parent;
	if (!n)
		return;
	f = arg->f < 1.0 ? arg->f + n->ratio : arg->f - 1.0;
	if (f < 0.2f || f > 0.8f)
		return;
	n->ratio = f;
	arrange(selmon);
}

void
bsp_unmanage(Client *c)
{
	if (!c || !c->bspnode)
		return;
	bsp_remove_leaf(c->mon, c->bspnode);
}

#endif /* BSP_LAYOUT */

/* Keep this translation unit non-empty when the module is disabled
 * (ISO C forbids an empty translation unit). */
typedef int vxwm_bsp_module_present;
