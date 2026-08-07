#pragma once

/* xrdb - load color schemes from the X resource database.
 *
 * Depends on:  XRDB  (also requires Xresource.h, enabled in config.mk)
 * Adds to Monitor: XrmDatabase xrdb; Clr **scheme;
 * Hooks used:  core createmon()/cleanupmon() manage the per-monitor xrdb
 *              databases; core drawbar()/drawbars()/arrange() call
 *              loadxrdb() when the XFTCOLOR resource is missing.
 *
 * loadxrdb() re-reads the resources from the DISPLAY and rebuilds the
 * scheme of every monitor, then re-renders everything.
 */

void loadxrdb(const Arg *arg);
