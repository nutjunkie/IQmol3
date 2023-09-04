/*******************************************************************************
         
  Copyright (C) 2023 Andrew Gilbert
      
  This file is part of IQmol, a free molecular visualization program. See
  <http://iqmol.org> for more details.
         
  IQmol is free software: you can redistribute it and/or modify it under the
  terms of the GNU General Public License as published by the Free Software  
  Foundation, either version 3 of the License, or (at your option) any later  
  version.

  IQmol is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.
      
  You should have received a copy of the GNU General Public License along
  with IQmol.  If not, see <http://www.gnu.org/licenses/>.
   
********************************************************************************/

#include "ProteinLayer.h"
#include "Data/PdbData.h"
#include "Data/Surface.h"
#include "Data/GridSize.h"
#include "Grid/MeshDecimator.h"
#include "Cartoon.h"
#include "SurfaceLayer.h"


using namespace qglviewer;

namespace IQmol {
namespace Layer {


Protein::Protein(Data::Pdb& pdbData, QObject* parent) :  
    Component("Protein", parent), m_radius(0), m_pdbData(pdbData)
{
   generateCartoons();   
}

std::vector<cpdb::Mesh> Protein::computeCartoonMesh(Data::Pdb& pdb)
{
    int nbChain(pdb.nChains());
    int* nbResPerChain(pdb.nres());
    double* CA_OPositions(pdb.cao());
    char* ssTypePerRes(pdb.ss());

    std::vector<cpdb::Mesh> meshes(nbChain);
    if (nbChain <= 0) {
        return meshes;
    }   

    for (int chainId = 0; chainId < nbChain; chainId++) {
        cpdb::Mesh m = cpdb::createChainMesh(chainId, nbResPerChain, CA_OPositions, ssTypePerRes);
        meshes[chainId] = m;
    }

    return meshes;
}


Data::MeshList Protein::fromCpdb(std::vector<cpdb::Mesh> const& meshes)
{
   Data::MeshList meshList;

   for (int i = 0; i < meshes.size(); i++) {
       Data::Mesh* mesh(new Data::Mesh);
       std::vector<Math::Vec3> verts  = meshes[i].vertices;
       std::vector<Math::Vec3> colors = meshes[i].colors;
       std::vector<Data::Mesh::Vertex> vertices;

       for (int j = 0; j < verts.size(); j++) {
           Data::Mesh::Vertex v(mesh->addVertex(verts[j][0], verts[j][1], verts[j][2]));
           vertices.push_back(v);
           //colors[j][0], colors[j][1], colors[j][2]);
       }       

       qDebug() << "Number of vertices" << vertices.size();

       std::vector<int> tri = meshes[i].triangles;
       for (unsigned j = 0; j < tri.size(); j += 3) {
           unsigned j0(tri[j  ]); 
           unsigned j1(tri[j+1]); 
           unsigned j2(tri[j+2]); 

           // Avoid adding degenerate faces
           if (j0 != j1 && j0 != j2 && j1 != j2) {
              Data::Mesh::Vertex v0(vertices[j0]);
              Data::Mesh::Vertex v1(vertices[j1]);
              Data::Mesh::Vertex v2(vertices[j2]);
              mesh->addFace(v0, v1, v2);
           }      
       }          
       mesh->computeFaceNormals();
       mesh->computeVertexNormals();

       double delta(Data::GridSize::stepSize(2));
       MeshDecimator decimator(*mesh);
       if (!decimator.decimate(delta)) {
          qDebug() <<"Mesh decimation failed: " + decimator.error();
       }   
       meshList.append(mesh);
   }       

   return meshList;
}


void Protein::generateCartoons()
{
   std::vector<cpdb::Mesh> meshes = computeCartoonMesh(m_pdbData);
   Data::MeshList list(fromCpdb(meshes));
   for (auto mesh : list) {
       Data::Surface* surface(new Data::Surface(*mesh));
       Layer::Surface* surfaceLayer(new Surface(*surface));
       surfaceLayer->setCheckState(Qt::Checked);
       appendLayer(surfaceLayer);
   }
}

} } // end namespace IQmol::Layer
