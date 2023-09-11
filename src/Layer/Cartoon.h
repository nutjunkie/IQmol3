#include <map>
#include <iostream>
#include "Math/Vec.h"
#include "Math/Spline.h"
#include "Data/CMesh.h"
#include "Data/AminoAcid.h"

//#include "Data/PdbData.h"

using namespace IQmol;

namespace cpdb {

const int splineSteps = 32/4;
const int profileDetail = 16/4;

const double ribbonWidth = 2.0f;
const double ribbonHeight = 0.125f;
const double ribbonOffset = 1.5f;
const double arrowHeadWidth = 3.0f;
const double arrowWidth = 2.0f;
const double arrowHeight = 0.5f;
const double tubeSize = 0.35f;


Mesh createChainMesh(const int nResidues, const double *CA_OPositions, const int *ssTypePerRes);

struct Residue 
{
   int id;
   int idx;
//   char type[5];
   int ss;
//   Residue *next, *prev;
};
   

struct PeptidePlane 
{
   const Residue *Residue1;
   const Residue *Residue2;
   const Residue *Residue3;
   Math::Vec3 Position;
   Math::Vec3 Normal;
   Math::Vec3 Forward;
   Math::Vec3 Side;
   bool Flipped;
};



inline PeptidePlane NewPeptidePlane(const double *r1CA, const double *r1O,const  double *r2CA,
   int ssr1, int ssr2, char ssr3, int idr1, int idr2, int idr3)
{
    PeptidePlane newPP;

    // if(CA1 == NULL || O1 == NULL || CA2 == NULL){
    //     std::cerr<<"Failed to get all the atoms for residue "<<r1.id<<std::endl;
    //     return newPP;
    // }

    Residue *r1 = (Residue *)calloc(1, sizeof(Residue));
    r1->id = idr1;
    r1->idx = idr1++;
//    r1->next = 0;
    r1->ss = ssr1;

    Residue *r2 = (Residue *)calloc(1, sizeof(Residue));
    r2->id = idr2;
    r2->idx = idr2++;
 //   r2->next = 0;
    r2->ss = ssr2;

    Residue *r3 = (Residue *)calloc(1, sizeof(Residue));
    r3->id = idr3;
    r3->idx = idr3++;
//    r3->next = 0;
    r3->ss = ssr3;

    Math::Vec3 ca1 {r1CA[0], r1CA[1], r1CA[2]};
    Math::Vec3 o1  {r1O[0],  r1O[1],  r1O[2] };
    Math::Vec3 ca2 {r2CA[0], r2CA[1], r2CA[2]};

    Math::Vec3 a(ca2 - ca1);
    a.normalize();

    Math::Vec3 b(o1 - ca1);
    b.normalize();

    Math::Vec3 c = a^b;
    c.normalize();

    Math::Vec3 d = c^a;
    d.normalize();

    Math::Vec3 p = (ca1 + ca2)/ 2.0f;

    newPP.Residue1 = r1;
    newPP.Residue2 = r2;
    newPP.Residue3 = r3;
    newPP.Position = p;
    newPP.Normal = c;
    newPP.Forward = a;
    newPP.Side = d;
    newPP.Flipped = false;

    return newPP;
}


inline void splineForPlanes(
   Math::Vec3 *&result, 
   PeptidePlane const& p1, 
   PeptidePlane const& p2, 
   PeptidePlane const& p3, 
   PeptidePlane const& p4, 
   int n, double u, double v) 
{
    Math::Vec3 g1 = p1.Position + p1.Side*u + p1.Normal * v;
    Math::Vec3 g2 = p2.Position + p2.Side*u + p2.Normal * v;
    Math::Vec3 g3 = p3.Position + p3.Side*u + p3.Normal * v;
    Math::Vec3 g4 = p4.Position + p4.Side*u + p4.Normal * v;

    spline(result, g1, g2, g3, g4, n); 
}

} // end namespace cpdb
