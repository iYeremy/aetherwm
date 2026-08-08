#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "vxwm.h"
#include "modules/zoom/zoom.h"

#if ZOOM

float
zoom_value(void)
{
	const char *display = getenv("DISPLAY");
	char path[64];
	float val = 1.0f;
	FILE *fp;

	if (!display)
		return 1.0f;

	snprintf(path, sizeof path, "/tmp/vcompmgr_%s.sock.zoom", display);

	fp = fopen(path, "r");
	if (!fp)
		return 1.0f;
	if (fscanf(fp, "%f", &val) != 1)
		val = 1.0f;
	fclose(fp);
	return val;
}

#endif /* ZOOM */

/* Keep this translation unit non-empty when the module is disabled
 * (ISO C forbids an empty translation unit). */
typedef int vxwm_zoom_module_present;
