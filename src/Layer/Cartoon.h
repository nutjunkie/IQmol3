#include <map>
#include "Math/Vector.h"
#include "Data/CMesh.h"
#include "Data/Residue.h"
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



Mesh createChainMesh(Data::ProteinChain const&);


struct Residue 
{
   int id, idx;
   Data::SecondaryStructure ss;
};
   

struct PeptidePlane 
{
   Residue Residue1;
   Residue Residue2;
   Residue Residue3;

   Vec3 Position;
   Vec3 Normal;
   Vec3 Forward;
   Vec3 Side;

   bool Flipped;
};


} // end namespace cpdb
