#pragma once
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

#include "ComponentLayer.h"
#include "Data/CMesh.h"
#include "Data/Mesh.h"
#include "Data/MacroMolecule.h"


namespace IQmol {

namespace Data {
   class Mesh;
   class Pdb;
}

namespace Layer {

   class Protein : public Component {

      Q_OBJECT

      //friend class Configurator::Surface;

      public:
         explicit Protein(Data::Pdb &, QObject* parent = 0 );

         double radius() { return m_radius; }

         ~Protein() { }

      Q_SIGNALS:
    
      private:

         //Data::Surface* generateSurface();
         void generateCartoons();

         std::vector<cpdb::Mesh> computeCartoonMesh(Data::Pdb& pdb);
         Data::MeshList fromCpdb(std::vector<cpdb::Mesh> const& meshes);

         Data::Pdb& m_pdbData;
         double m_radius;

   };

} } // end namespace IQmol::Layer 
