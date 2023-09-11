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

#include "ProteinChainLayer.h"
#include "Data/ProteinChain.h"
#include "Data/Surface.h"
#include "Data/GridSize.h"
#include "Grid/MeshDecimator.h"
#include "Cartoon.h"
#include "SurfaceLayer.h"
#include "Util/QsLog.h"


using namespace qglviewer;

namespace IQmol {
namespace Layer {


ProteinChain::ProteinChain(Data::ProteinChain& data, QObject* parent) :  
   MacroMolecule(data, parent), m_data(data)
{
   generateCartoon();   
}


void ProteinChain::cartoonAvailable()
{
   GenerateCartoon* generateCartoon(qobject_cast<GenerateCartoon*>(sender()));
   if (!generateCartoon) {
      QLOG_ERROR() << "Failed to cast GenerateCartoon task in ProteinChain";
      return;
   }

   Data::Surface* data(generateCartoon->getSurface());
   Layer::Surface* surfaceLayer(new Surface(*data));
   surfaceLayer->setCheckState(Qt::Checked);
   surfaceLayer->setText("Ribbon");
   prependLayer(surfaceLayer);
   updated();

   generateCartoon->deleteLater();;
}


void ProteinChain::generateCartoon()
{
   GenerateCartoon* generateCartoon = new GenerateCartoon(m_data);
   connect(generateCartoon, SIGNAL(finished()), this, SLOT(cartoonAvailable()));
   generateCartoon->start();
}


//------------------------------------------------------------------------------


void GenerateCartoon::run()
{
   Data::Mesh* mesh = fromCpdb(computeCartoonMesh());
   m_surface = new Data::Surface(*mesh);
}


cpdb::Mesh GenerateCartoon::computeCartoonMesh()
{
   double const* CA_OPositions(m_data.cao());
   int  const* ssTypePerRes(m_data.secondaryStructure());

   return cpdb::createChainMesh(m_data.nResidues(), CA_OPositions, ssTypePerRes);
}


Data::Mesh* GenerateCartoon::fromCpdb(cpdb::Mesh const& cmesh)
{
   Data::Mesh* mesh(new Data::Mesh);
   std::vector<Math::Vec3> verts  = cmesh.vertices;
   std::vector<Math::Vec3> colors = cmesh.colors;
   std::vector<Data::Mesh::Vertex> vertices;

   for (int j = 0; j < verts.size(); j++) {
       Data::Mesh::Vertex v(mesh->addVertex(verts[j][0], verts[j][1], verts[j][2]));
       vertices.push_back(v);
       //colors[j][0], colors[j][1], colors[j][2]);
   }       

   qDebug() << "Number of vertices" << vertices.size();

   std::vector<int> tri = cmesh.triangles;
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

   // Mesh decimation takes too long and the effects are negligible

   return mesh;
}

} } // end namespace IQmol::Layer
