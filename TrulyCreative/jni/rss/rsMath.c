/*
 * Copyright (C) 2002  Terence M. Welsh
 * Ported to Linux by Tugrul Galatali <tugrul@galatali.com>
 *
 * rsMath is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation.
 *
 * rsMath is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "config.h"
#include "rsMath.h"
#include "rsDefines.h"

float rsVec_length (float *v)
{
	return (float)sqrt (v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

float rsVec_normalize (float *v)
{
	float length;

	length = (float)sqrt (v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

	if (length == 0.0f) {
		v[1] = 1.0f;
		return (0.0f);
	}

	v[0] /= length;
	v[1] /= length;
	v[2] /= length;

	return length;
}

void rsVec_cross (float *v, float vec1[4], float vec2[4])
{
	v[0] = vec1[1] * vec2[2] - vec2[1] * vec1[2];
	v[1] = vec1[2] * vec2[0] - vec2[2] * vec1[0];
	v[2] = vec1[0] * vec2[1] - vec2[0] * vec1[1];
}

void rsVec_scale (float *v, float scale)
{
	v[0] *= scale;
	v[1] *= scale;
	v[2] *= scale;
}

void rsVec_copy (float v[3], float *dest)
{
	dest[0] = v[0];
	dest[1] = v[1];
	dest[2] = v[2];
}

void rsVec_add (float v[3], float vec[3], float *dest)
{
	dest[0] = v[0] + vec[0];
	dest[1] = v[1] + vec[1];
	dest[2] = v[2] + vec[2];
}

void rsVec_subtract (float v[3], float vec[3], float *dest)
{
	dest[0] = v[0] - vec[0];
	dest[1] = v[1] - vec[1];
	dest[2] = v[2] - vec[2];
}

void rsQuat_make (float *q, float a, float x, float y, float z)
{
	if (a < RSEPSILON && a > -RSEPSILON) {
		q[0] = 0.0f;
		q[1] = 0.0f;
		q[2] = 0.0f;
		q[3] = 1.0f;
	} else {
		float sintheta;

		a *= 0.5f;
		sintheta = sin (a);
		q[0] = sintheta * x;
		q[1] = sintheta * y;
		q[2] = sintheta * z;
		q[3] = cos (a);
	}
}

void rsQuat_preMult (float *q, float postQuat[4])
{
	/*
	 * q1q2 = s1v2 + s2v1 + v1xv2, s1s2 - v1.v2 
	 */
	float tempx = q[0];
	float tempy = q[1];
	float tempz = q[2];
	float tempw = q[3];

	q[0] = tempw * postQuat[0] + postQuat[3] * tempx + tempy * postQuat[2] - postQuat[1] * tempz;
	q[1] = tempw * postQuat[1] + postQuat[3] * tempy + tempz * postQuat[0] - postQuat[2] * tempx;
	q[2] = tempw * postQuat[2] + postQuat[3] * tempz + tempx * postQuat[1] - postQuat[0] * tempy;
	q[3] = tempw * postQuat[3] - tempx * postQuat[0] - tempy * postQuat[1] - tempz * postQuat[2];
}

void rsQuat_postMult (float *q, float preQuat[4])
{
	float tempx = q[0];
	float tempy = q[1];
	float tempz = q[2];
	float tempw = q[3];

	q[0] = preQuat[3] * tempx + tempw * preQuat[0] + preQuat[1] * tempz - tempy * preQuat[2];
	q[1] = preQuat[3] * tempy + tempw * preQuat[1] + preQuat[2] * tempx - tempz * preQuat[0];
	q[2] = preQuat[3] * tempz + tempw * preQuat[2] + preQuat[0] * tempy - tempx * preQuat[1];
	q[3] = preQuat[3] * tempw - preQuat[0] * tempx - preQuat[1] * tempy - preQuat[2] * tempz;
}

void rsQuat_toMat (float *q, float *mat)
{
	float s, xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;

	/*
	 * must have an axis 
	 */
	if (q[0] == 0.0f && q[1] == 0.0f && q[2] == 0.0f) {
		mat[0] = 1.0f;
		mat[1] = 0.0f;
		mat[2] = 0.0f;
		mat[3] = 0.0f;
		mat[4] = 0.0f;
		mat[5] = 1.0f;
		mat[6] = 0.0f;
		mat[7] = 0.0f;
		mat[8] = 0.0f;
		mat[9] = 0.0f;
		mat[10] = 1.0f;
		mat[11] = 0.0f;
		mat[12] = 0.0f;
		mat[13] = 0.0f;
		mat[14] = 0.0f;
		mat[15] = 1.0f;
		return;
	}

	s = 2.0f / (q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
	xs = q[0] * s;
	ys = q[1] * s;
	zs = q[2] * s;
	wx = q[3] * xs;
	wy = q[3] * ys;
	wz = q[3] * zs;
	xx = q[0] * xs;
	xy = q[0] * ys;
	xz = q[0] * zs;
	yy = q[1] * ys;
	yz = q[1] * zs;
	zz = q[2] * zs;

	mat[0] = 1.0f - yy - zz;
	mat[1] = xy + wz;
	mat[2] = xz - wy;
	mat[3] = 0.0f;
	mat[4] = xy - wz;
	mat[5] = 1.0f - xx - zz;
	mat[6] = yz + wx;
	mat[7] = 0.0f;
	mat[8] = xz + wy;
	mat[9] = yz - wx;
	mat[10] = 1.0f - xx - yy;
	mat[11] = 0.0f;
	mat[12] = 0.0f;
	mat[13] = 0.0f;
	mat[14] = 0.0f;
	mat[15] = 1.0f;
}
