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

#include "MacroMoleculeLayer.h"
#include "Data/CMesh.h"
#include "Data/Mesh.h"
#include "Data/MacroMolecule.h"

#include "Util/Task.h"


namespace IQmol {

namespace Data {
   class Mesh;
   class Surface;
   class ProteinChain;
}


namespace Layer {

   class ProteinChain : public MacroMolecule {

      Q_OBJECT

      public:
         explicit ProteinChain(Data::ProteinChain& data, QObject* parent = 0);

      public Q_SLOTS:
         void cartoonAvailable();

      private:
         void generateCartoon();
         Data::ProteinChain& m_data;

   };


   class GenerateCartoon : public Task {

      Q_OBJECT

      public:
         GenerateCartoon(Data::ProteinChain const& data) : 
           m_data(data), m_surface(0) { }

         Data::Surface* getSurface() { return m_surface; } 

      protected:
         void run();

      private:
         Data::Mesh* fromCpdb(cpdb::Mesh const& meshes);

         Data::ProteinChain const& m_data;
         Data::Surface* m_surface;
   };

} } // end namespace IQmol::Layer 
