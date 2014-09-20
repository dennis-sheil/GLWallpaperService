/*
 * Copyright (C) 2002 Tugrul Galatali <tugrul@galatali.com>
 *
 * driver.c is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * driver.c is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/param.h>

#include "config.h"

#include "driver.h"


xstuff_t *XStuff;

extern const char *ht_hack_name;
extern const char *sd_hack_name;

/*
 * display parameters
 */
int rootWindow = False;
int glewInitialized = False;
#ifdef HAVE_GLEW
int frameTime = 10000;
int vsync = 1;
#else
int frameTime = 33333;
int vsync = 0;
#endif
int idleOnDPMS = 1;
int signalled = 0;


int strtol_minmaxdef (const char *optarg, const int base, const int min, const int max, const int type, const int def, const char *errmsg)
{
	const int result = strtol (optarg, (char **)NULL, base);

	if (result < min) {
		if (errmsg) {
			fprintf (stderr, "%s %d < %d, using %d instead.\n", errmsg, result, min, type ? min : def);
		}

		return type ? min : def;
	}

	if (result > max) {
		if (errmsg) {
			fprintf (stderr, "%s %d > %d, using %d instead.\n", errmsg, result, max, type ? max : def);
		}

		return type ? max : def;
	}

	return result;
}


void handle_global_opts (int c)
{
	switch (c) {
	case 'r':
		rootWindow = 1;

		break;
	case 'x':
		c = strtol_minmaxdef (optarg, 10, 0, 10000, 1, 100, "--maxfps: ");

		frameTime = (c > 0) ? 1000000 / c : 0;

		break;
	case 'y':
		vsync = strtol_minmaxdef (optarg, 10, 0, 100, 1, 2, "--vsync: ");

#ifndef HAVE_GLEW
		if (vsync)
			fprintf (stderr, "Not compiled with GLEW, vsync not supported.\n");
#endif

		break;
	case 'M':
		idleOnDPMS = strtol_minmaxdef (optarg, 10, 0, 1, 0, 1, "--dpms: ");

#ifndef HAVE_DPMS_EXT
		if (idleOnDPMS)
			fprintf (stderr, "Not compiled with DPMS, idling on DPMS not supported.\n");
#endif

		break;
	}
}
