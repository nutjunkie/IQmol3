#include <map>
#include <iostream>
#include "Math/v3.h"
#include "Math/Spline.h"
#include "Data/CMesh.h"
#include "Data/PdbData.h"

using namespace IQmol;

namespace cpdb {

const int splineSteps = 32/4;
const int profileDetail = 16/4;

const float ribbonWidth = 2.0f;
const float ribbonHeight = 0.125f;
const float ribbonOffset = 1.5f;
const float arrowHeadWidth = 3.0f;
const float arrowWidth = 2.0f;
const float arrowHeight = 0.5f;
const float tubeSize = 0.35f;


Mesh createChainMesh(const Data::chain &C);
Mesh createChainMesh(int chainId, const int *nbResPerChain, const float *CA_OPositions, 
   const char *ssTypePerRes);



struct PeptidePlane {
    const Data::residue *Residue1;
    const Data::residue *Residue2;
    const Data::residue *Residue3;
    v3 Position;
    v3 Normal;
    v3 Forward;
    v3 Side;
    bool Flipped;
};


inline PeptidePlane NewPeptidePlane(const Data::residue &r1, const Data::residue &r2, 
   const Data::residue &r3)
{
    PeptidePlane newPP;

    Data::atom const*CA1 = Data::Pdb::getAtom(r1, (char *)"CA");
    Data::atom const*O1  = Data::Pdb::getAtom(r1, (char *)"O");
    Data::atom const*CA2 = Data::Pdb::getAtom(r2, (char *)"CA");

    if(CA1 == NULL || O1 == NULL || CA2 == NULL){
        std::cerr<<"Failed to get all the atoms for residue "<<r1.id<<std::endl;
        return newPP;
    }

    v3 ca1 = CA1->coor;
    v3 o1 = O1->coor;
    v3 ca2 = CA2->coor;

    v3 a = (ca2 - ca1).normalized();
    v3 b = (o1 - ca1).normalized();
    v3 c = v3::crossProduct(a, b).normalized();
    v3 d = v3::crossProduct(c, a).normalized();
    v3 p = (ca1 + ca2)/ 2.0f;
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

inline PeptidePlane NewPeptidePlane(const float *r1CA,const  float *r1O,const  float *r2CA,
   char ssr1, char ssr2, char ssr3, int idr1, int idr2, int idr3){
                                
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

    v3 ca1 = v3(r1CA[0], r1CA[1], r1CA[2]);
    v3 o1 = v3(r1O[0], r1O[1], r1O[2]);
    v3 ca2 = v3(r2CA[0], r2CA[1], r2CA[2]);

    v3 a = (ca2 - ca1).normalized();
    v3 b = (o1 - ca1).normalized();
    v3 c = v3::crossProduct(a, b).normalized();
    v3 d = v3::crossProduct(c, a).normalized();
    v3 p = (ca1 + ca2)/ 2.0f;

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


inline void Transition(const PeptidePlane &pp, char &type1, char &type2) 
{
    char t1 = pp.Residue1->ss;
    char t2 = pp.Residue2->ss;
    char t3 = pp.Residue3->ss;
    type1 = t2;
    type2 = t2;
    if (t2 > t1 && t2 == t3){
        type1 = t1;
    }
    if (t2 > t3 && t1 == t2){
        type2 = t3;
    }
}


inline void Flip(PeptidePlane &pp) 
{
    pp.Side = pp.Side * -1;
    pp.Normal = pp.Normal * -1;
    pp.Flipped = !pp.Flipped;
}


inline void splineForPlanes(v3 *&result, const PeptidePlane &p1, const PeptidePlane &p2, 
                    const PeptidePlane &p3,const PeptidePlane &p4, int n, float u, float v) {
    v3 g1 = p1.Position + p1.Side*u + p1.Normal * v;
    v3 g2 = p2.Position + p2.Side*u + p2.Normal * v;
    v3 g3 = p3.Position + p3.Side*u + p3.Normal * v;
    v3 g4 = p4.Position + p4.Side*u + p4.Normal * v;
    spline(result, g1, g2, g3, g4, n); 
}

} // end namespace cpdb
