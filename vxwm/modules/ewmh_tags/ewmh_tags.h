#pragma once

/* ewmh_tags - publish the tag list to EWMH-aware pagers.
 *
 * Depends on:  EWMH_TAGS
 * Hooks used:  core setup() calls ewmh_tags_init(); core calls
 *              ewmh_setviewport() on every view change (view, toggleview,
 *              focusmon) and ewmh_setclientdesktop() when adopting a client.
 *
 * The tag list is reported as _NET_DESKTOP_NAMES (one entry per tag, NUL
 * separated), so pagers that show one viewport per entry display each tag.
 * _NET_NUMBER_OF_DESKTOPS, _NET_CURRENT_DESKTOP and _NET_DESKTOP_VIEWPORT
 * follow the focused monitor's tagset, and _NET_WM_DESKTOP is set per client
 * to its first set tag.
 */

void ewmh_tags_init(void);
void ewmh_setviewport(void);
void ewmh_setclientdesktop(Client *c);
