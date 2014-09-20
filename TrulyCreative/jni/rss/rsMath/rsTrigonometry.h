/*
 * Copyright (C) 1999-2005  Terence M. Welsh
 *
 * This file is part of rsMath.
 *
 * rsMath is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation.
 *
 * rsMath is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#ifndef RSTRIGONOMETRY_H
#define RSTRIGONOMETRY_H

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif 

// The bias 2^22 is divided by 2^16 and multiplied by 2*M_PI during remapping
#define RS_SINCOS_MAX (64 * 2 * M_PI)
// This technique is only valid while the exponent is 23, otherwise the bits 
// in the mantissa are shifted


extern float rs_cosine_table[256];
extern float rs_cosine_fraction_table[256];

extern float rs_sine_table[256];
extern float rs_sine_fraction_table[256];


void rsSinCosInit();


// union to help with fast typecasting from int to float
typedef union{
	unsigned int i;
	float f;
} bytes_or_float;


// Remap value from {0,2pi} to {0,65536} and add fast typecast bias
// Use low-order byte for fractional multiplier
// Use high-order byte for table lookup
#define common() \
        bytes_or_float bof; \
        bof.f = value * (65536.0f / (2.0f * (float)M_PI)) + 12582912.0f; \
        const float fraction = (float)(unsigned char)bof.i / 256.0f; \
        const unsigned char i = bof.i >> 8; 

// Fast trig functions
// They're not as precise as cosf and sinf, but they're stupid fast.
static inline float rsCosf(const float value)
{
	common()
        // Do it
        return rs_cosine_table[i] + fraction * rs_cosine_fraction_table[i];
}

static inline float rsSinf(const float value)
{
	common()
        // Do it
        return rs_sine_table[i] + fraction * rs_sine_fraction_table[i];
}

static inline void rsSinCosf(const float value, float * const s, float * const c)
{
	common()
        // Do it
        *c = rs_cosine_table[i] + fraction * rs_cosine_fraction_table[i];
        *s = rs_sine_table[i] + fraction * rs_sine_fraction_table[i];
}


#ifdef __cplusplus
}
#endif 

#endif

