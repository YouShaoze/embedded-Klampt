/*
* @File Name BV.cpp
* @File Path M:\MAS2\embedded Klampt\KrisLibrary\geometry\PQP\src\BV.cpp
* @Author: Ruige_Lee
* @Date:   2019-05-19 11:47:45
* @Last Modified by:   Ruige_Lee
* @Last Modified time: 2019-05-20 16:10:15
* @Email: 295054118@whut.edu.cn
* @page: https://whutddk.github.io/
*/
/*************************************************************************\

  Copyright 1999 The University of North Carolina at Chapel Hill.
  All Rights Reserved.

  Permission to use, copy, modify and distribute this software and its
  documentation for educational, research and non-profit purposes, without
  fee, and without a written agreement is hereby granted, provided that the
  above copyright notice and the following three paragraphs appear in all
  copies.

  IN NO EVENT SHALL THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL BE
  LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
  CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE
  USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY
  OF NORTH CAROLINA HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH
  DAMAGES.

  THE UNIVERSITY OF NORTH CAROLINA SPECIFICALLY DISCLAIM ANY
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE
  PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
  NORTH CAROLINA HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT,
  UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

  The authors may be contacted via:

  US Mail:             E. Larsen
                       Department of Computer Science
                       Sitterson Hall, CB #3175
                       University of N. Carolina
                       Chapel Hill, NC 27599-3175

  Phone:               (919)962-1749

  EMail:               geom@cs.unc.edu


\**************************************************************************/

#include <stdlib.h>
#include <math.h>
#include "BV.h"
#include "MatVec.h"
#include "RectDist.h"
#include "OBB_Disjoint.h"

BV::BV()
{
  first_child = 0;
}

BV::~BV()
{
}

static
inline 
double 
MaxOfTwo(double a, double b) 
{
  if (a > b) return a;
  return b;
}

void
BV::FitToTris(double O[3][3], const Tri *tris, int num_tris)
{
  // store orientation

  McM(R,O);
 
#if PQP_BV_TYPE & RSS_TYPE
  int num_points = 3*num_tris;
  double (*P)[3] = new double[num_points][3];

  int i;
  int point = 0;
  for (i = 0; i < num_tris; i++) 
  {
    MTxV(P[point],R,tris[i].p1);
    point++;

    MTxV(P[point],R,tris[i].p2);
    point++;

    MTxV(P[point],R,tris[i].p3);
    point++;
  }
  FitToPointsLocal(P,num_tris*3);

  delete [] P;
#elif PQP_BV_TYPE & OBB_TYPE

  // project points of tris to R coordinates

  double minx, maxx, miny, maxy, minz, maxz, c[3];
  int i;

  MTxV(c,R,tris[0].p1);
  minx = maxx = c[0];
  miny = maxy = c[1];
  minz = maxz = c[2];
  for (i = 0; i < num_tris; i++)
  {
    MTxV(c,R,tris[i].p1);
    if (c[0] < minx) minx = c[0];
    else if (c[0] > maxx) maxx = c[0];
    if (c[1] < miny) miny = c[1];
    else if (c[1] > maxy) maxy = c[1];
    if (c[2] < minz) minz = c[2];
    else if (c[2] > maxz) maxz = c[2];
    MTxV(c,R,tris[i].p2);
    if (c[0] < minx) minx = c[0];
    else if (c[0] > maxx) maxx = c[0];
    if (c[1] < miny) miny = c[1];
    else if (c[1] > maxy) maxy = c[1];
    if (c[2] < minz) minz = c[2];
    else if (c[2] > maxz) maxz = c[2];
    MTxV(c,R,tris[i].p3);
    if (c[0] < minx) minx = c[0];
    else if (c[0] > maxx) maxx = c[0];
    if (c[1] < miny) miny = c[1];
    else if (c[1] > maxy) maxy = c[1];
    if (c[2] < minz) minz = c[2];
    else if (c[2] > maxz) maxz = c[2];
  }
  c[0] = (double)0.5*(maxx + minx);
  c[1] = (double)0.5*(maxy + miny);
  c[2] = (double)0.5*(maxz + minz);
  MxV(To,R,c);

  d[0] = (double)0.5*(maxx - minx);
  d[1] = (double)0.5*(maxy - miny);
  d[2] = (double)0.5*(maxz - minz);
#endif
}

void BV::FitToPointsLocal(const double (*P)[3],int num_points)
{
  // project points of tris to R coordinates

  double minx, maxx, miny, maxy, minz, maxz, c[3];
  int i;

#if PQP_BV_TYPE & OBB_TYPE
  minx = maxx = P[0][0];
  miny = maxy = P[0][1];
  minz = maxz = P[0][2];
  for (i = 1; i < num_points; i++)
  {
    if (P[i][0] < minx) minx = P[i][0];
    else if (P[i][0] > maxx) maxx = P[i][0];
    if (P[i][1] < miny) miny = P[i][1];
    else if (P[i][1] > maxy) maxy = P[i][1];
    if (P[i][2] < minz) minz = P[i][2];
    else if (P[i][2] > maxz) maxz = P[i][2];
  }
  c[0] = (double)0.5*(maxx + minx);
  c[1] = (double)0.5*(maxy + miny);
  c[2] = (double)0.5*(maxz + minz);
  MxV(To,R,c);

  d[0] = (double)0.5*(maxx - minx);
  d[1] = (double)0.5*(maxy - miny);
  d[2] = (double)0.5*(maxz - minz);
#endif

#if PQP_BV_TYPE & RSS_TYPE
  // compute thickness, which determines radius, and z of rectangle corner
  
  double cz,radsqr;
  minz = maxz = P[0][2];
  for (i = 1; i < num_points; i++) 
  {
    if (P[i][2] < minz) minz = P[i][2];
    else if (P[i][2] > maxz) maxz = P[i][2];
  }
  r = (double)0.5*(maxz - minz);
  radsqr = r*r;
  cz = (double)0.5*(maxz + minz);

  // compute an initial length of rectangle along x direction

  // find minx and maxx as starting points

  int minindex, maxindex;
  minindex = maxindex = 0;
  for (i = 1; i < num_points; i++) 
  {
    if (P[i][0] < P[minindex][0]) minindex = i; 
    else if (P[i][0] > P[maxindex][0]) maxindex = i;
  }
  double x, dz;
  dz = P[minindex][2] - cz;
  minx = P[minindex][0] + sqrt(MaxOfTwo(radsqr - dz*dz,0));
  dz = P[maxindex][2] - cz;
  maxx = P[maxindex][0] - sqrt(MaxOfTwo(radsqr - dz*dz,0));

  // grow minx

  for (i = 0; i < num_points; i++) 
  {
    if (P[i][0] < minx) 
    {
      dz = P[i][2] - cz;
      x = P[i][0] + sqrt(MaxOfTwo(radsqr - dz*dz,0));
      if (x < minx) minx = x;
    }
  }

  // grow maxx

  for (i = 0; i < num_points; i++) 
  {
    if (P[i][0] > maxx) 
    {
      dz = P[i][2] - cz;
      x = P[i][0] - sqrt(MaxOfTwo(radsqr - dz*dz,0));
      if (x > maxx) maxx = x;
    }
  }
  
  // compute an initial length of rectangle along y direction

  // find miny and maxy as starting points

  minindex = maxindex = 0;
  for (i = 1; i < num_points; i++) 
  {
    if (P[i][1] < P[minindex][1]) minindex = i;
    else if (P[i][1] > P[maxindex][1]) maxindex = i;
  }
  double y;
  dz = P[minindex][2] - cz;
  miny = P[minindex][1] + sqrt(MaxOfTwo(radsqr - dz*dz,0));
  dz = P[maxindex][2] - cz;
  maxy = P[maxindex][1] - sqrt(MaxOfTwo(radsqr - dz*dz,0));

  // grow miny

  for (i = 0; i < num_points; i++) 
  {
    if (P[i][1] < miny) 
    {
      dz = P[i][2] - cz;
      y = P[i][1] + sqrt(MaxOfTwo(radsqr - dz*dz,0));
      if (y < miny) miny = y;
    }
  }

  // grow maxy

  for (i = 0; i < num_points; i++) 
  {
    if (P[i][1] > maxy) 
    {
      dz = P[i][2] - cz;
      y = P[i][1] - sqrt(MaxOfTwo(radsqr - dz*dz,0));
      if (y > maxy) maxy = y;
    }
  }
  
  // corners may have some points which are not covered - grow lengths if
  // necessary
  
  double dx, dy, u, t;
  double a = sqrt((double)0.5);
  for (i = 0; i < num_points; i++) 
  {
    if (P[i][0] > maxx) 
    {
      if (P[i][1] > maxy) 
      {
        dx = P[i][0] - maxx;
        dy = P[i][1] - maxy;
        u = dx*a + dy*a;
        t = (a*u - dx)*(a*u - dx) + 
            (a*u - dy)*(a*u - dy) +
            (cz - P[i][2])*(cz - P[i][2]);
        u = u - sqrt(MaxOfTwo(radsqr - t,0));
        if (u > 0) 
        {
          maxx += u*a;
          maxy += u*a;
        }
      }
      else if (P[i][1] < miny) 
      {
        dx = P[i][0] - maxx;
        dy = P[i][1] - miny;
        u = dx*a - dy*a;
        t = (a*u - dx)*(a*u - dx) + 
            (-a*u - dy)*(-a*u - dy) +
            (cz - P[i][2])*(cz - P[i][2]);
        u = u - sqrt(MaxOfTwo(radsqr - t,0));
        if (u > 0) 
        {
          maxx += u*a;
          miny -= u*a;
        }
      }
    }
    else if (P[i][0] < minx) 
    {
      if (P[i][1] > maxy) 
      {
        dx = P[i][0] - minx;
        dy = P[i][1] - maxy;
        u = dy*a - dx*a;
        t = (-a*u - dx)*(-a*u - dx) + 
            (a*u - dy)*(a*u - dy) +
            (cz - P[i][2])*(cz - P[i][2]);
        u = u - sqrt(MaxOfTwo(radsqr - t,0));
        if (u > 0) 
        {
          minx -= u*a;
          maxy += u*a;
        }     
      }
      else if (P[i][1] < miny) 
      {
        dx = P[i][0] - minx;
        dy = P[i][1] - miny;
        u = -dx*a - dy*a;
        t = (-a*u - dx)*(-a*u - dx) + 
            (-a*u - dy)*(-a*u - dy) +
            (cz - P[i][2])*(cz - P[i][2]);
        u = u - sqrt(MaxOfTwo(radsqr - t,0));
        if (u > 0) 
        {
          minx -= u*a; 
          miny -= u*a;
        }
      }
    }
  }

  c[0] = minx;
  c[1] = miny;
  c[2] = cz;
  MxV(Tr,R,c);

  l[0] = maxx - minx;  
  if (l[0] < 0) l[0] = 0;
  l[1] = maxy - miny;
  if (l[1] < 0) l[1] = 0;
#endif
}

/*
int 
BV_Overlap(double R[3][3], double T[3], BV *b1, BV *b2)
{
#if PQP_BV_TYPE & OBB_TYPE
  return (obb_disjoint(R,T,b1->d,b2->d) == 0);
#else
  double dist = RectDist(R,T,b1->l,b2->l);
  if (dist <= (b1->r + b2->r)) return 1;
  return 0;
#endif
}

#if PQP_BV_TYPE & RSS_TYPE
double
BV_Distance(double R[3][3], double T[3], BV *b1, BV *b2)
{
  double dist = RectDist(R,T,b1->l,b2->l);
  dist -= (b1->r + b2->r);
  return (dist < (double)0.0)? (double)0.0 : dist;
}
#endif
*/


//performs tranformation A^{-1}*B*C
inline void TransformMulAInvBC(const double Ra[3][3], const double Ta[3],
			       const double Rb[3][3], const double Tb[3],
			       const double Rc[3][3], const double Tc[3],
			       double R[3][3], double T[3])
{
  double Rtemp[3][3],Ttemp[3];
  //multiply BC
  MxM(Rtemp,Rb,Rc);
  MxV(Ttemp,Rb,Tc);
  VpV(Ttemp,Ttemp,Tb);
  //multiply A^{-1}BC
  MTxM(R,Ra,Rtemp);
  MTxV(T,Ra,Ttemp);
  //translation of A^{-1} = - Ra^T * Ta
  MTxV(Ttemp,Ra,Ta);
  VmV(T,T,Ttemp);
}

int 
BV_Overlap2(double R[3][3], double T[3], const BV *b1, const BV *b2)
{
  //T1^-1 * T * T2
  double Rtemp[3][3];
  double Ttemp[3];
#if PQP_BV_TYPE & OBB_TYPE
  TransformMulAInvBC(b1->R,b1->To,R,T,b2->R,b2->To,Rtemp,Ttemp);
  return (obb_disjoint(Rtemp,Ttemp,b1->d,b2->d) == 0);
#else
  TransformMulAInvBC(b1->R,b1->Tr,R,T,b2->R,b2->Tr,Rtemp,Ttemp);
  double dist = RectDist(Rtemp,Ttemp,b1->l,b2->l);
  if (dist <= (b1->r + b2->r)) return 1;
  return 0;
#endif
}

#if PQP_BV_TYPE & RSS_TYPE
double
BV_Distance2(double R[3][3], double T[3], const BV *b1, const BV *b2)
{
  //T1^-1 * T * T2
  double Rtemp[3][3];
  double Ttemp[3];
  TransformMulAInvBC(b1->R,b1->Tr,R,T,b2->R,b2->Tr,Rtemp,Ttemp);
  double dist = RectDist(Rtemp,Ttemp,b1->l,b2->l);
  dist -= (b1->r + b2->r);
  return (dist < (double)0.0)? (double)0.0 : dist;
}
#endif
