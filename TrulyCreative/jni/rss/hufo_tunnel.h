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

#ifndef HUFO_TUNNEL_H
#define HUFO_TUNNEL_H

#include "rsRand.h"
#include <rsMath/rsVec.h>
#include <rsMath/rsQuat.h>

#ifdef _DEBUG
#include <assert.h>
#endif

// sorry but the following comments are in french, no time to translate :-(

#define HVCtrX 0.0
#define HVCtrY 0.0
float HoleFoc = 2.0f;

#define HoleFocX HoleFoc
#define HoleFocY HoleFoc

#define SHFTHTPS 8
#define HOLEGEN 25
#define HoleNbParImg 64
#define HoleNbImg 128
#define HOLEVIT 8

#define TUNNELCOLORRND 0.15f
#define TUNNELCOLORFACT 0.9999f

int dCoarse;

struct THole {
	float u, v;		// position du point sur le plan
	float c1, c2;		// color coefficients
};
THole Hole[HoleNbImg][HoleNbParImg];
THole RefHole[HoleNbParImg];

struct THoleTraj {
	rsVec a;		// angles
	rsVec o, m, n;	// vecteurs base du plan
	float s;		// taille du cercle
};
THoleTraj HoleTraj[HoleNbImg];

void HoleInitPlan (int p, int t, float ss = 1.0)
{
	float c1, c2;
	float s = ss;

	// calc color and position

	for (int i = 0; i < HoleNbParImg; i++) {
		if (dCoarse) {
			c1 = RefHole[i].c1 + rsRandf (2 * TUNNELCOLORRND) - TUNNELCOLORRND;
			if (c1 < 0)
				c1 = 0;
			else if (c1 > 1)
				c1 = 1;
			c2 = RefHole[i].c2 + rsRandf (2 * TUNNELCOLORRND) - TUNNELCOLORRND;
			if (c2 < 0)
				c2 = 0;
			else if (c2 > 1)
				c2 = 1;

			Hole[p][i].c1 = c1;
			Hole[p][i].c2 = c2;
			s = ss * (2.0f - c1);
		}

		Hole[p][i].u = RefHole[i].u * s;
		Hole[p][i].v = RefHole[i].v * s;
	}

	if (dCoarse) {
		// color smoothing
		for (int i = 0; i < HoleNbParImg; i++) {
			RefHole[i].c1 = (Hole[p][i].c1 + Hole[p][(i + 1) & (HoleNbParImg - 1)].c1 + Hole[p][(i - 1) & (HoleNbParImg - 1)].c1) * TUNNELCOLORFACT / 3;
			RefHole[i].c2 = (Hole[p][i].c2 + Hole[p][(i + 1) & (HoleNbParImg - 1)].c2 + Hole[p][(i - 1) & (HoleNbParImg - 1)].c2) * TUNNELCOLORFACT / 3;
		}
	}

	HoleTraj[p].s = s * HOLEGEN;

	if (dCoarse) {
		// calc trajectory (based on c2)

		static float c2a_0 = 0;
		static float c2b_0 = 0;

#define C2_VIT 0.01f

		float c2a = RefHole[0].c2;
		float c2b = RefHole[HoleNbParImg / 2].c2;

		if (c2a < c2a_0 - C2_VIT)
			c2a_0 -= C2_VIT;
		else if (c2a > c2a_0 + C2_VIT)
			c2a_0 += C2_VIT;
		else
			c2a_0 = c2a;

		if (c2b < c2b_0 - C2_VIT)
			c2b_0 -= C2_VIT;
		else if (c2b > c2b_0 + C2_VIT)
			c2b_0 += C2_VIT;
		else
			c2b_0 = c2a;

		float az = 2 * c2a_0 - 1.0f;

		az = az * 2;
		float ax = 2 * c2b_0 - 1.0f;

		ax = ax * 2;

		HoleTraj[p].a.y() = 0;	//(float)(sin(t*M_PI/90)*4*HOLEVIT*M_PI/1500);
		HoleTraj[p].a.z() = (az * az * az * 2) * HOLEVIT * M_PI / 2500;	//(float)(sin(t*M_PI/170)*2*HOLEVIT*M_PI/2500);
		HoleTraj[p].a.x() = (ax * ax * ax * 2) * HOLEVIT * M_PI / 2500;	//(float)(sin(t*M_PI/170)*2*HOLEVIT*M_PI/2500);
	} else {
		HoleTraj[p].a.x() = (float)(sin (t * M_PI / 40) * HOLEVIT * M_PI / 1500);
		HoleTraj[p].a.y() = (float)(sin (t * M_PI / 90) * 4 * HOLEVIT * M_PI / 1500);
		HoleTraj[p].a.z() = (float)(sin (t * M_PI / 70) * 2 * HOLEVIT * M_PI / 1500);
	}
}

int HoleLastP;			// dernier plan calculé
int HoleNbImgA;			// nombre de plans affichés
bool dSinHole = false;
bool StopHole = false;
void HoleInit ()
{
	int i;

	for (i = 0; i < HoleNbParImg; i++) {
		RefHole[i].u = (float)(sin (i * (2 * M_PI / HoleNbParImg)) * HOLEGEN);
		RefHole[i].v = (float)(cos (i * (2 * M_PI / HoleNbParImg)) * HOLEGEN);
		RefHole[i].c1 = 0;
		RefHole[i].c2 = 0;
	}
	HoleLastP = -1;
}

struct BBox2D {
	float u0, v0;
	float u1, v1;
};

/*
 voir calculs a la fin
*/
void InterLnCircle (double u, double v, double w, float *x1, float *y1, float *x2, float *y2)
{
	double d = 1.0 / sqrt (u * u + v * v);

	u *= d;
	v *= d;
	w *= d;
#ifdef _DEBUG
	assert (abs (w) < 1.0);
#endif
	d = sqrt (1.0 - w * w);
	*x1 = w * u - d * v;
	*y1 = w * v + d * u;
	*x2 = w * u + d * v;
	*y2 = w * v - d * u;
#ifdef _DEBUG
	assert (abs ((*x1) * u + (*y1) * v - w) < 1e-6);
	assert (abs ((*x2) * u + (*y2) * v - w) < 1e-6);
	assert (abs (sqr (*x1) + sqr (*y1) - 1.0) < 1e-6);
	assert (abs (sqr (*x2) + sqr (*y2) - 1.0) < 1e-6);
#endif
}

void CalcBBoxPlan (int p, BBox2D * b)
{
	rsVec o, m, n;

	o = HoleTraj[p].o;
	m = HoleTraj[p].m * HoleTraj[p].s;
	n = HoleTraj[p].n * HoleTraj[p].s;
	float ca1, ca2, sa1, sa2;
	float f1, f2;

	//ca(mxoy-myox)+sa(nyox-nxoy)=mynx-mxny
	InterLnCircle (o.y() * m.x() - o.x() * m.y(), o.x() * n.y() - o.y() * n.x(), m.y() * n.x() - m.x() * n.y(), &ca1, &sa1, &ca2, &sa2);
	f1 = HVCtrX + HoleFocX * (o.x() + m.x() * sa1 + n.x() * ca1) / (o.y() + m.y() * sa1 + n.y() * ca1);
	f2 = HVCtrX + HoleFocX * (o.x() + m.x() * sa2 + n.x() * ca2) / (o.y() + m.y() * sa2 + n.y() * ca2);
	if (f1 < f2) {
		b->u0 = f1;
		b->u1 = f2;
	} else {
		b->u0 = f2;
		b->u1 = f1;
	}
	//ca(mzoy-myoz)+sa(nyoz-nzoy)=mynz-mzny
	InterLnCircle (o.y() * m.z() - o.z() * m.y(), o.z() * n.y() - o.y() * n.z(), m.y() * n.z() - m.z() * n.y(), &ca1, &sa1, &ca2, &sa2);
	f1 = HVCtrY - HoleFocY * (o.z() + m.z() * sa1 + n.z() * ca1) / (o.y() + m.y() * sa1 + n.y() * ca1);
	f2 = HVCtrY - HoleFocY * (o.z() + m.z() * sa2 + n.z() * ca2) / (o.y() + m.y() * sa2 + n.y() * ca2);
	if (f1 < f2) {
		b->v0 = f1;
		b->v1 = f2;
	} else {
		b->v0 = f2;
		b->v1 = f1;
	}
}
void InterBBox (BBox2D * a, BBox2D const *b)
{
#define max(a, b) ((a > b) ? a : b);
#define min(a, b) ((a > b) ? b : a);

	a->u0 = max (a->u0, b->u0);
	a->u1 = min (a->u1, b->u1);
	a->v0 = max (a->v0, b->v0);
	a->v1 = min (a->v1, b->v1);
}
bool BBoxEmpty (BBox2D const *b)
{
	return (b->u0 > b->u1 || b->v0 > b->v1);
}

void MkBBoxAll (BBox2D * b)
{
	b->u0 = -1e10;
	b->u1 = 1e10;
	b->v0 = -1e10;
	b->v1 = 1e10;
}

BBox2D BBPlan[HoleNbImg];

struct HPT {
	float ex, ey;
	float u, v;
	float c1;
} Pt[HoleNbImg][HoleNbParImg + 1];
float PtDist[HoleNbImg];

void CalcHole (int T)
{
	float ft = (T & ((1 << SHFTHTPS) - 1)) * (1.0 / (1 << SHFTHTPS));
	int it = T >> SHFTHTPS;
	int i, p, s;

	// Premiere etape : calcul de la position des plans
	rsVec o (0, 0, 0);
	rsVec m (1, 0, 0);
	rsVec v (0, HOLEVIT, 0);
	rsVec n (0, 0, 1);
	rsQuat mm;
	BBox2D bplan, bhole;

	MkBBoxAll (&bhole);
	for (i = 0; i < HoleNbImg && !BBoxEmpty (&bhole); i++) {
		p = (i + it) & (HoleNbImg - 1);
		if (i + it > HoleLastP) {	// le plan n'a pas été calculé
			if (StopHole)
				break;	// on ne calcule plus de plan
			HoleInitPlan (p, i + it, (dSinHole ? (sin ((i + it) * M_PI / 10) + 4.0) / 4 : 1.0));
			HoleLastP = i + it;
		}
		if (!i)
			o += v * (1 - ft);
		else
			o += v;
		HoleTraj[p].o = o;
		HoleTraj[p].m = m;
		HoleTraj[p].n = n;
		if (!i) {
			mm.fromEuler (HoleTraj[p].a.x() * (1 - ft), HoleTraj[p].a.y() * (1 - ft), HoleTraj[p].a.z() * (1 - ft));
		} else {
			mm.fromEuler (HoleTraj[p].a.x(), HoleTraj[p].a.y(), HoleTraj[p].a.z());
		}
		m = mm.apply(m);
		v = mm.apply(v);
		n = mm.apply(n);
		CalcBBoxPlan (p, &bplan);
		BBPlan[i] = bhole;
		InterBBox (&bhole, &bplan);
	}
	HoleNbImgA = i;
	if (!HoleNbImgA)
		return;
	// Deuxieme etape : position des points
	// Equations:
	// un point de coordonnees (u,v) du plan (O,m,n)
	// on a : s>=o>+u*m>+v*n>
	// px=ox+u*mx+v*nx
	// py=oy+u*my+v*ny
	// pz=oz+u*mz+v*nz
	rsVec pp;
	float txtv;
	float pu, pv;

//      for (i=HoleNbImgA-1;i>=0;i--)
	for (i = 0; i < HoleNbImgA; ++i) {
		PtDist[i] = i + 1.0 - ft;
		p = (i + it) & (HoleNbImg - 1);
		o = HoleTraj[p].o;
		m = HoleTraj[p].m;
		n = HoleTraj[p].n;
		txtv = (i + it) * (2.0 / HoleNbImg);
		for (s = 0; s <= HoleNbParImg; s += ((dCoarse > 0) ? dCoarse : 1)) {
			pu = Hole[p][s & (HoleNbParImg - 1)].u;
			pv = Hole[p][s & (HoleNbParImg - 1)].v;
			//pp=o+m*pu+n*pv;
			pp.x() = o.x() + pu * m.x() + pv * n.x();
			pp.y() = o.y() + pu * m.y() + pv * n.y();
			pp.z() = o.z() + pu * m.z() + pv * n.z();
			if (pp.y() <= 0)
				pp.y() = 0.1f;	// en cas de probleme
			Pt[i][s].ex = HVCtrX + HoleFocX * pp.x() / pp.y();
			Pt[i][s].ey = HVCtrY - HoleFocY * pp.z() / pp.y();
			if (dCoarse)
				Pt[i][s].c1 = 0.25f + 0.75f * Hole[p][s & (HoleNbParImg - 1)].c1;
			Pt[i][s].u = s * (1.0 / HoleNbParImg);
			Pt[i][s].v = txtv;
		}
	}
}

/*
Optimisation de l'affichage du tunnel:
le tunnel est constitué de cercles.
on peut arreter l'affichage du  tunnel quand le reste cache tout l'écran
si on garde une zone de visibilité qui montre le zone encore visible de l'écran
après l'affichage d'une partir du tunnel, on s'arrete quand cete zone est vide.
On a donc besoin:
_ d'une fonction qui calcul la zone du trou du tunnel apres l'affichage d'un plan donné
_ d'une fonction qui calcule l'intersection de deux zones
Pour leur simplicité on va choisir des rectangles
*/

/* calcul du rectangle projeté
  les points suivant sont sur le cercle: P=O+M*sin(a)+N*cos(a)
	Px=ox+sa*mx+ca+nx Py=oy+sa*my+ca+ny Pz=oz+sa*mz+ca+nz
	ex=cx+fx*Px/Py
	ey=cy-fy*Pz/Py
	on cherche les bornes de ex et de ey
	ex=exmin ou exmax <=> dex/da=0
	ey=eymin ou eymax <=> dey/da=0
	dex/da=fx*(Py * dPx/da - Px * dPy/da) / Py² = 0
	<=> Py * dPx/da - Px * dPy/da = 0
	(camx-sanx)(oy+samy+cany)-(ox+samx+canx)(camy-sany)=0
	 camxoy+casamxmy+ca²mxny-sanxoy-sa²nxmy-casanxny
	-camyox-casamymx-ca²mynx+sanyox+sa²nymx+casanxny   =0
	ca(mxoy-myox)+sa(nyox-nxoy)+(ca²+sa²)(mxny-mynx)   =0
	ca(mxoy-myox)+sa(nyox-nxoy)=mynx-mxny

	(oymx-oxmy)ca + (-oynx+oxny)sa + (mxny-mynx)ca² + (mxny-mynx)sa² =0
	(oymx-oxmy)ca + (-oynx+oxny)sa = mynx-mxny et ca²+sa²=1
	U*ca+V*sa=W et ca²+sa²=1

	Ux+Vy=W d=1/sqrt(U²+V²),u=U*d,v=V*d,w=W*d => ux+vy=w (u²+v²=1)
	distance entre la droite et l'origine:d=abs(w)
	point a cette distance:
	vx-uy=0 && ux+vy=w
	=> x=uw
	=> y=vw
	si d>1 -> erreur
	les solutions sont (x,y)+-(1-d)*(-v,u)
	x1=uw-sqrt(1-sqr(d))*v y1=vw+sqrt(1-sqr(d))*u
	x2=uw+sqrt(1-sqr(d))*v y2=vw-sqrt(1-sqr(d))*u
*/

#endif
