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

#include "DataList.h"
#include "SurfaceType.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"


namespace IQmol {
namespace Data {

   /// Data class for density matrices.  Note these are read in as upper
   /// triangular and stored in a vectorized form.
   class Density : public Base {

      public:
         Type::ID typeID() const { return Type::Density; }

         Density() { }

         Density(SurfaceType const& surfaceType, QList<double> const& vectorElements,
            QString const& label = QString(), bool square = false);

         Density(SurfaceType const& surfaceType, Matrix const& matrix,
            QString const& label = QString());


         SurfaceType const& surfaceType() const { return m_surfaceType; }

         QString const& label() const { return m_label; }

         Vector* vector() { return &m_elements; }

         void dump() const;

      private:
         SurfaceType m_surfaceType;
         QString     m_label;
         unsigned    m_nBasis;
         Vector      m_elements;
   };


   class DensityList : public List<Density> { 
      public:
         Type::ID typeID() const { return Type::DensityList; }
   };

} } // end namespace IQmol::Data
