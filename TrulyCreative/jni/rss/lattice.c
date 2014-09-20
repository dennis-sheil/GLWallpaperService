/*
 * Copyright (C) 2002  Terence M. Welsh
 * Ported to Linux by Tugrul Galatali <tugrul@galatali.com>
 *
 * Lattice is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation.
 *
 * Lattice is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * Lattice screensaver 
 */

const char *lt_hack_name = "lattice";

#include <stdio.h>
#ifdef __ANDROID_API__
#include <GLES/gl.h>
#include "jwzgles.h"
#include "optlist.h"
#define TRUE 1
#define FALSE 0
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <pthread.h>

#include "driver.h"
#include "lattice_textures.h"
#include "loadTexture.h"
#include "rsDefines.h"
#include "rsRand.h"
#include "rsMath.h"

#define PRESET_REGULAR 1
#define PRESET_CHAINMAIL 2
#define PRESET_BRASSMESH 3
#define PRESET_COMPUTER 4
#define PRESET_SLICK 5
#define PRESET_TASTY 6

/*
 * Where in the registry to store user defined variables 
 */
#define NUMOBJECTS 20
#define LATSIZE 12
#define TEXSIZE 256

/*
 * Globals 
 */
static int transitions[20][6] = {
	{1, 2, 12, 4, 14, 8},
	{0, 3, 15, 7, 7, 7},
	{3, 4, 14, 0, 7, 16},
	{2, 1, 15, 7, 7, 7},
	{5, 10, 12, 17, 17, 17},
	{4, 3, 13, 11, 9, 17},
	{12, 4, 10, 17, 17, 17},
	{2, 0, 14, 8, 16, 19},
	{1, 3, 15, 7, 7, 7},
	{4, 10, 12, 17, 17, 17},
	{11, 4, 12, 17, 17, 17},
	{10, 5, 15, 13, 17, 18},
	{13, 10, 4, 17, 17, 17},
	{12, 1, 11, 5, 6, 17},
	{15, 2, 12, 0, 7, 19},
	{14, 3, 1, 7, 7, 7},
	{3, 1, 15, 7, 7, 7},
	{5, 11, 13, 6, 9, 18},
	{10, 4, 12, 17, 17, 17},
	{15, 1, 3, 7, 7, 7}
};

typedef struct {
	float cullVec[4][4];
	float farplane;
} camera;

static camera theCamera;

static unsigned int lattice[LATSIZE][LATSIZE][LATSIZE];
static unsigned int list_base;
static unsigned int texture_id[2];

static float bPnt[10][6];	/* Border points and direction
			 * vectors where camera can cross
			 * from cube to cube */
static float path[7][6];
static int globalxyz[3];
static int lastBorder;
static int segments;

static unsigned int width;
static unsigned int height;

static float elapsedTime;

/*
 * Parameters edited in the dialog box 
 */
int dLongitude;
int dLatitude;
int dThick;
int dDensity;
int dDrawdepth;
int dFov;
int dPathrand;
static int dSpeed;
int dTexture;
int dSmooth;
int dFog;

// It would be nice to turn mut on, and feel safer about mutual exclusion
//pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutd = PTHREAD_MUTEX_INITIALIZER;

/*
 * Modulus function for picking the correct element of lattice array 
 */
#define myMod1(x) ((x % LATSIZE) + ((x < 0) ? LATSIZE : 0))
#define myMod(x) ((myMod1(x) == LATSIZE) ? 0 : myMod1(x))

/*
int myMod (int x)
{
	while (x < 0)
		x += LATSIZE;

	return (x % LATSIZE);
}
*/

/*
 * start point, start slope, end point, end slope, position (0.0 - 1.0) 
 * returns point somewhere along a smooth curve between the start point 
 * and end point 
 */
float interpolate (float a, float b, float c, float d, float where)
{
	float q = 2.0f * (a - c) + b + d;
	float r = 3.0f * (c - a) - 2.0f * b - d;

	return ((where * where * where * q) + (where * where * r) + (where * b) + a);
}

void camera_init (camera * c, float *mat, float f)
{
	float temp;

	/*
	 * far clipping plane 
	 */
	c->farplane = f;

	/*
	 * bottom and planes' vectors 
	 */
	temp = atan (1.0f / mat[5]);
	c->cullVec[0][0] = 0.0f;
	c->cullVec[0][1] = cos (temp);
	c->cullVec[0][2] = -sin (temp);
	c->cullVec[1][0] = 0.0f;
	c->cullVec[1][1] = -c->cullVec[0][1];
	c->cullVec[1][2] = c->cullVec[0][2];

	/*
	 * left and right planes' vectors 
	 */
	temp = atan (1.0f / mat[0]);
	c->cullVec[2][0] = cos (temp);
	c->cullVec[2][1] = 0.0f;
	c->cullVec[2][2] = -sin (temp);
	c->cullVec[3][0] = -c->cullVec[2][0];
	c->cullVec[3][1] = 0.0f;
	c->cullVec[3][2] = c->cullVec[2][2];
}

void setDefaults (int which)
{
	switch (which) {
	case PRESET_REGULAR:	/* Regular */
		dLongitude = 16;
		dLatitude = 8;
		dThick = 50;
		dDensity = 50;
		dDrawdepth = 4;
		dFov = 90;
		dPathrand = 7;
		dSpeed = 10;
		dTexture = 0;
		dSmooth = FALSE;
		dFog = TRUE;
		break;
	case PRESET_CHAINMAIL:	/* Chainmail */
		dLongitude = 24;
		dLatitude = 12;
		dThick = 50;
		dDensity = 80;
		dDrawdepth = 3;
		dFov = 90;
		dPathrand = 7;
		dSpeed = 10;
		dTexture = 3;
		dSmooth = TRUE;
		dFog = TRUE;
		break;
	case PRESET_BRASSMESH:	/* Brass Mesh */
		dLongitude = 4;
		dLatitude = 4;
		dThick = 40;
		dDensity = 50;
		dDrawdepth = 4;
		dFov = 90;
		dPathrand = 7;
		dSpeed = 10;
		dTexture = 4;
		dSmooth = FALSE;
		dFog = TRUE;
		break;
	case PRESET_COMPUTER:	/* Computer */
		dLongitude = 4;
		dLatitude = 6;
		dThick = 70;
		dDensity = 90;
		//dDrawdepth = 4;
		dDrawdepth = 1;
		//dFov = 90;
		dFov = 45;
		dPathrand = 7;
		//dSpeed = 10;
		dSpeed = 15;
		dTexture = 7;
		dSmooth = FALSE;
		dFog = TRUE;
		break;
	case PRESET_SLICK:	/* Slick */
		dLongitude = 24;
		dLatitude = 12;
		dThick = 100;
		dDensity = 30;
		dDrawdepth = 4;
		dFov = 90;
		dPathrand = 7;
		dSpeed = 10;
		dTexture = 5;
		dSmooth = TRUE;
		dFog = TRUE;
		break;
	case PRESET_TASTY:	/* Tasty */
		dLongitude = 24;
		dLatitude = 12;
		dThick = 100;
		//dDensity = 25;
		dDensity = 10;
		//dDrawdepth = 4;
		dDrawdepth = 2;
		//dFov = 90;
		dFov = 75;
		dPathrand = 7;
		//dSpeed = 10;
		dSpeed = 15;
		dTexture = 8;
		dSmooth = TRUE;
		dFog = TRUE;
		break;
	}
}

void computerCheck() {
if (dTexture != 7) {
glDeleteLists (0, NUMOBJECTS);
computerSet();
shape();
doAnInit();
}


}

void computerSet() {
                dLongitude = 4;
                dLatitude = 6;
                dThick = 70;
                dDensity = 90;
                dDrawdepth = 1;
                dFov = 45;
                dPathrand = 7;
                //dSpeed = 15;
                dTexture = 7;
                dSmooth = FALSE;
                dFog = TRUE;
}

void initTextures ()
{
	unsigned char *l_tex;

	glGenTextures (2, texture_id);

	switch (dTexture) {

	case 7:
		LOAD_TEXTURE (l_tex, circuittex, circuittex_compressedsize, circuittex_size)
		glBindTexture (GL_TEXTURE_2D, texture_id[0]);
		glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		gluBuild2DMipmaps (GL_TEXTURE_2D, GL_ALPHA, TEXSIZE, TEXSIZE, GL_ALPHA, GL_UNSIGNED_BYTE, l_tex);
		FREE_TEXTURE (l_tex)

		break;

	case 8:
		LOAD_TEXTURE (l_tex, doughtex, doughtex_compressedsize, doughtex_size)
		glBindTexture (GL_TEXTURE_2D, texture_id[0]);
		glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		gluBuild2DMipmaps (GL_TEXTURE_2D, 4, TEXSIZE, TEXSIZE, GL_RGBA, GL_UNSIGNED_BYTE, l_tex);
		FREE_TEXTURE (l_tex)
	}
}

void makeTorus (int smooth, int longitude, int latitude, float centerradius, float thickradius)
{
	int i, j;
	float r, rr;		/* Radius */
	float z, zz;		/* Depth */
	float cosa, sina;	/* Longitudinal positions */
	float cosn, cosnn, sinn, sinnn;	/* Normals for shading */
	float ncosa, nsina;	/* Longitudinal positions for shading */
	float u, v1, v2, ustep, vstep;
	float temp;
	float oldcosa = 0, oldsina = 0, oldncosa = 0, oldnsina = 0, oldcosn = 0, oldcosnn = 0, oldsinn = 0, oldsinnn = 0;

	/*
	 * Smooth shading? 
	 */
	if (smooth)
		glShadeModel (GL_SMOOTH);
	else
		glShadeModel (GL_FLAT);

	/*
	 * Initialize texture stuff 
	 */
	vstep = 1.0f / (float)latitude;
	ustep = ((float)((int)(centerradius / thickradius) + 0.5f)) / (float)longitude;
	v2 = 0.0f;

	for (i = 0; i < latitude; i++) {
		temp = PIx2 * (float)i / (float)latitude;
		cosn = cos (temp);
		sinn = sin (temp);
		temp = PIx2 * (float)(i + 1) / (float)latitude;
		cosnn = cos (temp);
		sinnn = sin (temp);
		r = centerradius + thickradius * cosn;
		rr = centerradius + thickradius * cosnn;
		z = thickradius * sinn;
		zz = thickradius * sinnn;
		if (!smooth) {	/* Redefine normals for flat shaded model */
			temp = PIx2 * ((float)i + 0.5f) / (float)latitude;
			cosn = cosnn = cos (temp);
			sinn = sinnn = sin (temp);
		}
		v1 = v2;
		v2 += vstep;
		u = 0.0f;
		glBegin (GL_TRIANGLE_STRIP);
		for (j = 0; j < longitude; j++) {
			temp = PIx2 * (float)j / (float)longitude;
			cosa = cos (temp);
			sina = sin (temp);
			if (smooth) {
				ncosa = cosa;
				nsina = sina;
			} else {	/* Redefine longitudinal component of
					 * normal for flat shading */
				temp = PIx2 * ((float)j - 0.5f) / (float)longitude;
				ncosa = cos (temp);
				nsina = sin (temp);
			}
			if (j == 0) {	/* Save first values for end of circular
					 * tri-strip */
				oldcosa = cosa;
				oldsina = sina;
				oldncosa = ncosa;
				oldnsina = nsina;
				oldcosn = cosn;
				oldcosnn = cosnn;
				oldsinn = sinn;
				oldsinnn = sinnn;
			}
			glNormal3f (cosnn * ncosa, cosnn * nsina, sinnn);
			glTexCoord2f (u, v2);
			glVertex3f (cosa * rr, sina * rr, zz);
			glNormal3f (cosn * ncosa, cosn * nsina, sinn);
			glTexCoord2f (u, v1);
			glVertex3f (cosa * r, sina * r, z);
			u += ustep;	/* update u texture coordinate */
		}
		/*
		 * Finish off circular tri-strip with saved first values 
		 */
		glNormal3f (oldcosnn * oldncosa, oldcosnn * oldnsina, oldsinnn);
		glTexCoord2f (u, v2);
		glVertex3f (oldcosa * rr, oldsina * rr, zz);
		glNormal3f (oldcosn * oldncosa, oldcosn * oldnsina, oldsinn);
		glTexCoord2f (u, v1);
		glVertex3f (oldcosa * r, oldsina * r, z);
		glEnd ();
	}
}

void setMaterialAttribs(){
	if(dTexture == 0 || dTexture >= 5)
		glColor3f(rsRandf(1.0f), rsRandf(1.0f), rsRandf(1.0f));
	if(dTexture == 1)
		glBindTexture(GL_TEXTURE_2D, texture_id[rsRandi(2)]);
}

/*
 * Build the lattice display lists 
 */
void makeLatticeObjects ()
{
	int i, d = 0;
	float thick = (float)dThick * 0.001f;

	list_base = glGenLists(NUMOBJECTS);

	for (i = 0; i < NUMOBJECTS; i++) {
		glNewList (list_base + i, GL_COMPILE);

		if (dTexture >= 2)
			glBindTexture (GL_TEXTURE_2D, texture_id[0]);

		if (d < dDensity) {
			glPushMatrix ();

			setMaterialAttribs();

			glTranslatef (-0.25f, -0.25f, -0.25f);

			if (rsRandi (2))
				glRotatef (180.0f, 1, 0, 0);

			makeTorus (dSmooth, dLongitude, dLatitude, 0.36f - thick, thick);

			glPopMatrix ();
		}

		d = (d + 37) % 100;

		if (d < dDensity) {
			glPushMatrix ();

			setMaterialAttribs();

			glTranslatef (0.25f, -0.25f, -0.25f);

			if (rsRandi (2))
				glRotatef (90.0f, 1, 0, 0);
			else
				glRotatef (-90.0f, 1, 0, 0);

			makeTorus (dSmooth, dLongitude, dLatitude, 0.36f - thick, thick);

			glPopMatrix ();
		}

		d = (d + 37) % 100;

		if (d < dDensity) {
			glPushMatrix ();

			setMaterialAttribs();

			glTranslatef (0.25f, -0.25f, 0.25f);

			if (rsRandi (2))
				glRotatef (90.0f, 0, 1, 0);
			else
				glRotatef (-90.0f, 0, 1, 0);

			makeTorus (dSmooth, dLongitude, dLatitude, 0.36f - thick, thick);

			glPopMatrix ();
		}

		d = (d + 37) % 100;

		if (d < dDensity) {
			glPushMatrix ();

			setMaterialAttribs();

			glTranslatef (0.25f, 0.25f, 0.25f);

			if (rsRandi (2))
				glRotatef (180.0f, 1, 0, 0);

			makeTorus (dSmooth, dLongitude, dLatitude, 0.36f - thick, thick);

			glPopMatrix ();
		}

		d = (d + 37) % 100;

		if (d < dDensity) {
			glPushMatrix ();

			setMaterialAttribs();

			glTranslatef (-0.25f, 0.25f, 0.25f);

			if (rsRandi (2))
				glRotatef (90.0f, 1, 0, 0);
			else
				glRotatef (-90.0f, 1, 0, 0);

			makeTorus (dSmooth, dLongitude, dLatitude, 0.36f - thick, thick);

			glPopMatrix ();
		}

		d = (d + 37) % 100;
		if (d < dDensity) {
			glPushMatrix ();

			setMaterialAttribs();

			glTranslatef (-0.25f, 0.25f, -0.25f);

			if (rsRandi (2))
				glRotatef (90.0f, 0, 1, 0);
			else
				glRotatef (-90.0f, 0, 1, 0);

			makeTorus (dSmooth, dLongitude, dLatitude, 0.36f - thick, thick);

			glPopMatrix ();
		}

		glEndList ();

		d = (d + 37) % 100;
	}
}

void reconfigure ()
{
	int i, j;
	int newBorder, positive;

	/*
	 * End of old path = start of new path 
	 */
	for (i = 0; i < 6; i++)
		path[0][i] = path[segments][i];

	/*
	 * determine if direction of motion is positive or negative 
	 */
	/*
	 * update global position 
	 */
	if (lastBorder < 6) {
		if ((path[0][3] + path[0][4] + path[0][5]) > 0.0f) {
			positive = 1;
			globalxyz[lastBorder / 2]++;
		} else {
			positive = 0;
			globalxyz[lastBorder / 2]--;
		}
	} else {
		if (path[0][3] > 0.0f) {
			positive = 1;
			globalxyz[0]++;
		} else {
			positive = 0;
			globalxyz[0]--;
		}

		if (path[0][4] > 0.0f)
			globalxyz[1]++;
		else
			globalxyz[1]--;

		if (path[0][5] > 0.0f)
			globalxyz[2]++;
		else
			globalxyz[2]--;
	}

	if (!rsRandi (11 - dPathrand)) {	/* Change directions */
		if (!positive)
			lastBorder += 10;

		newBorder = transitions[lastBorder][rsRandi (6)];
		positive = 0;
		if (newBorder < 10)
			positive = 1;
		else
			newBorder -= 10;

		for (i = 0; i < 6; i++)	/* set the new border point */
			path[1][i] = bPnt[newBorder][i];

		if (!positive) {	/* flip everything if direction is
					 * negative */
			if (newBorder < 6)
				path[1][newBorder / 2] *= -1.0f;
			else
				for (i = 0; i < 3; i++)
					path[1][i] *= -1.0f;
			for (i = 3; i < 6; i++)
				path[1][i] *= -1.0f;
		}

		for (i = 0; i < 3; i++)	/* reposition the new border */
			path[1][i] += globalxyz[i];

		lastBorder = newBorder;
		segments = 1;
	} else {		/* Just keep going straight */
		newBorder = lastBorder;

		for (i = 0; i < 6; i++)
			path[1][i] = bPnt[newBorder][i];

		i = newBorder / 2;

		if (!positive) {
			if (newBorder < 6)
				path[1][i] *= -1.0f;
			else {
				path[1][0] *= -1.0f;
				path[1][1] *= -1.0f;
				path[1][2] *= -1.0f;
			}

			path[1][3] *= -1.0f;
			path[1][4] *= -1.0f;
			path[1][5] *= -1.0f;
		}

		for (j = 0; j < 3; j++) {
			path[1][j] += globalxyz[j];
			if ((newBorder < 6) && (j != 1))
				path[1][j] += rsRandf (0.15f) - 0.075f;
		}

		if (newBorder >= 6)
			path[1][0] += rsRandf (0.1f) - 0.05f;

		segments = 1;
	}
}

void draw ()
{
	int i, j, k;
	int indexx, indexy, indexz;
	float xyz[4], dir[4], angvel[4], tempVec[4];
	static float oldxyz[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	static float oldDir[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	static float oldAngvel[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float angle, distance;
	float rotMat[16];
	float newQuat[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	static float quat[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	static int flymode = 1;
	static float flymodeChange = 20.0f;
	static int seg = 0;	/* Section of path */
	static float where = 0.0f;	/* Position on path */
	static float rollVel = 0.0f, rollAcc = 0.0f;
	int drawDepth = dDrawdepth + 2;
	static float rollChange = 0;	/* rsRandf (10.0f) + 2.0f; */
	pthread_mutex_lock(&mutd);

	where += (float)dSpeed * 0.05f * elapsedTime;
	if (where >= 1.0f) {
		where -= 1.0f;
		seg++;
	}

	if (seg >= segments) {
		seg = 0;
		reconfigure ();
	}


	/*
	 * Calculate position 
	 */
	xyz[0] = interpolate (path[seg][0], path[seg][3], path[seg + 1][0], path[seg + 1][3], where);
	xyz[1] = interpolate (path[seg][1], path[seg][4], path[seg + 1][1], path[seg + 1][4], where);
	xyz[2] = interpolate (path[seg][2], path[seg][5], path[seg + 1][2], path[seg + 1][5], where);

	/*
	 * Do rotation stuff 
	 */
	rsVec_subtract (xyz, oldxyz, (float *)&dir);
	rsVec_normalize ((float *)&dir);
	rsVec_cross ((float *)&angvel, dir, oldDir); /* Desired axis of rotation */

	/* Protect against mild "overflow" */
	float dot = MAX(MIN(rsVec_dot (oldDir, dir), -1.0), 1.0);
	float maxSpin = 0.25f * (float)dSpeed * elapsedTime;
	angle = MAX(MIN(acos(dot), -maxSpin), maxSpin);

	rsVec_scale ((float *)&angvel, angle); /* Desired angular velocity */
	rsVec_subtract (angvel, oldAngvel, (float *)&tempVec); /* Change in angular velocity */
	distance = rsVec_length (tempVec); /* Don't let angular velocity change too much */
	float rotationInertia = 0.007f * (float)dSpeed * elapsedTime;
	if (distance > rotationInertia * elapsedTime) {
		rsVec_scale ((float *)&tempVec, ((rotationInertia * elapsedTime) / distance));
		rsVec_add (oldAngvel, tempVec, (float *)&angvel);
	}

	flymodeChange -= elapsedTime;

	if (flymodeChange <= 1.0f)	/* prepare to transition */
		rsVec_scale ((float *)&angvel, flymodeChange);

	if (flymodeChange <= 0.0f) {	/* transition from one fly mode to 
					 * the other? */
		flymode = rsRandi (4);
		flymodeChange = rsRandf ((float)(150 - dSpeed)) + 5.0f;
	}

	rsVec_copy (angvel, (float *)&tempVec);	/* Recompute desired rotation */
	angle = rsVec_normalize ((float *)&tempVec);
	rsQuat_make ((float *)&newQuat, angle, tempVec[0], tempVec[1], tempVec[2]);	/* Use rotation */

	if (flymode)		/* fly normal (straight) */
		rsQuat_preMult ((float *)&quat, newQuat);
	else			/* don't fly normal (go backwards and stuff) */
		rsQuat_postMult ((float *)&quat, newQuat);

	/* Roll */
	rollChange -= elapsedTime;
	if (rollChange <= 0.0f) {
		rollAcc = rsRandf (0.02f * (float)dSpeed) - (0.01f * (float)dSpeed);
		rollChange = rsRandf (10.0f) + 2.0f;
	}

	rollVel += rollAcc * elapsedTime;

	if (rollVel > (0.04f * (float)dSpeed) && rollAcc > 0.0f)
		rollAcc = 0.0f;

	if (rollVel < (-0.04f * (float)dSpeed) && rollAcc < 0.0f)
		rollAcc = 0.0f;

	rsQuat_make ((float *)&newQuat, rollVel * elapsedTime, oldDir[0], oldDir[1], oldDir[2]);
	rsQuat_preMult ((float *)&quat, newQuat);

	/* quat.normalize(); */
	rsQuat_toMat ((float *)&quat, (float *)&rotMat);

	/*
	 * Save old stuff 
	 */
	rsVec_copy (xyz, (float *)&oldxyz);
	oldDir[0] = -rotMat[2];
	oldDir[1] = -rotMat[6];
	oldDir[2] = -rotMat[10];
	rsVec_copy (angvel, (float *)&oldAngvel);

	/*
	 * Apply transformations 
	 */
	glMatrixMode (GL_MODELVIEW);
	glLoadMatrixf (rotMat);
	glTranslatef (-xyz[0], -xyz[1], -xyz[2]);

	// Just in case display lists contain no colors
	glColor3f(1.0f, 1.0f, 1.0f);


	// Environment mapping for crystal, chrome, brass, shiny, and ghostly
	if(dTexture == 2 || dTexture == 3 || dTexture == 4  || dTexture == 5 || dTexture == 6){
                /*
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
                */
	}

	/*
	 * Render everything 
	 */
	//glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear (GL_COLOR_BUFFER_BIT);
	glClear (GL_DEPTH_BUFFER_BIT);
	for (i = globalxyz[0] - drawDepth; i <= globalxyz[0] + drawDepth; i++) {
		for (j = globalxyz[1] - drawDepth; j <= globalxyz[1] + drawDepth; j++) {
			for (k = globalxyz[2] - drawDepth; k <= globalxyz[2] + drawDepth; k++) {
				float tpos[4];	/* transformed position */

				tempVec[0] = (float)i - xyz[0];
				tempVec[1] = (float)j - xyz[1];
				tempVec[2] = (float)k - xyz[2];

				tpos[0] = tempVec[0] * rotMat[0] + tempVec[1] * rotMat[4] + tempVec[2] * rotMat[8];	/* + rotMat[12]; */
				tpos[1] = tempVec[0] * rotMat[1] + tempVec[1] * rotMat[5] + tempVec[2] * rotMat[9];	/* + rotMat[13]; */
				tpos[2] = tempVec[0] * rotMat[2] + tempVec[1] * rotMat[6] + tempVec[2] * rotMat[10];	/* + rotMat[14]; */

				#define radius 0.9f
				/* camera_inViewVolume */
				/*
				 * check back plane 
				 */
				if (!(tpos[2] < -(theCamera.farplane + radius))) {
					/*
					 * check bottom plane 
					 */
					if (!(rsVec_dot(tpos, theCamera.cullVec[0]) < -radius)) {
						/*
						 * check top plane 
						 */
						if (!(rsVec_dot(tpos, theCamera.cullVec[1]) < -radius)) {
							/*
							 * check left plane 
							 */
							if (!(rsVec_dot(tpos, theCamera.cullVec[2]) < -radius)) {
								/*
								 * check right plane 
								 */
								if (!(rsVec_dot(tpos, theCamera.cullVec[3]) < -radius)) {
									indexx = myMod (i);
									indexy = myMod (j);
									indexz = myMod (k);

									/*
									 * draw it 
									 */
									glPushMatrix ();
									glTranslatef ((float)i, (float)j, (float)k);
									glCallList (lattice[indexx][indexy][indexz]);
									glPopMatrix ();
								}
							}
						}
					}
				}
			}
		}
	}

	if(dTexture == 2 || dTexture == 3 || dTexture == 4  || dTexture == 5 || dTexture == 6){
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
        }

	glFlush ();
	pthread_mutex_unlock(&mutd);
}

void lt_hack_draw (xstuff_t * XStuff, double currentTime, float frameTime)
{
	static float times[10] = { 0.03f, 0.03f, 0.03f, 0.03f, 0.03f,
		0.03f, 0.03f, 0.03f, 0.03f, 0.03f
	};
	static int timeindex = 0;
#define BENCHMARK 1
#ifdef BENCHMARK
	static int a = 1;
#endif

	if (frameTime > 0)
		times[timeindex] = frameTime;
	else
		times[timeindex] = elapsedTime;

#ifdef BENCHMARK
	elapsedTime = 0.027;
#else
	elapsedTime = 0.1f * (times[0] + times[1] + times[2] + times[3] + times[4] + times[5] + times[6] + times[7] + times[8] + times[9]);

	timeindex++;
	if (timeindex >= 10)
		timeindex = 0;
#endif

	computerCheck();

	draw ();

#ifdef BENCHMARK
//	if (a++ == 1000)
//		exit(0);
#endif
}

void lt_hack_reshape (xstuff_t * XStuff)
{
	float mat[16] = { cos ((float)dFov * 0.5f * DEG2RAD) / sin ((float)dFov * 0.5f * DEG2RAD),
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f - 0.02f / (float)dDrawdepth, -1.0f,
		0.0f, 0.0f, -(0.02f + 0.0002f / (float)dDrawdepth), 0.0f
	};

        height = XStuff->windowHeight;
        width = XStuff->windowWidth;

	glViewport (0, 0, (GLint) XStuff->windowWidth, (GLint) XStuff->windowHeight);

	glFrontFace (GL_CCW);
	glEnable (GL_CULL_FACE);
	glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();

	mat[5] = mat[0] * ((float)XStuff->windowWidth / (float)XStuff->windowHeight);

	glLoadMatrixf (mat);
	camera_init (&theCamera, mat, (float)dDrawdepth);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
}

void shape ()
{
	float mat[16] = { cos ((float)dFov * 0.5f * DEG2RAD) / sin ((float)dFov * 0.5f * DEG2RAD),
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f - 0.02f / (float)dDrawdepth, -1.0f,
		0.0f, 0.0f, -(0.02f + 0.0002f / (float)dDrawdepth), 0.0f
	};

	glViewport (0, 0, width, height);

	glFrontFace (GL_CCW);
	glEnable (GL_CULL_FACE);
	glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();

	mat[5] = mat[0] * ((float)width/(float) height);

	glLoadMatrixf (mat);
	camera_init (&theCamera, mat, (float)dDrawdepth);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
}


void lt_hack_init (xstuff_t * XStuff)
{
	int i, j, k;
	glDeleteLists (0, NUMOBJECTS);
	//pthread_mutex_lock(&mut);
        // see note above regarding mut

	lt_hack_reshape (XStuff);
        doAnInit();
}

void doAnInit()
{
	int i, j, k;

	if (dTexture == 9)	/* Choose random texture */
		dTexture = rsRandi (9);

	if (dTexture != 2 && dTexture != 6)	/* No z-buffering for crystal or ghostly */
		glEnable (GL_DEPTH_TEST);

	if (dTexture != 3 && dTexture != 4 && dTexture != 6) {	/* No lighting for chrome, brass, or ghostly */
		float ambient[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		float diffuse[4] = { 1.0f, 1.0f, 1.0f, 0.0f };
		float specular[4] = { 1.0f, 1.0f, 1.0f, 0.0f };
		float position[4] = { 400.0f, 300.0f, 500.0f, 0.0f };

		/* float position[4] = {0.0f, 0.0f, 0.0f, 1.0f}; */

		glEnable (GL_LIGHTING);
		glEnable (GL_LIGHT0);
		//glLightModeli (GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
		// ^ not an option for Android apparently
		// I will try two side.  AMBIENT is another choice
		glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

		glLightfv (GL_LIGHT0, GL_AMBIENT, ambient);
		glLightfv (GL_LIGHT0, GL_DIFFUSE, diffuse);
		glLightfv (GL_LIGHT0, GL_SPECULAR, specular);
		glLightfv (GL_LIGHT0, GL_POSITION, position);
	}
	glEnable (GL_COLOR_MATERIAL);
	if (dTexture == 0 || dTexture == 5 || dTexture >= 7) {
		glMaterialf (GL_FRONT, GL_SHININESS, 50.0f);
		glColorMaterial (GL_FRONT, GL_SPECULAR);
	}
	if (dTexture == 2) {
		glMaterialf (GL_FRONT, GL_SHININESS, 10.0f);
		glColorMaterial (GL_FRONT, GL_SPECULAR);
	}
	glColorMaterial (GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	if (dFog) {
		float fog_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

		glEnable (GL_FOG);
		glFogfv (GL_FOG_COLOR, fog_color);
		glFogf (GL_FOG_MODE, GL_LINEAR);
		glFogf (GL_FOG_START, (float)dDrawdepth * 0.3f);
		glFogf (GL_FOG_END, (float)dDrawdepth - 0.1f);
	}

	if (dTexture == 2 || dTexture == 6) {	/* Use blending for crystal and ghostly */
		glBlendFunc (GL_SRC_ALPHA, GL_ONE);
		glEnable (GL_BLEND);
	}

	if (dTexture == 7) {	/* Use blending for circuits */
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable (GL_BLEND);
	}

	if (dTexture) {
		glEnable (GL_TEXTURE_2D);
		initTextures ();
	}

	/*
	 * Initialize lattice objects and their positions in the lattice array 
	 */
	makeLatticeObjects ();
	for (i = 0; i < LATSIZE; i++) {
		for (j = 0; j < LATSIZE; j++) {
			for (k = 0; k < LATSIZE; k++) {
				lattice[i][j][k] = list_base + rsRandi (NUMOBJECTS);
			}
		}
	}

	/*
	 * Initialize border points horizontal border points 
	 */

	/*
	 * horizontal border points 
	 */
	for (i = 0; i < 6; i++) {
		for (j = 0; j < 6; j++) {
			bPnt[i][j] = 0.0f;
		}
	}

	bPnt[0][0] = 0.5f;
	bPnt[0][1] = -0.25f;
	bPnt[0][2] = 0.25f;
	bPnt[0][3] = 1.0f;	/* right */
	bPnt[1][0] = 0.5f;
	bPnt[1][1] = 0.25f;
	bPnt[1][2] = -0.25f;
	bPnt[1][3] = 1.0f;	/* right */
	bPnt[2][0] = -0.25f;
	bPnt[2][1] = 0.5f;
	bPnt[2][2] = 0.25f;
	bPnt[2][4] = 1.0f;	/* top */
	bPnt[3][0] = 0.25f;
	bPnt[3][1] = 0.5f;
	bPnt[3][2] = -0.25f;
	bPnt[3][4] = 1.0f;	/* top */
	bPnt[4][0] = -0.25f;
	bPnt[4][1] = -0.25f;
	bPnt[4][2] = 0.5f;
	bPnt[4][5] = 1.0f;	/* front */
	bPnt[5][0] = 0.25f;
	bPnt[5][1] = 0.25f;
	bPnt[5][2] = 0.5f;
	bPnt[5][5] = 1.0f;	/* front */

	/*
	 * diagonal border points 
	 */
	bPnt[6][0] = 0.5f;
	bPnt[6][1] = -0.5f;
	bPnt[6][2] = -0.5f;
	bPnt[6][3] = 1.0f;
	bPnt[6][4] = -1.0f;
	bPnt[6][5] = -1.0f;
	bPnt[7][0] = 0.5f;
	bPnt[7][1] = 0.5f;
	bPnt[7][2] = -0.5f;
	bPnt[7][3] = 1.0f;
	bPnt[7][4] = 1.0f;
	bPnt[7][5] = -1.0f;
	bPnt[8][0] = 0.5f;
	bPnt[8][1] = -0.5f;
	bPnt[8][2] = 0.5f;
	bPnt[8][3] = 1.0f;
	bPnt[8][4] = -1.0f;
	bPnt[8][5] = 1.0f;
	bPnt[9][0] = 0.5f;
	bPnt[9][1] = 0.5f;
	bPnt[9][2] = 0.5f;
	bPnt[9][3] = 1.0f;
	bPnt[9][4] = 1.0f;
	bPnt[9][5] = 1.0f;

	globalxyz[0] = 0;
	globalxyz[1] = 0;
	globalxyz[2] = 0;

	/*
	 * Set up first path section 
	 */
	path[0][0] = 0.0f;
	path[0][1] = 0.0f;
	path[0][2] = 0.0f;
	path[0][3] = 0.0f;
	path[0][4] = 0.0f;
	path[0][5] = 0.0f;

	j = rsRandi (12);
	k = j % 6;
	for (i = 0; i < 6; i++)
		path[1][i] = bPnt[k][i];

	if (j > 5) {		/* If we want to head in a negative direction */
		i = k / 2;	/* then we need to flip along the appropriate axis */
		path[1][i] *= -1.0f;
		path[1][i + 3] *= -1.0f;
	}

	lastBorder = k;
	segments = 1;
}

void lt_hack_cleanup (xstuff_t * XStuff)
{
	glDeleteLists (0, NUMOBJECTS);
	//pthread_mutex_unlock(&mut);
        // see note above regarding mut
}

void lt_hack_handle_opts (int argc, char **argv)
{
	setDefaults (PRESET_COMPUTER);

	option_t *optList, *thisOpt;
	optList = NULL;
	optList = GetOptList(argc, argv, "hp:l:L:t:d:D:o:P:e:T:sSfF");

	int change_flag = 0;

	while (optList != NULL) {
		thisOpt = optList;
		optList = optList->next;

		int c;
		c= (int) thisOpt->option;

		if (c == NULL)
			break;

		switch (c) {
		DRIVER_OPTIONS_CASES case 'h':
			printf ("%s:"
#ifndef HAVE_GETOPT_H
				" Not built with GNU getopt.h, long options *NOT* enabled."
#endif
				"\n" DRIVER_OPTIONS_HELP 
				"\t--preset/-p <arg>\n" "\t--regular\n" "\t--chainmail\n" "\t--brassmesh\n" "\t--computer\n" "\t--slick\n" "\t--tasty\n"
				"\t--longitude/-l <arg>\n" "\t--latitude/-L <arg>\n" 
				"\t--thick/-t <arg>\n" "\t--density/-d <arg>\n" 
				"\t--drawdepth/-D <arg>\n" "\t--fov/-o <arg>\n" 
				"\t--pathrand/-P <arg>\n" "\t--speed/-e <arg>\n" 
				"\t--texture/-T <arg>\n" "\t--industrial\n" "\t--crystal\n" "\t--chrome\n" "\t--brass\n" "\t--shiny\n" "\t--ghostly\n" "\t--circuits\n" "\t--doughnuts\n"
				"\t--smooth/-s\n" "\t--no-smooth/-S\n" 
				"\t--fog/-f\n" " \t--no-fog/-F\n", argv[0]);
			exit (1);
		case 'p':
			change_flag = 1;
			setDefaults (strtol_minmaxdef (thisOpt->argument, 10, 1, 6, 0, 1, "--preset: "));
			break;
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			change_flag = 1;
			setDefaults (c - 9);
			break;
		case 'l':
			change_flag = 1;
			dLongitude = strtol_minmaxdef (optarg, 10, 4, 100, 1, 16, "--longitude: ");
			break;
		case 'L':
			change_flag = 1;
			dLatitude = strtol_minmaxdef (optarg, 10, 2, 100, 1, 8, "--latitude: ");
			break;
		case 't':
			change_flag = 1;
			dThick = strtol_minmaxdef (optarg, 10, 1, 100, 1, 50, "--thick: ");
			break;
		case 'd':
			change_flag = 1;
			dDensity = strtol_minmaxdef (optarg, 10, 1, 100, 1, 50, "--density: ");
			break;
		case 'D':
			change_flag = 1;
			dDrawdepth = strtol_minmaxdef (thisOpt->argument, 10, 1, 8, 1, 4, "--drawdepth: ");
			break;
		case 'o':
			change_flag = 1;
			dFov = strtol_minmaxdef (optarg, 10, 10, 150, 1, 90, "--fov: ");
			break;
		case 'P':
			change_flag = 1;
			dPathrand = strtol_minmaxdef (optarg, 10, 1, 10, 1, 1, "--pathrand: ");
			break;
		case 'e':
			change_flag = 1;
			dSpeed = strtol_minmaxdef (thisOpt->argument, 10, 1, 100, 1, 1, "--speed: ");
			break;
		case 'T':
			change_flag = 1;
			dTexture = strtol_minmaxdef (optarg, 10, 0, 9, 0, 0, "--texture: ");
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
			change_flag = 1;
			dTexture = c;
			break;
		case 's':
			change_flag = 1;
			dSmooth = 1;
			break;
		case 'S':
			change_flag = 1;
			dSmooth = 0;
			break;
		case 'f':
			change_flag = 1;
			dFog = 1;
			break;
		case 'F':
			change_flag = 1;
			dFog = 0;
			break;
		}
	}

}
