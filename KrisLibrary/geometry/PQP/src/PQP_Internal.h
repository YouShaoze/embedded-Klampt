/*
* @File Name PQP_Internal.h
* @File Path M:\MAS2\embedded Klampt\KrisLibrary\geometry\PQP\src\PQP_Internal.h
* @Author: Ruige_Lee
* @Date:   2019-05-19 11:47:45
* @Last Modified by:   Ruige_Lee
* @Last Modified time: 2019-05-20 19:20:20
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

	US Mail:             S. Gottschalk, E. Larsen
											 Department of Computer Science
											 Sitterson Hall, CB #3175
											 University of N. Carolina
											 Chapel Hill, NC 27599-3175

	Phone:               (919)962-1749

	EMail:               geom@cs.unc.edu


\**************************************************************************/

#include "Tri.h"
#include "BV.h"
#include <map>


class PQP_Model
{

public:

	int build_state;

	Tri *tris;  
	int num_tris;
	int num_tris_alloced;

	BV *b;
	int num_bvs;
	int num_bvs_alloced;
	
	BV *child(int n) { return &b[n]; }
	const BV *child(int n) const { return &b[n]; }

	PQP_Model();
	~PQP_Model();

	int BeginModel(int num_tris = 8); // preallocate for num_tris triangles;
																		// the parameter is optional, since
																		// arrays are reallocated as needed
	int AddTri(const double *p1, const double *p2, const double *p3, 
						 int id);
	int EndModel();
	int MemUsage(int msg) const;  // returns model mem usage.  
														 // prints message to stderr if msg == TRUE
};

struct CollisionPair
{
	int id1;
	int id2;
};

struct PQP_CollideResult  
{
	// stats

	int num_bv_tests;
	int num_tri_tests;

	// xform from model 1 to model 2

	double R[3][3];
	double T[3];

	int num_pairs_alloced;
	int num_pairs;
	CollisionPair *pairs;

	void SizeTo(int n);    
	void Add(int i1, int i2); 

	PQP_CollideResult();
	~PQP_CollideResult();

	// statistics

	int NumBVTests() const { return num_bv_tests; }
	int NumTriTests() const { return num_tri_tests; }

	// free the list of contact pairs; ordinarily this list is reused
	// for each query, and only deleted in the destructor.

	void FreePairsList(); 

	// query results

	int Colliding() const { return (num_pairs > 0); }
	int NumPairs() const { return num_pairs; }
	int Id1(int k) const { return pairs[k].id1; }
	int Id2(int k) const { return pairs[k].id2; }
};



struct PQP_DistanceResult 
{
	// stats

	int num_bv_tests;
	int num_tri_tests;

	// xform from model 1 to model 2

	double R[3][3];
	double T[3];

	double rel_err; 
	double abs_err; 

	double distance;
	double p1[3]; 
	double p2[3];
	int t1,t2;     // triangle indices of the closest pair
	int tid1,tid2; // id's of the triangles of the closest pair
	int qsize;
	
	// statistics

	int NumBVTests() const { return num_bv_tests; }
	int NumTriTests() const { return num_tri_tests; }

	// The following distance and points established the minimum distance
	// for the models, within the relative and absolute error bounds 
	// specified.
	// Points are defined: double p1[3], p2[3];

	double Distance() const { return distance; }
	const double *P1() const { return p1; }
	const double *P2() const { return p2; }
};

struct PQP_ToleranceResult 
{
	// stats

	int num_bv_tests;
	int num_tri_tests;

	// xform from model 1 to model 2

	double R[3][3];
	double T[3];

	int    closer_than_tolerance;   
	double tolerance;      

	double distance;
	double p1[3]; 
	double p2[3]; 
	int t1,t2;     // triangle indices of the closest pair
	int tid1,tid2; // id's of the triangles of the closest pair
	int qsize;

	// statistics

	int NumBVTests() { return num_bv_tests; }
	int NumTriTests() { return num_tri_tests; }

	// If the models are closer than ( <= ) tolerance, these points 
	// and distance were what established this.  Otherwise, 
	// distance and point values are not meaningful.

	double Distance() { return distance; }
	const double *P1() { return p1; }
	const double *P2() { return p2; }

	// boolean says whether models are closer than tolerance distance

	int CloserThanTolerance() { return closer_than_tolerance; }
};

struct PQP_ClosestPoints
{
	double p1[3];
	double p2[3];
};

struct PQP_ToleranceAllResult
{
	/*
	std::vector<double> triDist1,triDist2;       //closest distance per triangle
	std::vector<int> triPartner1,triPartner2;  //closest triangle on the other
	std::vector<PQP_ClosestPoints> triCp1,triCp2;   //closest points per triangle
	*/
	std::map<int,double> triDist1,triDist2;       //closest distance per triangle
	std::map<int,int> triPartner1,triPartner2;  //closest triangle on the other
	std::map<int,PQP_ClosestPoints> triCp1,triCp2;   //closest points per triangle
};


