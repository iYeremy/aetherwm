#pragma once

/* directionalmove - swap the focused tiled client with the closest neighbour.
 *
 * Depends on:  DIRECTIONAL_MOVE
 * Hooks used:  none in the core; bind movedir() in config.def.h keys[], and
 *              the moveresizekbd arrows call movedir() on tiled clients.
 *
 * movedir() scores the visible clients in the requested direction (axial
 * distance weighted 20x against lateral distance, wrapping around the monitor
 * edges) and swaps the focused client with the best match using the core
 * swaptile() helper, then re-arranges. Only active in layouts that arrange
 * their windows (tiled/monocle), matching the original `!= tile` intent but
 * working with gaps/monocle layouts too.
 */

void movedir(const Arg *arg);
