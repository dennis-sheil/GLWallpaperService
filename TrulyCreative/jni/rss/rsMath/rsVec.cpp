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


#include <rsMath/rsMath.h>
#include <math.h>



rsVec::rsVec(){
	v[0] = 0.0;
	v[1] = 0.0;
	v[2] = 0.0;
}

rsVec::rsVec(const float &xx, const float &yy, const float &zz){
	v[0] = xx;
	v[1] = yy;
	v[2] = zz;
}

rsVec::~rsVec(){
}

void rsVec::set(const float &xx, const float &yy, const float &zz){
	v[0] = xx;
	v[1] = yy;
	v[2] = zz;
}

float rsVec::length2() const{
	return(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

float rsVec::length() const{
	return(sqrtf(this->length2()));
}

float rsVec::normalize(){
	float length = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	if(length == 0.0f){
		v[1] = 1.0f;
		return(0.0f);
	}
	const float normalizer(1.0f / length);
	v[0] *= normalizer;
	v[1] *= normalizer;
	v[2] *= normalizer;
	return(length);
}

float rsVec::dot(const rsVec &vec1) const{
	return(v[0] * vec1[0] + v[1] * vec1[1] + v[2] * vec1[2]);
}

void rsVec::cross(const rsVec &vec1, const rsVec &vec2){
	v[0] = vec1[1] * vec2[2] - vec2[1] * vec1[2];
	v[1] = vec1[2] * vec2[0] - vec2[2] * vec1[0];
	v[2] = vec1[0] * vec2[1] - vec2[0] * vec1[1];
}

void rsVec::scale(const float &scale){
	v[0] *= scale;
	v[1] *= scale;
	v[2] *= scale;
}

void rsVec::transPoint(const rsMatrix &m){
	const float x = v[0];
	const float y = v[1];
	const float z = v[2];
	v[0] = x * m[0] + y * m[4] + z * m[8] + m[12];
	v[1] = x * m[1] + y * m[5] + z * m[9] + m[13];
	v[2] = x * m[2] + y * m[6] + z * m[10] + m[14];
}


void rsVec::transVec(const rsMatrix &m){
	const float x = v[0];
	const float y = v[1];
	const float z = v[2];
	v[0] = x * m[0] + y * m[4] + z * m[8];
	v[1] = x * m[1] + y * m[5] + z * m[9];
	v[2] = x * m[2] + y * m[6] + z * m[10];
}


int rsVec::almostEqual(const rsVec &vec, const float &tolerance) const{
	if(sqrtf((v[0]-vec[0])*(v[0]-vec[0])
		+ (v[1]-vec[1])*(v[1]-vec[1])
		+ (v[2]-vec[2])*(v[2]-vec[2]))
		<= tolerance)
		return 1;
	else
		return 0;
}

rsVec & rsVec::linearInterp(const rsVec &aa, const rsVec &bb, const float &ii){
	v[0] = aa[0] * (1.0f - ii) + bb[0] * ii;
	v[1] = aa[1] * (1.0f - ii) + bb[1] * ii;
	v[2] = aa[2] * (1.0f - ii) + bb[2] * ii;

	return *this;
}

