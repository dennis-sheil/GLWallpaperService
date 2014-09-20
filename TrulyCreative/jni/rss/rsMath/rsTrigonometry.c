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


#include <rsMath/rsTrigonometry.h>


float rs_cosine_table[256];
float rs_cosine_fraction_table[256];

float rs_sine_table[256];
float rs_sine_fraction_table[256];


void rsSinCosInit()
{
	for (int ii = 0; ii < 256; ii++)
	{
		rs_sine_table[ii] = sinf(2.0f * (float)M_PI * (float)ii / 256.0f);
		rs_cosine_table[ii] = cosf(2.0f * (float)M_PI * (float)ii / 256.0f);
	}

	for (int ii = 0; ii < 256; ii++)
	{
		rs_cosine_fraction_table[ii] = rs_cosine_table[(unsigned char)(ii + 1)] - rs_cosine_table[ii];
		rs_sine_fraction_table[ii] = rs_sine_table[(unsigned char)(ii + 1)] - rs_sine_table[ii];
	}
}
