#pragma once

#include <map>
#include "Math/Vec.h"

using namespace IQmol;

namespace cpdb {

struct cmpFloat3 
{
    bool operator()(const Math::Vec3 &a, const Math::Vec3 &b) const 
    {
        if      (a[0] <= b[0] && a[1] <= b[1] && a[2] < b[2]) return true;
        else if (a[0] <= b[0] && a[1] < b[1]) return true;
        else if (a[0] < b[0]) return true;
        else return false;
    }   
};


struct Mesh
{
    std::vector<Math::Vec3> vertices;
    std::vector<Math::Vec3> colors;
    std::vector<int> triangles;
    std::map<Math::Vec3, int, cmpFloat3> verticesDict;
};

}
