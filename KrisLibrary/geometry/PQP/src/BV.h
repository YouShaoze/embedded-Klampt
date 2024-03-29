/*
* @File Name BV.h
* @File Path M:\MAS2\embedded Klampt\KrisLibrary\geometry\PQP\src\BV.h
* @Author: Ruige_Lee
* @Date:   2019-05-19 11:47:45
* @Last Modified by:   Ruige_Lee
* @Last Modified time: 2019-05-20 16:30:47
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

#ifndef PQP_BV_H
#define PQP_BV_H

#include <math.h>
#include "Tri.h"
#include "PQP_Compile.h"

struct BV
{
  double R[3][3];     // orientation of RSS & OBB


  double Tr[3];       // position of rectangle
  double l[2];        // side lengths of rectangle
  double r;           // radius of sphere summed with rectangle to form RSS

  double To[3];       // position of obb
  double d[3];        // (half) dimensions of obb


  int first_child;      // positive value is index of first_child bv
                        // negative value is -(index + 1) of triangle

  BV();
  ~BV();
  int      Leaf() const   { return first_child < 0; }
  double GetSize() const; 
  void     FitToTris(double O[3][3], const Tri *tris, int num_tris);
  void     FitToPointsLocal(const double (*pts)[3],int num_pts);
};

inline double BV::GetSize() const
{

  return (sqrt(l[0]*l[0] + l[1]*l[1]) + 2*r);

}

/*  Deprecated
int
BV_Overlap(double R[3][3], double T[3], BV *b1, BV *b2);

#if PQP_BV_TYPE & RSS_TYPE
double
BV_Distance(double R[3][3], double T[3], BV *b1, BV *b2);
#endif
*/

int BV_Overlap2(double R[3][3], double T[3], const BV *b1, const BV *b2);


double BV_Distance2(double R[3][3], double T[3], const BV *b1, const BV *b2);


#endif


