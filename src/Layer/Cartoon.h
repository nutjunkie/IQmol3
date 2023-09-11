#include <map>
//#include <iostream>
#include "Math/Vec.h"
#include "Math/Spline.h"
#include "Data/CMesh.h"
#include "Data/AminoAcid.h"
#include "Data/ProteinChain.h"


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

//['#fbb4ae','#b3cde3','#ccebc5','#decbe4','#fed9a6','#ffffcc','#e5d8bd']


Mesh createChainMesh(Data::ProteinChain const&);


struct Residue 
{
   int id, idx, ss;
};
   

struct PeptidePlane 
{
   Residue Residue1;
   Residue Residue2;
   Residue Residue3;

   Math::Vec3 Position;
   Math::Vec3 Normal;
   Math::Vec3 Forward;
   Math::Vec3 Side;

   bool Flipped;
};





} // end namespace cpdb
