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


#ifndef RSQUAT_H
#define RSQUAT_H



class rsVec;



class rsQuat{
public:
	float q[4];

	rsQuat();
	rsQuat(const float &x, const float &y, const float &z, const float &w);
	~rsQuat();
	void set(const float &x, const float &y, const float &z, const float &w);	// x, y, z, w
	void copy(const rsQuat &);							// Copy another quaternion
	void make(float a, const float &x, const float &y, const float &z);		// angle, axis
	void make(float a, const rsVec &v);
	void normalize();
	void preMult(const rsQuat &postQuat);						// Multiply this quaternion by
											// the passed quaternion
											// (this * passed)
	void postMult(const rsQuat &preQuat);						// Multiply the passed quaternion
											// by this quaternion
											// (passed * this)
	void toMat(float *) const;							// Convert quaternion to array of 16 floats
	void fromMat(float *);								// Convert array of 16 floats to quaternion
	void fromEuler(const float &, const float &, const float &);			// Convert from hpr angles
	void slerp(const rsQuat &, const rsQuat &, const float &);			// Interpolate first quaternion
											// to second using float from 0.0
											// to 1.0

	rsVec apply(const rsVec &pi) const;

	float & operator [] (int i) {return q[i];}
	const float & operator [] (int i) const {return q[i];}
};



#endif
