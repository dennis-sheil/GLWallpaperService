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

#ifndef RSMATH_H
#define RSMATH_H

#include <math.h>
#include <stdlib.h>

#include "config.h"

float rsVec_length (float *v);
float rsVec_normalize (float *v);
void rsVec_cross (float *v, float vec1[3], float vec2[3]);
void rsVec_scale (float *v, float scale);
#define rsVec_dot(x, y) (x[0] * y[0] + x[1] * y[1] + x[2] * y[2])
void rsVec_copy (float v[3], float *dest);
void rsVec_add (float v[3], float vec[3], float *dest);
void rsVec_subtract (float v[3], float vec[3], float *dest);

void rsQuat_make (float *q, float a, float x, float y, float z);
void rsQuat_preMult (float *q, float postQuat[4]);
void rsQuat_postMult (float *q, float preQuat[4]);
void rsQuat_toMat (float *q, float *mat);

#endif /* RSMATH_H */
