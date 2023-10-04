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
#include "Data/MacroMolecule.h"

//#include "Util/octree/octree.h"
//#include "Util/octree/point3d.h"


namespace IQmol {

namespace Layer {

   // Component of a System that is large and will eventually support
   // boxing via an octree

   class MacroMolecule : public Component {

      Q_OBJECT

      public:
         explicit MacroMolecule(Data::MacroMolecule const& macroMolecule, QObject* parent = 0 );

         ~MacroMolecule() { }

         void setBoxingScale(double const scale);

         void reBox();

         double radius() { return m_radius; };

      Q_SIGNALS:
    
      private:
         double m_radius;
//         Octree< Point3D<double> >  m_octree;
   };

} } // end namespace IQmol::Layer 
