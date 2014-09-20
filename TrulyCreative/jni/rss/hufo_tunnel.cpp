/*
 * Copyright (C) 2002  Jeremie Allard (Hufo / N.A.A.)
 * Ported to Linux by Tugrul Galatali <tugrul@galatali.com>
 *
 * hufo_tunnel is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * hufo_tunnel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

// hufo_smoke screen saver

#include <math.h>
#include <stdio.h>
#include <GLES/gl.h>
#include "jwzgles.h"

#include "driver.h"
#include "hufo_tunnel.h"
#include "hufo_tunnel_textures.h"
#include "loadTexture.h"


//#define OPTLIST_H 1

#include "optlist.h"


const char *ht_hack_name = "Hufo's little tunnel";

int dTexture;
int dWireframe;

#define XSTD (4.0/3.0)
float THole;			// tunnel time
float TVit;

void ht_hack_reshape (xstuff_t * XStuff)
{
    float x = (float)XStuff->windowWidth / (float)XStuff->windowHeight;	// Correct the viewing ratio of the window in the X axis.

    // Window initialization
    glViewport (0, 0, XStuff->windowWidth, XStuff->windowHeight);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();

    if (x > XSTD)
        gluOrtho2D (-x, x, -1, 1);	// Reset to a 2D screen space.
    else
        gluOrtho2D (-XSTD, XSTD, -XSTD / x, XSTD / x);	// Reset to a 2D screen space.

    // Reset model view matrix stack.
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
}

// Startup Stuff.
void ht_hack_init (xstuff_t * XStuff)	// Called right after the window is created, and OpenGL is initialized.
{
    // Reset the matrix to something we know.
    ht_hack_reshape (XStuff);

    if (!dWireframe) {
        if (dTexture) {
            unsigned char *l_tex;
            unsigned int tex;

            glGenTextures (1, &tex);

            glBindTexture (GL_TEXTURE_2D, tex);

            glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

            if (dTexture == 1) {
                LOAD_TEXTURE (l_tex, swirlmap, swirlmap_compressedsize, swirlmap_size)
                gluBuild2DMipmaps (GL_TEXTURE_2D, 3, 128, 128, GL_RGB, GL_UNSIGNED_BYTE, l_tex);
            } else {
                LOAD_TEXTURE (l_tex, marblemap, marblemap_compressedsize, marblemap_size)
                gluBuild2DMipmaps (GL_TEXTURE_2D, 3, 256, 256, GL_RGB, GL_UNSIGNED_BYTE, l_tex);
            }
            FREE_TEXTURE (l_tex)

            glEnable (GL_TEXTURE_2D);
        }
    }

    glCullFace (GL_FRONT);	// reject fliped faces
    glEnable (GL_CULL_FACE);
    glDisable (GL_DEPTH_TEST);	// no zbuffer

    THole = 0.0;
    //TVit = 8000.0;
    TVit = 6000.0;
    //TVit = 1.0;
    HoleInit ();		// initialise tunnel pos
}

void ht_hack_cleanup (xstuff_t * XStuff)
{
}

// Draw all the scene related stuff.
void ht_hack_draw (xstuff_t * XStuff, double currentTime, float frameTime)
{

//if (THole >= 5000) THole = 0; // does nothing, repeats as fast
//dWireframe = 0;
//dCoarse = 3;
//dTexture = 2;


    //THole += frameTime * TVit;
    // do not speed up ad you go on
    THole = frameTime * TVit;
    CalcHole ((int)THole);	// animate the tunnel
    if (HoleNbImgA == 0) {
        glClear (GL_COLOR_BUFFER_BIT);

        return;
    }
    // and render it
    glClear (GL_COLOR_BUFFER_BIT);
    int p, i;
    float f1, f2;

    for (p = HoleNbImgA - 2; p >= 0; --p) {
        f1 = min (1.0, 1.0 / (0.1 + PtDist[p] * (0.15)));
        f2 = min (1.0, 1.0 / (0.1 + PtDist[p + 1] * (0.15)));

        if (dWireframe) {
            glBegin (GL_LINES);
        } else {
            glBegin (GL_QUAD_STRIP);
        }

        for (i = 0; i <= HoleNbParImg; i += ((dCoarse > 0) ? dCoarse : 1)) {
            float f;

            if (dCoarse) {
                f = f1 * Pt[p][i].c1;
                glColor3f (f, f, f);
            } else {
                glColor3f (f1, f1, f1);
            }

            glTexCoord2f (Pt[p][i].u, Pt[p][i].v);
            glVertex2f (Pt[p][i].ex, Pt[p][i].ey);

            if (dCoarse) {
                f = f2 * Pt[p + 1][i].c1;
                glColor3f (f, f, f);
            } else {
                glColor3f (f2, f2, f2);
            }

            glTexCoord2f (Pt[p + 1][i].u, Pt[p + 1][i].v);
            glVertex2f (Pt[p + 1][i].ex, Pt[p + 1][i].ey);
        }
        glEnd ();
        if (dWireframe) {
            if (BBoxEmpty (&BBPlan[p]))
                glColor3f (1.0, 0.0, 0.0);
            else
                glColor3f (0.0, 1.0, 0.0);
            glBegin (GL_LINE_STRIP);
            glVertex2f (BBPlan[p].u0, BBPlan[p].v0);
            glVertex2f (BBPlan[p].u1, BBPlan[p].v0);
            glVertex2f (BBPlan[p].u1, BBPlan[p].v1);
            glVertex2f (BBPlan[p].u0, BBPlan[p].v1);
            glVertex2f (BBPlan[p].u0, BBPlan[p].v0);
            glEnd ();
            glColor3f (f1, f1, f1);
            glBegin (GL_LINE_STRIP);
            for (i = 0; i <= HoleNbParImg; i += ((dCoarse > 0) ? dCoarse : 1)) {
                glTexCoord2f (Pt[p][i].u, Pt[p][i].v);
                glVertex2f (Pt[p][i].ex, Pt[p][i].ey);
            }
            glEnd ();
        }
    }
}

void ht_hack_handle_opts (int argc, char *argv[])
{

    option_t *optList, *thisOpt;
    optList = NULL;
    optList = GetOptList(argc, argv, "hc:t:sSwW");
    int change_flag = 0;

    //dTexture = 2;
    //dCoarse = 3;
    dTexture = 1;
    //dCoarse = 0;
    dSinHole = 0;
    dWireframe = 0;

    //start
    while (optList != NULL) {
        thisOpt = optList;
        optList = optList->next;

        int c;

#ifdef HAVE_GETOPT_H
        static struct option ht_long_options[] = {
            {"help", 0, 0, 'h'},
            DRIVER_OPTIONS_LONG
            {"texture", 1, 0, 't'},
            {"swirl", 0, 0, 1},
            {"marble", 0, 0, 2},
            {"coarseness", 1, 0, 'c'},
            {"sinusoide", 0, 0, 's'},
            {"no-sinusoide", 0, 0, 'S'},
            {"wireframe", 0, 0, 'w'},
            {"no-wireframe", 0, 0, 'W'},
            {0, 0, 0, 0}
        };
#endif

        c= (int) thisOpt->option;


        if (c == NULL) {
            break;
        }

        switch (c) {
        DRIVER_OPTIONS_CASES case 'h':
            printf ("%s:"
#ifndef HAVE_GETOPT_H
                    " Not built with GNU getopt.h, long options *NOT* enabled."
#endif
                    "\n" DRIVER_OPTIONS_HELP "\t--texture/-t <arg>\n" "\t--swirl\n" "\t--marble\n" "\t--coarseness/-c <arg>\n" "\t--sinusoide/-s\n" "\t--no-sinusoide/-S\n"
                    "\t--wireframe/-w\n" "\t--no-wireframe/-W\n", argv[0]);
            exit (1);
        case 't':
            change_flag = 1;
            dTexture = strtol_minmaxdef (thisOpt->argument, 10, 0, 2, 0, 1, "--texture: ");
            break;
        case 1:
            change_flag = 1;
            dTexture = 1;
            break;
        case 2:
            change_flag = 1;
            dTexture = 2;
            break;
        case 'c':
            change_flag = 1;
            dCoarse = 1 << (3 - strtol_minmaxdef (thisOpt->argument, 10, 0, 3, 1, 0, "--coarseness: "));

            if (dCoarse == 8)
                dCoarse = 0;

            break;
        case 's':
            change_flag = 1;
            dSinHole = 1;
            break;
        case 'S':
            change_flag = 1;
            dSinHole = 0;
            break;
        case 'w':
            change_flag = 1;
            dWireframe = 1;
            break;
        case 'W':
            change_flag = 1;
            dWireframe = 0;
            break;
        }
    }
    // end

    free(thisOpt);
/*
    if (!change_flag) {
        dTexture = rsRandi (2) + 1;
        dCoarse = 1 << (3 - rsRandi (4));

        if (dCoarse == 8)
            dCoarse = 0;

        dSinHole = rsRandi (2);
        dWireframe = (rsRandi (10) == 0);
    }
*/
}
