#pragma once

/* directionalfocus - focus the client closest in a given direction.
 *
 * Depends on:  DIRECTIONAL_FOCUS
 * Hooks used:  none in the core; bind focusdir() in config.def.h keys[].
 *
 * focusdir() scores the visible clients in the requested direction and focuses
 * the best match. In tiled layouts it uses axial distance weighted 20x against
 * lateral distance (dwm dwindle style); in the floating layout (INFINITE_TAGS)
 * it uses the axial distance plus the lateral distance squared over axial+1 so
 * directly-aligned windows win. Floating clients are only matched against
 * floating clients, tiled against tiled.
 */

void focusdir(const Arg *arg);
