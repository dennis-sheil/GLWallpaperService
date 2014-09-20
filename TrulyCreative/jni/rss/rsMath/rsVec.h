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


#ifndef RSVEC_H
#define RSVEC_H



class rsMatrix;



class rsVec{
public:
	float v[3];

	rsVec();
	rsVec(const float &xx, const float &yy, const float &zz);
	virtual ~rsVec();
	void set(const float &xx, const float &yy, const float &zz);
	float length() const;
	float length2() const;
	float normalize();
	float dot(const rsVec &) const;
	void cross(const rsVec &, const rsVec &);
	void scale(const float &);
	void transPoint(const rsMatrix &m);
	void transVec(const rsMatrix &m);
	int almostEqual(const rsVec &vec, const float &tolerance) const;
	rsVec & linearInterp(const rsVec &, const rsVec &, const float &);

	float & operator [] (const int i) {return v[i];}
	const float & operator [] (const int i) const {return v[i];}
	float & x() {return v[0];}
	float & y() {return v[1];}
	float & z() {return v[2];}
	rsVec & zero ()
		{v[0]=0;v[1]=0;v[2]=0;return *this;}
	rsVec & operator = (const rsVec &vec)
		{v[0]=vec[0];v[1]=vec[1];v[2]=vec[2];return *this;}
	rsVec operator + (const rsVec &vec) const
		{return(rsVec(v[0]+vec[0], v[1]+vec[1], v[2]+vec[2]));}
	rsVec operator - (const rsVec &vec) const
		{return(rsVec(v[0]-vec[0], v[1]-vec[1], v[2]-vec[2]));}
	rsVec operator * (const float &mul) const
		{return(rsVec(v[0]*mul, v[1]*mul, v[2]*mul));}
	rsVec operator / (const float &div) const
		{float rec = 1.0f/div; return(rsVec(v[0]*rec, v[1]*rec, v[2]*rec));}
	rsVec & operator += (const rsVec &vec)
		{v[0]+=vec[0];v[1]+=vec[1];v[2]+=vec[2];return *this;}
	rsVec & operator -= (const rsVec &vec)
		{v[0]-=vec[0];v[1]-=vec[1];v[2]-=vec[2];return *this;}
	rsVec & operator *= (const rsVec &vec)
		{v[0]*=vec[0];v[1]*=vec[1];v[2]*=vec[2];return *this;}
	rsVec & operator *= (const float &mul)
		{v[0]*=mul;v[1]*=mul;v[2]*=mul;return *this;}
};



#endif
