#include <vector>
#include <math.h>

#include "Cartoon.h"
#include "Math/Spline.h"

using namespace IQmol::Math;


namespace cpdb {

inline Vec3 lerp(Vec3 v0, Vec3 v1, float t) 
{
    return v0 * (1 - t) + v1 * t;
}


inline void Flip(PeptidePlane &pp) 
{
    pp.Side = pp.Side * -1;
    pp.Normal = pp.Normal * -1;
    pp.Flipped = !pp.Flipped;
}


inline void Transition(PeptidePlane const& pp, char& type1, char& type2) 
{
    char t1 = pp.Residue1->ss;
    char t2 = pp.Residue2->ss;
    char t3 = pp.Residue3->ss;
    type1 = t2;
    type2 = t2;

    if (t2 > t1 && t2 == t3) {
        type1 = t1;
    }
    if (t2 > t3 && t1 == t2) {
        type2 = t3;
    }
}





void ellipseProfile(std::vector<Vec3> &profile, int n, float w, float h) 
{
    for (int i = 0; i < n; i++) {
        float t = (float)i / (float)n;
        float a = t * 2.0f * M_PI + M_PI / 4.0f;
        float x = cosf(a) * w / 2.0f;
        float y = sinf(a) * h / 2.0f;
        profile[i][0] = x;
        profile[i][1] = y;
        profile[i][2] = 0.0f;
    }
}


void rectangleProfile(std::vector<Vec3> &profile, int n, float w, float h) {
    float hw = w / 2.0f;
    float hh = h / 2.0f;
    Vec3 segments[][2] =
    {
        {   Vec3{hw, hh, 0.0f},
            Vec3{-hw, hh, 0.0f}
        },

        {   Vec3{-hw, hh, 0.0f},
            Vec3{-hw, -hh, 0.0f}
        },

        {   Vec3{-hw, -hh, 0.0f},
            Vec3{hw, -hh, 0.0f}
        },

        {   Vec3{hw, -hh, 0.0f},
            Vec3{hw, hh, 0.0f}
        }
    };

    int m = n / 4;
    int cpt = 0;
    for (int s = 0; s < 4; s++) {
        for (int i = 0; i < m; i++) {
            float t = (float)i / (float)m;
            Vec3 p = lerp(segments[s][0], segments[s][1], t);
            profile[cpt++] = lerp(segments[s][0], segments[s][1], t);
        }
    }
}


void roundedRectangleProfile(std::vector<Vec3> &profile, int n, float w, float h) 
{
    float r = h / 2.0f;
    float hw = w / 2.0f - r;
    float hh = h / 2.0f;

    Vec3 segments[][2] =
    {
        {   Vec3{hw, hh, 0.0f},
            Vec3{-hw, hh, 0.0f}
        },

        {   Vec3{-hw, 0.0f, 0.0f},
            Vec3{0.0f, 0.0f, 0.0f}
        },

        {   Vec3{-hw, -hh, 0.0f},
            Vec3{hw, -hh, 0.0f}
        },

        {   Vec3{hw, 0.0f, 0.0f},
            Vec3{0.0f, 0.0f, 0.0f}
        }
    };

    int m = n / 4;
    int cpt = 0;

    for (int s = 0; s < 4; s++) {
        for (int i = 0; i < m; i++) {
            float t = (float)i / (float)m;
            Vec3 p;
            if (s == 0 || s == 2) {
                p = lerp(segments[s][0], segments[s][1], t);
            }
            else if (s == 1) {
                float a = M_PI / 2.0f + M_PI * t;
                float x = cosf(a) * r;
                float y = sinf(a) * r;
                p = segments[s][0] + Vec3{x, y, 0.0f};
            }
            else if (s == 3) {
                float a = 3 * M_PI / 2.0f + M_PI * t;
                float x = cosf(a) * r;
                float y = sinf(a) * r;
                p = segments[s][0] + Vec3{x, y, 0.0f};
            }
            profile[cpt++] = p;
        }
    }
}


void scaleProfile(std::vector<Vec3> &p, float s, int lenP) 
{
    for (int i = 0; i < lenP; i++) {
        p[i] = p[i] * s;
    }
}


void translateProfile(std::vector<Vec3> &p, float dx, float dy, int lenP) 
{
    for (int i = 0; i < lenP; i++) {
        p[i] = p[i] + Vec3{dx, dy, 0.0f};
    }
}


void segmentProfiles(const PeptidePlane &pp1, const PeptidePlane &pp2, 
   int n, std::vector<Vec3> &p1, std::vector<Vec3> &p2) 
{
    char type0 = pp1.Residue1->ss;
    char type1, type2;
    Transition(pp1, type1, type2);

    float offset1 = ribbonOffset;
    float offset2 = ribbonOffset;

    if (pp1.Flipped) {
        offset1 = -offset1;
    }
    if (pp2.Flipped) {
        offset2 = -offset2;
    }

    switch (type1) {
    case HELIX:
        if (type0 == STRAND) {
            roundedRectangleProfile(p1, n, 0.0f, 0.0f);
        }
        else {
            roundedRectangleProfile(p1, n, ribbonWidth, ribbonHeight);
        }
        translateProfile(p1, 0.0f, offset1, n);
        break;
    case STRAND:
        if (type2 == STRAND) {
            rectangleProfile(p1, n, arrowWidth, arrowHeight);
        } else {
            rectangleProfile(p1, n, arrowHeadWidth, arrowHeight);
        }
        break;
    default:
        if (type0 == STRAND) {
            ellipseProfile(p1, n, 0.0f, 0.0f);
        } else {
            ellipseProfile(p1, n, tubeSize, tubeSize);
        }
        break;
    }
    switch (type2) {
    case HELIX:
        roundedRectangleProfile(p2, n, ribbonWidth, ribbonHeight);
        translateProfile(p2, 0.0f, offset2, n);
        break;
    case STRAND:
        rectangleProfile(p2, n, arrowWidth, arrowHeight);
        break;
    default:
        ellipseProfile(p2, n, tubeSize, tubeSize);
        break;
    }
    if (type1 == STRAND && type2 != STRAND) {
        rectangleProfile(p2, n, 0.0f, arrowHeight);
    }
}


void segmentColors(const PeptidePlane &pp, Vec3 &c1, Vec3 &c2) 
{
    // const minTemp = 10
    // const maxTemp = 50
    // f1 := pp.Residue2.Atoms["CA"].TempFactor
    // f2 := pp.Residue3.Atoms["CA"].TempFactor
    // t1 := fauxgl.Clamp((f1-minTemp)/(maxTemp-minTemp), 0, 1)
    // t2 := fauxgl.Clamp((f2-minTemp)/(maxTemp-minTemp), 0, 1)
    // c1 = fauxgl.MakeColor(Viridis.Color(t1))
    // c2 = fauxgl.MakeColor(Viridis.Color(t2))
    // return
    char type1, type2;
    Transition(pp, type1, type2);
    switch (type1) {
    case HELIX:
        c1 = Vec3{1.0f, 0.71f, 0.2f};
        break;
    case STRAND:
        c1 = Vec3{0.96f, 0.45f, 0.21f};
        break;
    default:
        c1 = Vec3{0.02f, 0.47f, 0.47f};
        break;
    }
    switch (type2) {
    case HELIX:
        c2 = Vec3{1.0f, 0.71f, 0.2f};
        break;
    case STRAND:
        c2 = Vec3{0.96f, 0.45f, 0.21f};
        break;
    default:
        c2 = Vec3{0.02f, 0.47f, 0.47f};
        break;
    }
    if (type1 == STRAND) {
        c2 = c1;
    }
}


void triangulateQuad(Mesh &mesh,
   Vec3& p1, Vec3& p2, Vec3& p3, Vec3& p4, Vec3& c1, Vec3& c2, Vec3& c3, Vec3& c4) 
{
    int res1 = 0;
    int res2 = 0;
    int res3 = 0;
    int res4 = 0;

    int idp1 = res1;
    int idp2 = res2;
    int idp3 = res3;
    int idp4 = res4;

    const float tolerance = 1e-4f;

    p1[0] = floor(p1[0] / tolerance) * tolerance;
    p1[1] = floor(p1[1] / tolerance) * tolerance;
    p1[2] = floor(p1[2] / tolerance) * tolerance;

    p2[0] = floor(p2[0] / tolerance) * tolerance;
    p2[1] = floor(p2[1] / tolerance) * tolerance;
    p2[2] = floor(p2[2] / tolerance) * tolerance;

    p3[0] = floor(p3[0] / tolerance) * tolerance;
    p3[1] = floor(p3[1] / tolerance) * tolerance;
    p3[2] = floor(p3[2] / tolerance) * tolerance;

    p4[0] = floor(p4[0] / tolerance) * tolerance;
    p4[1] = floor(p4[1] / tolerance) * tolerance;
    p4[2] = floor(p4[2] / tolerance) * tolerance;

    if (mesh.verticesDict.find(p1) != mesh.verticesDict.end()) {
        idp1 = mesh.verticesDict.at(p1);
    }
    else {
        mesh.vertices.push_back(p1);
        idp1 = mesh.vertices.size() - 1;
        mesh.colors.push_back(c1);
        mesh.verticesDict.insert(std::pair<Vec3, int>(p1, idp1));
    }

    if (mesh.verticesDict.find(p2) != mesh.verticesDict.end()) {
        idp2 = mesh.verticesDict.at(p2);
    }
    else {
        mesh.vertices.push_back(p2);
        idp2 = mesh.vertices.size() - 1;
        mesh.colors.push_back(c2);
        mesh.verticesDict.insert(std::pair<Vec3, int>(p2, idp2));
    }

    if (mesh.verticesDict.find(p3) != mesh.verticesDict.end()) {
        idp3 = mesh.verticesDict.at(p3);
    }
    else {
        mesh.vertices.push_back(p3);
        idp3 = mesh.vertices.size() - 1;
        mesh.colors.push_back(c3);
        mesh.verticesDict.insert(std::pair<Vec3, int>(p3, idp3));
    }

    if (mesh.verticesDict.find(p4) != mesh.verticesDict.end()) {
        idp4 = mesh.verticesDict.at(p4);
    }
    else {
        mesh.vertices.push_back(p4);
        idp4 = mesh.vertices.size() - 1;
        mesh.colors.push_back(c4);
        mesh.verticesDict.insert(std::pair<Vec3, int>(p4, idp4));
    }

    //Add 2 triangles
    mesh.triangles.push_back(idp1);
    mesh.triangles.push_back(idp2);
    mesh.triangles.push_back(idp3);

    mesh.triangles.push_back(idp1);
    mesh.triangles.push_back(idp3);
    mesh.triangles.push_back(idp4);
}


void createSegmentMesh(Mesh &mesh, int curi, int n, const PeptidePlane &pp1, 
    const PeptidePlane &pp2, const PeptidePlane &pp3, const PeptidePlane &pp4) 
{
    char type0 = pp2.Residue1->ss;
    char type1, type2;
    Transition(pp2, type1, type2);
    Vec3 c1, c2;
    segmentColors(pp2, c1, c2);

    std::vector<Vec3> profile1(n);
    std::vector<Vec3> profile2(n);

    int lenProf1 = profileDetail;
    int lenProf2 = profileDetail;
    segmentProfiles(pp2, pp3, profileDetail, profile1, profile2);

    auto easeFunc = &Linear;
    if (!(type1 == STRAND && type2 != STRAND)) {

        easeFunc = &InOutQuad;
    }
    if (type0 == STRAND && type1 != STRAND) {
        easeFunc = &OutCirc;
    }
    // if type1 != STRAND && type2 == STRAND {
    //  easeFunc = &ease.InOutSquare
    // }


    // if (curi == 0) {
    //     ellipseProfile(profile1, profileDetail, 0.0f, 0.0f);
    //     easeFunc = &OutCirc;
    // } else if (curi == n-1) {
    //     ellipseProfile(profile2, profileDetail, 0.0f, 0.0f);
    //     easeFunc = &InCirc;
    // }

    Vec3 **splines1 = new Vec3*[lenProf1];
    Vec3 **splines2 = new Vec3*[lenProf2];

    for (int i = 0; i < lenProf1; i++) {
        splines1[i] = new Vec3[n];
        splines2[i] = new Vec3[n];
    }

    for (int i = 0; i < lenProf1; i++) {
        Vec3 p1 = profile1[i];
        Vec3 p2 = profile2[i];
        splineForPlanes(splines1[i], pp1, pp2, pp3, pp4, splineSteps, p1[0], p1[1]);
        splineForPlanes(splines2[i], pp1, pp2, pp3, pp4, splineSteps, p2[0], p2[1]);
    }

    for (int i = 0; i < splineSteps; i++) {
        float t0 = easeFunc(((float)i) / splineSteps);
        float t1 = easeFunc(((float)i + 1) / splineSteps);
        if (i == 0 && type1 == STRAND && type2 != STRAND) {
            Vec3 p00 = splines1[0][i];
            Vec3 p10 = splines1[profileDetail / 4][i];
            Vec3 p11 = splines1[2 * profileDetail / 4][i];
            Vec3 p01 = splines1[3 * profileDetail / 4][i];
            triangulateQuad(mesh, p00, p01, p11, p10, c1, c1, c1, c1);
        }

        for (int j = 0; j < profileDetail ; j++) {
            // for(int j = 0; j < profileDetail && j < lenProf1 && j < lenProf2; j++){
            Vec3 p100 = splines1[j][i];
            Vec3 p101 = splines1[j][i + 1];
            Vec3 p110 = splines1[(j + 1) % profileDetail][i];
            Vec3 p111 = splines1[(j + 1) % profileDetail][i + 1];
            Vec3 p200 = splines2[j][i];
            Vec3 p201 = splines2[j][i + 1];
            Vec3 p210 = splines2[(j + 1) % profileDetail][i];
            Vec3 p211 = splines2[(j + 1) % profileDetail][i + 1];
            Vec3 p00 = lerp(p100, p200, t0);
            Vec3 p01 = lerp(p101, p201, t1);
            Vec3 p10 = lerp(p110, p210, t0);
            Vec3 p11 = lerp(p111, p211, t1);
            Vec3 c00 = lerp(c1, c2, t0);
            Vec3 c01 = lerp(c1, c2, t1);
            Vec3 c10 = lerp(c1, c2, t0);
            Vec3 c11 = lerp(c1, c2, t1);
            triangulateQuad(mesh, p10, p11, p01, p00, c10, c11, c01, c00);
        }
    }
    // mesh.triangles = triangles;
    // mesh.colors = colors;
    // mesh.vertices = vertices;
    for (int i = 0; i < lenProf1; i++) {
        delete(splines1[i]);
        delete(splines2[i]);
    }
    delete[] splines1;
    delete[] splines2;
}


bool diffPP(int id1, int id2) {
    int diff = 1;
    if (id1 < 0 && id2 > 0) {
        diff = 2;
    }
    if (id2 - id1 > diff) {
        return true;
    }
    return false;
}


bool discontinuity(PeptidePlane pp1, PeptidePlane pp2, PeptidePlane pp3, PeptidePlane pp4) 
{
    int diff = 1;
    if (diffPP(pp1.Residue1->id, pp1.Residue2->id) || diffPP(pp1.Residue2->id, pp1.Residue3->id)) {
        return true;
    }
    if (diffPP(pp2.Residue1->id, pp2.Residue2->id) || diffPP(pp2.Residue2->id, pp2.Residue3->id)) {
        return true;
    }
    if (diffPP(pp3.Residue1->id, pp3.Residue2->id) || diffPP(pp3.Residue2->id, pp3.Residue3->id)) {
        return true;
    }
    if (diffPP(pp4.Residue1->id, pp4.Residue2->id) || diffPP(pp4.Residue2->id, pp4.Residue3->id)) {
        return true;
    }

    return false;
}


Mesh createChainMesh(const Data::chain &C) 
{
    Mesh mesh;
    PeptidePlane *planes = new PeptidePlane[C.residues.size() + 1];
    Vec3 previous;
    int nbPlanes = 0;
    for (int i = -1; i < C.residues.size(); i++) {

        int id = i;
        int id1 = i + 1;
        int id2 = i + 2;

        if (i == -1) {
            id = 0;
            id1 = 0;
            id2 = i + 1;
        }
        else if (i == C.residues.size() - 2) {
            id = i;
            id1 = i + 1;
            id2 = i + 1;
        }
        else if (i == C.residues.size() - 1) {
            id = i - 1;
            id1 = i;
            id2 = i;
        }
        const Data::residue &r1 = C.residues[id];
        const Data::residue &r2 = C.residues[id1];
        const Data::residue &r3 = C.residues[id2];

        PeptidePlane plane = NewPeptidePlane(r1, r2, r3);
        if (plane.Residue1 == NULL) {
            // TODO: better handling missing required atoms
            // planes[nbPlanes++] = plane;
            continue;
        }
        //Make sure to start at the first CA position
        if (i <= 0) {

            Data::atom const* CA = Data::Pdb::getAtom(r1, (char *)"CA");
            plane.Position = CA->coor;
        }
        //Make sure to end at the last CA position
        if (i >= C.residues.size() - 2) {
            Data::atom const* CA = Data::Pdb::getAtom(r3, (char *)"CA");
            plane.Position = CA->coor;
        }

        if (plane.Residue1 != NULL) {
            // TODO: better handling missing required atoms
            planes[nbPlanes++] = plane;
        }
    }
    for (int i = 0; i < nbPlanes; i++) {
        PeptidePlane &p = planes[i];
        if (i > 0 && p.Side * previous < 0.0f ) {
            Flip(p);
        }
        previous = p.Side;
    }

    int n = nbPlanes - 3;

    int nbTri =  n * (splineSteps + 1) * profileDetail * 6 ;
    int nbVert = n * (splineSteps + 1) * profileDetail * 4 ;
    mesh.triangles.reserve(nbTri);
    mesh.colors.reserve(nbVert);
    mesh.vertices.reserve(nbVert);

    for (int i = 0; i < n; i++) {
        // TODO: handle ends better
        PeptidePlane &pp1 = planes[i];
        PeptidePlane &pp2 = planes[i + 1];
        PeptidePlane &pp3 = planes[i + 2];
        PeptidePlane &pp4 = planes[i + 3];

        if (discontinuity(pp1, pp2, pp3, pp4)) {
            continue;
        }

        createSegmentMesh(mesh, i, n, pp1, pp2, pp3, pp4);
    }
    delete[] planes;
    return mesh;
}


Mesh createChainMesh(int chainId, const int *nbResPerChain,
                     const float *CA_OPositions, const char *ssTypePerRes) 
{
    Mesh mesh;
    int chainSize = nbResPerChain[chainId];
    PeptidePlane *planes = new PeptidePlane[chainSize + 1];
    Vec3 previous;
    int nbPlanes = 0;
    int startRes = 0;
    for (int i = 0; i < chainId; i++){
        startRes += nbResPerChain[i];
    }
    const char *SSRes = &ssTypePerRes[startRes];
    const float *atomPos = &CA_OPositions[startRes * 6];

    for (int i = -1; i < chainSize; i++) {

        int id = i;
        int id1 = i + 1;
        int id2 = i + 2;

        if (i == -1) {
            id = 0;
            id1 = 0;
            id2 = i + 1;
        }
        else if (i == chainSize - 2) {
            id = i;
            id1 = i + 1;
            id2 = i + 1;
        }
        else if (i == chainSize - 1) {
            id = i - 1;
            id1 = i;
            id2 = i;
        }

        PeptidePlane plane = NewPeptidePlane(&atomPos[id * 6],
                                             &atomPos[id * 6 + 3],
                                             &atomPos[id1 * 6],
                                             SSRes[id], SSRes[id1], SSRes[id2],
                                             id, id1, id2);
        if (plane.Residue1 == NULL) {
            // TODO: better handling missing required atoms
            // planes[nbPlanes++] = plane;
            continue;
        }
        //Make sure to start at the first CA position
        if (i <= 0) {
            //CA of r1
            plane.Position = Vec3{atomPos[0], atomPos[1], atomPos[2]};
        }
        //Make sure to end at the last CA position
        if (i >= chainSize - 2) {
            //CA of r3
            plane.Position = Vec3{atomPos[id2 * 3 * 2 + 0], 
                                  atomPos[id2 * 3 * 2 + 1],
                                  atomPos[id2 * 3 * 2 + 2]};
        }

        if (plane.Residue1 != NULL) {
            // TODO: better handling missing required atoms
            planes[nbPlanes++] = plane;
        }
    }
    for (int i = 0; i < nbPlanes; i++) {
        PeptidePlane &p = planes[i];
        if (i > 0 && p.Side * previous < 0.0f ) {
            Flip(p);
        }
        previous = p.Side;
    }

    int n = nbPlanes - 3;

    int nbTri =  n * (splineSteps + 1) * profileDetail * 6 ;
    int nbVert = n * (splineSteps + 1) * profileDetail * 4 ;
    mesh.triangles.reserve(nbTri);
    mesh.colors.reserve(nbVert);
    mesh.vertices.reserve(nbVert);

    for (int i = 0; i < n; i++) {
        // TODO: handle ends better
        PeptidePlane &pp1 = planes[i];
        PeptidePlane &pp2 = planes[i + 1];
        PeptidePlane &pp3 = planes[i + 2];
        PeptidePlane &pp4 = planes[i + 3];

        if (discontinuity(pp1, pp2, pp3, pp4)) {
            continue;
        }

        createSegmentMesh(mesh, i, n, pp1, pp2, pp3, pp4);
    }
    delete[] planes;
    return mesh;
}

} // end namespace cpdb
