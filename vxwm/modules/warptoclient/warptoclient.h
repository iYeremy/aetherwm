#pragma once

/* warptoclient - warp the pointer to the centre of a client.
 *
 * Depends on:  WARP_TO_CLIENT (each individual WARP_TO_* hook is its own
 *              switch and is inert while WARP_TO_CLIENT is off).
 * Hooks used:  the individual WARP_TO_*_OF_* switches gate calls from the
 *              core/modules; warptoclient() with a NULL client warps to the
 *              centre of the current monitor's work area.
 */

void warptoclient(Client *c);
