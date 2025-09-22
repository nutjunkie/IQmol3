#pragma once
/*******************************************************************************

  Copyright (C) 2022 Andrew Gilbert

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

#include "GridData.h"
#include "Geometry.h"


namespace IQmol {
namespace Data {

   class CubeData : public GridData {

      public:
         Type::ID typeID() const { return Type::CubeData; }

         CubeData(Geometry const& geometry, GridSize const& size, SurfaceType const& type, 
           QList<double> const& data) : GridData(size, type, data), m_geometry(geometry) { }

         CubeData() { }  // for boost::serialize;

         Geometry const& geometry() const { return m_geometry; }

         QString const& label() const { return m_label; }

         void setLabel(QString const& label) { m_label = label; }

      private:
         Geometry m_geometry;
         QString  m_label;
   };

} } // end namespace IQmol::Data
