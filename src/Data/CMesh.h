#pragma once

#include "Math/v3.h"

using namespace IQmol;

namespace cpdb {

struct cmpFloat3 {
    bool operator()(const v3 &a, const v3 &b) const {
        if      (a.x <= b.x && a.y <= b.y && a.z < b.z) return true;
        else if (a.x <= b.x && a.y < b.y) return true;
        else if (a.x < b.x) return true;
        else return false;
    }   
};

struct Mesh
{
    std::vector<v3> vertices;
    std::vector<v3> colors;
    std::vector<int> triangles;
    std::map<v3, int, cmpFloat3> verticesDict;
};

}
