#pragma once

#include <map>
#include "Math/Vector.h"

using namespace IQmol;

namespace cpdb {

struct cmpFloat3 
{
    bool operator()(const Vec3 &a, const Vec3 &b) const 
    {
        if      (a[0] <= b[0] && a[1] <= b[1] && a[2] < b[2]) return true;
        else if (a[0] <= b[0] && a[1] < b[1]) return true;
        else if (a[0] < b[0]) return true;
        else return false;
    }   
};


struct Mesh
{
    std::vector<Vec3> vertices;
    std::vector<Vec3> colors;
    std::vector<int> vertexResidues;
    std::vector<int> triangles;
    std::map<Vec3, int, cmpFloat3> verticesDict;
};

}
