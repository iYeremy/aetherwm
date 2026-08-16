#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "vxwm.h"
#include "modules/autostart/autostart.h"

#if AUTOSTART

/* Shell commands run at startup, in order. Each entry is executed with
 * `sh -c <cmd>`. Skip this entirely with `vxwm -ignoreautostart`.
 *
 * Add entries here to launch daemons/tray apps at startup, e.g. "nm-applet"
 * or "picom". */
static const char *const autostart[] = {
	"$HOME/touchpad.sh", /* tap-to-click + natural scrolling (see scripts/) */
	NULL,
};

void
runautostart(void)
{
	const char *const *cmd = autostart;

	if (fork() == 0) {
		setsid();

		while (*cmd != NULL) {
			if (fork() == 0) {
				execl("/bin/sh", "sh", "-c", *cmd, NULL);
				exit(1);
			}
			cmd++;
		}
		exit(0);
	}
}

#endif /* AUTOSTART */

/* Keep this translation unit non-empty when the module is disabled
 * (ISO C forbids an empty translation unit). */
typedef int vxwm_autostart_module_present;
