#include <map>
#include <iostream>
#include "Math/Vec.h"
#include "Math/Spline.h"
#include "Data/CMesh.h"
#include "Data/PdbData.h"

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


Mesh createChainMesh(const Data::chain &C);
Mesh createChainMesh(int chainId, const int *nbResPerChain, const double *CA_OPositions, 
   const char *ssTypePerRes);



struct PeptidePlane {
    const Data::residue *Residue1;
    const Data::residue *Residue2;
    const Data::residue *Residue3;
    Math::Vec3 Position;
    Math::Vec3 Normal;
    Math::Vec3 Forward;
    Math::Vec3 Side;
    bool Flipped;
};


inline PeptidePlane NewPeptidePlane(Data::residue const& r1, Data::residue const& r2, 
   Data::residue const& r3)
{
    PeptidePlane newPP;

    Data::atom const*CA1 = Data::Pdb::getAtom(r1, (char *)"CA");
    Data::atom const*O1  = Data::Pdb::getAtom(r1, (char *)"O");
    Data::atom const*CA2 = Data::Pdb::getAtom(r2, (char *)"CA");

    if(CA1 == NULL || O1 == NULL || CA2 == NULL){
        std::cerr<<"Failed to get all the atoms for residue "<<r1.id<<std::endl;
        return newPP;
    }

    Math::Vec3 ca1 = CA1->coor;
    Math::Vec3 o1  = O1->coor;
    Math::Vec3 ca2 = CA2->coor;

    Math::Vec3 a = (ca2 - ca1);
    a.normalize();

    Math::Vec3 b = (o1 - ca1);
    b.normalize();

    Math::Vec3 c = a ^ b;
    c.normalize();

    Math::Vec3 d = c ^ a; 
    d.normalize();

    Math::Vec3 p = (ca1 + ca2)/ 2.0f;
    newPP.Residue1 = &r1;
    newPP.Residue2 = &r2;
    newPP.Residue3 = &r3;

    newPP.Position = p;
    newPP.Normal = c;
    newPP.Forward = a;
    newPP.Side = d;
    newPP.Flipped = false;

    return newPP;
}


inline PeptidePlane NewPeptidePlane(const double *r1CA,const  double *r1O,const  double *r2CA,
   char ssr1, char ssr2, char ssr3, int idr1, int idr2, int idr3)
{
    PeptidePlane newPP;

    // if(CA1 == NULL || O1 == NULL || CA2 == NULL){
    //     std::cerr<<"Failed to get all the atoms for residue "<<r1.id<<std::endl;
    //     return newPP;
    // }

    Data::residue *r1 = (Data::residue *)calloc(1, sizeof(Data::residue));
    r1->id = idr1;
    r1->idx = idr1++;
    r1->next = 0;
    r1->ss = ssr1;

    Data::residue *r2 = (Data::residue *)calloc(1, sizeof(Data::residue));
    r2->id = idr2;
    r2->idx = idr2++;
    r2->next = 0;
    r2->ss = ssr2;

    Data::residue *r3 = (Data::residue *)calloc(1, sizeof(Data::residue));
    r3->id = idr3;
    r3->idx = idr3++;
    r3->next = 0;
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



inline void splineForPlanes(Math::Vec3 *&result, PeptidePlane const& p1, 
   PeptidePlane const& p2, PeptidePlane const& p3, PeptidePlane const& p4, 
   int n, double u, double v) 
                    
{
    Math::Vec3 g1 = p1.Position + p1.Side*u + p1.Normal * v;
    Math::Vec3 g2 = p2.Position + p2.Side*u + p2.Normal * v;
    Math::Vec3 g3 = p3.Position + p3.Side*u + p3.Normal * v;
    Math::Vec3 g4 = p4.Position + p4.Side*u + p4.Normal * v;

    spline(result, g1, g2, g3, g4, n); 
}

} // end namespace cpdb
