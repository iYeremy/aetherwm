#pragma once

/* externalbars - reserve screen area for external bars.
 *
 * Depends on:  EXTERNAL_BARS
 * Adds to Monitor: int strut_top, strut_bottom, strut_left, strut_right;
 * Adds to the Net atom enum: NetWMStrut, NetWMStrutPartial.
 * Hooks used:  core propertynotify() calls externalbars_update() whenever an
 *              external bar's _NET_WM_STRUT / _NET_WM_STRUT_PARTIAL property
 *              is changed or deleted; core updatebarpos() shrinks the work
 *              area by the per-monitor struts.
 *
 * External bar programs (polybar, lemonbar, ...) that are managed as regular
 * windows set one of these properties; dwm then keeps its tiled/floating
 * windows out of the reserved strip so the bar is never covered.
 */

void externalbars_update(Client *c);
