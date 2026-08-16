#pragma once

/* bsp - balanced binary-space-partition tiling layout.
 *
 * Every tiled window lives on a leaf of a binary split tree. A new window
 * splits the leaf that is under the mouse pointer (falling back to the
 * focused window), so the layout never collapses onto a single side and
 * four quadrants are reached naturally as the split direction alternates
 * between vertical and horizontal lines according to the aspect ratio of
 * the region being split.
 *
 * Depends on:  BSP_LAYOUT
 * Adds to Client: bspnode (the leaf node holding the client).
 * Adds to Monitor: bsproot (root of the split tree).
 * Core hooks used:
 *   - swaptile()  routes through bsp_swap() when both clients are in the
 *                 tree, so mouse/keyboard moves reorder the tiling without
 *                 ever making the window floating.
 *   - setmfact()  adjusts the split ratio of the focused window's container.
 *   - unmanage()  calls bsp_unmanage() before the Client is freed so the
 *                 tree never keeps a dangling pointer.
 *
 * The tree is kept per monitor across tags and layouts; subtrees whose
 * windows are not visible collapse during arrangement so visible windows
 * always fill the screen. Floating and fullscreen windows are removed from
 * the tree and re-inserted when they become tiled again.
 */

struct BspNode;
typedef struct BspNode BspNode;

void bsp_arrange(Monitor *m);
void bsp_swap(Client *a, Client *b);
void bsp_setmfact(const Arg *arg);
void bsp_resizemouse(Client *c, int dx, int dy,
	int left, int right, int top, int bottom);
void bsp_unmanage(Client *c);
