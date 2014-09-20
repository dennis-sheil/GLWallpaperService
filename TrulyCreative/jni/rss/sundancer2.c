/*
 * Copyright (C) 2002  Dirk Songuer <delax@sundancerinc.de>
 * Ported to Linux by Tugrul Galatali <tugrul@galatali.com>
 *
 * Sundancer[2] is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation.
 *
 * Sundancer[2] is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

// Sundancer[2] screen saver

#include <math.h>
#include <stdio.h>

#include <GLES/gl.h>
#include "jwzgles.h"
#include "optlist.h"

typedef struct quadsystem_struct {
	float PositionX, PositionY, PositionZ;
	float ColorR, ColorG, ColorB;
	float Rotation;
	int Laenge, Hoehe;
	int Status, Intensity;
} quadsystem;

#include "driver.h"
#include "rsRand.h"

const char *sd_hack_name = "Sundancer[2]";

quadsystem *quads;

int quad_count = 150;

#define QUAD_SPEED_MAX_DEFAULT 20.0 / 400.0
float quad_speed_max = QUAD_SPEED_MAX_DEFAULT;
float quad_speed = QUAD_SPEED_MAX_DEFAULT;

int quads_timer = -1;
float direction_add = 0.00001f;
int reverse = 0;

int sr = 255, sg = 0, sb = 0;
int er = 255, eg = 255, eb = 0;

float transparency_value = 0.25f;

float LightPosition[4] = { 0.0, 0.0, 0.0, 1.0 };

float vlight, hlight, zlight;
float vlightmul, hlightmul, zlightmul;

float vertexw1, vertexw2, vertexw3, vertexw4;
float vertexwmul1, vertexwmul2, vertexwmul3, vertexwmul4;

void sd_hack_reshape (xstuff_t * XStuff)
{
	// Window initialization
	glViewport (0, 0, (GLsizei) XStuff->windowWidth, (GLsizei) XStuff->windowHeight);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();

	glOrtho (-400, 400, -300, 300, -400, 400);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
}

void sd_hack_init (xstuff_t * XStuff)	// Called right after the window is created, and OpenGL is initialized.
{
	float Ambient[4] = { 0.1, 0.1, 0.1, 1.0 };
	float Diffuse[4] = { 1.0, 1.0, 1.0, 1.0 };
	float temp_r, temp_g, temp_b;
	float step_r, step_g, step_b;
	int i;

	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel (GL_SMOOTH);

	if (transparency_value == 1.0f) {
		glEnable (GL_DEPTH_TEST);
		glClearDepth (1.0);
		glDepthFunc (GL_LESS);
	} else {
		glEnable (GL_BLEND);	// Turn Blending On
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable (GL_LIGHTING);

	glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, Diffuse);
	glLightfv (GL_LIGHT1, GL_AMBIENT, Ambient);
	glLightfv (GL_LIGHT1, GL_DIFFUSE, Diffuse);
	glLightfv (GL_LIGHT1, GL_POSITION, LightPosition);
	glLightfv (GL_LIGHT1, GL_SPOT_DIRECTION, LightPosition);

	glEnable (GL_LIGHT1);
	glEnable (GL_COLOR_MATERIAL);

	sd_hack_reshape (XStuff);

// Init Quads
	quads = (quadsystem *) malloc (quad_count * sizeof (quadsystem));

	step_r = (er - sr) / (quad_count * 255.0f);
	step_g = (eg - sg) / (quad_count * 255.0f);
	step_b = (eb - sb) / (quad_count * 255.0f);

	temp_r = sr / 255.0f;
	temp_g = sg / 255.0f;
	temp_b = sb / 255.0f;

	for (i = 0; i < quad_count; i++) {
		quads[i].PositionX = -100.0f;
		quads[i].PositionY = -100.0f;
		quads[i].PositionZ = i * 1.4f;

		quads[i].Laenge = 300 - i;
		quads[i].Hoehe = 300 - i;
		quads[i].Rotation = i / 100.0f;

		quads[i].ColorR = temp_r;
		quads[i].ColorG = temp_g;
		quads[i].ColorB = temp_b;

		temp_r += step_r;
		temp_g += step_g;
		temp_b += step_b;
	}

// Init Light
	vlight = 0.0;
	hlight = 0.0;
	zlight = 0.0;
	vlightmul = -1;
	hlightmul = -1;
	zlightmul = -1;

// Init Vertices
	vertexw1 = 1.0;
	vertexw2 = 1.0;
	vertexw3 = 1.0;
	vertexw4 = 1.0;

	vertexwmul1 = rsRandf (10) / 4096;
	vertexwmul2 = rsRandf (10) / 4096;
	vertexwmul3 = rsRandf (10) / 4096;
	vertexwmul4 = rsRandf (10) / 4096;
}

void sd_hack_cleanup (xstuff_t * XStuff)
{
}

void sd_hack_draw (xstuff_t * XStuff, double currentTime, float frameTime)
{
	static int change_direction = 0;
	int i;

	if (quads_timer == -1)
		quads_timer = (int)currentTime + (rsRandi (30) + 3);

	LightPosition[0] = hlight;	// set new light position - x
	LightPosition[1] = vlight;	// set new light position - y
	LightPosition[2] = zlight;	// set new light position - z

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity ();

	glLightfv (GL_LIGHT1, GL_POSITION, LightPosition);
	glTranslatef (0.0, -10.0, 0.0);

	for (i = 0; i < quad_count; i++) {
		glRotatef (quads[i].Rotation, 0.0, 0.0, 1.0);
		glColor4f (quads[i].ColorR, quads[i].ColorG, quads[i].ColorB, transparency_value);

		glColorMaterial (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

		glBegin (GL_QUADS);
		glVertex4f (quads[i].PositionX, quads[i].PositionY, quads[i].PositionZ, vertexw1);
		glVertex4f (quads[i].PositionX + quads[i].Laenge, quads[i].PositionY, quads[i].PositionZ, vertexw2);
		glVertex4f (quads[i].PositionX + quads[i].Laenge, quads[i].PositionY + quads[i].Hoehe, quads[i].PositionZ, vertexw3);
		glVertex4f (quads[i].PositionX, quads[i].PositionY + quads[i].Hoehe, quads[i].PositionZ, vertexw4);
		glEnd ();
	}

// Update Quads
	if (reverse) {		//rotation
		if (quads_timer < (int)currentTime)
			change_direction = 1;

		if (change_direction)
			quad_speed -= direction_add * quad_count;

		if ((quad_speed > quad_speed_max) || (quad_speed < -quad_speed_max)) {
			if (quad_speed > quad_speed_max) {
				quad_speed = quad_speed_max - 0.0001;
			} else {
				quad_speed = -quad_speed_max + 0.0001;
			}

			change_direction = 0;
			direction_add *= -1;
			quads_timer = (int)currentTime + (rsRandi (30) + 3);
		}
	}

	for (i = 0; i < quad_count; i++) {
		quads[i].Rotation += quad_speed;
	}

// Update Light
	if ((vlight > 300) || (vlight < -300))
		vlightmul *= -1;
	if ((hlight > 300) || (hlight < -300))
		hlightmul *= -1;
	if ((zlight > 400) || (zlight < 0))
		zlightmul *= -1;

	vlight += vlightmul;
	hlight += hlightmul;
	zlight += zlightmul;

// Update Vertices
	if ((vertexw1 > 2.5) || (vertexw1 < 0.7))
		vertexwmul1 *= -1;
	if ((vertexw2 > 2.5) || (vertexw2 < 0.7))
		vertexwmul2 *= -1;
	if ((vertexw3 > 2.5) || (vertexw3 < 0.7))
		vertexwmul3 *= -1;
	if ((vertexw4 > 2.5) || (vertexw4 < 0.7))
		vertexwmul4 *= -1;

	vertexw1 += vertexwmul1;
	vertexw2 += vertexwmul2;
	vertexw3 += vertexwmul3;
	vertexw4 += vertexwmul4;
}

void sd_hack_handle_opts (int argc, char **argv)
{
    option_t *optList, *thisOpt;
    optList = NULL;
    optList = GetOptList(argc, argv, "hq:s:t:f:b:eE");
	int change_flag = 0;

	while (optList != NULL) {
        thisOpt = optList;
        optList = optList->next;

		int c;

#ifdef HAVE_GETOPT_H
		static struct option sd_long_options[] = {
			{"help", 0, 0, 'h'},
			DRIVER_OPTIONS_LONG {"quad_count", 1, 0, 'q'},
			{"speed", 1, 0, 's'},
			{"transparency", 1, 0, 't'},
			{"foreground", 1, 0, 'f'},
			{"background", 1, 0, 'b'},
			{"reverse", 0, 0, 'e'},
			{"no-reverse", 0, 0, 'E'},
			{0, 0, 0, 0}
		};

#endif

                c= (int) thisOpt->option;

		if (c == NULL) break;

		switch (c) {
		DRIVER_OPTIONS_CASES 
		case 'h':
			printf ("%s:"
#ifndef HAVE_GETOPT_H
				" Not built with GNU getopt.h, long options *NOT* enabled."
#endif
				"\n" DRIVER_OPTIONS_HELP "\t--quad_count/-q <arg>\n" "\t--speed/-s <arg>\n" "\t--transparency/-t <arg>\n" "\t--foreground/-f <arg>\n"
				"\t--background/-b <arg>\n" "\t--reverse/-e\n" "\t--no-reverse/-E\n", argv[0]);
			exit (1);
		case 'q':
			change_flag = 1;
			quad_count = strtol_minmaxdef (thisOpt->argument, 10, 1, 300, 1, 150, "--quad_count: ");
			break;
		case 's':
			change_flag = 1;
			quad_speed_max = strtol_minmaxdef (thisOpt->argument, 10, 1, 40, 1, 20, "--speed: ") / 400.0;
			quad_speed = quad_speed_max;

			break;
		case 't':
			change_flag = 1;
			transparency_value = strtol_minmaxdef (optarg, 10, 1, 100, 1, 75, "--transparency: ") / 100.0f;
			break;
		case 'f':{
				int color = strtol_minmaxdef (optarg, 16, 0, 16777216, 0, 0xFF0000, "--foreground: ");

				change_flag = 1;

				er = (color & 0xFF0000) >> 16;
				eg = (color & 0xFF00) >> 8;
				eb = (color & 0xFF);

			}
			break;
		case 'b':{
				int color = strtol_minmaxdef (optarg, 16, 0, 16777216, 0, 0xFFFF00, "--background: ");

				change_flag = 1;

				sr = (color & 0xFF0000) >> 16;
				sg = (color & 0xFF00) >> 8;
				sb = (color & 0xFF);

			}
			break;
		case 'e':
			change_flag = 1;
			reverse = 1;
			break;
		case 'E':
			change_flag = 1;
			reverse = 0;
			break;
		}
	}

	if (!change_flag) {
		quad_count = rsRandi (296) + 5;

		quad_speed_max = rsRandf (50) / 400.0;
		quad_speed = quad_speed_max;

		transparency_value = (rsRandf (100) + 1) / 100.0f;

		sr = rsRandi (256);
		sg = rsRandi (256);
		sb = rsRandi (256);

		er = rsRandi (256);
		eg = rsRandi (256);
		eb = rsRandi (256);

		reverse = rsRandi (2);
	}
}
