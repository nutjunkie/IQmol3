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

#include "OrbitalsLayer.h"
#include "Data/ComplexOrbitals.h"

class QProgressDialog;

namespace IQmol {

class GridProduct;

namespace Configurator {
   class Orbitals;
}

namespace Layer {

   class ComplexOrbitals : public Orbitals {

      Q_OBJECT

      friend class Configurator::Orbitals;

      public:
         ComplexOrbitals(Data::ComplexOrbitals&);
         ~ComplexOrbitals() { }

         void computeFirstOrderDensityMatrix();
         
      protected:
         double alphaOrbitalEnergy(unsigned const i) const;
         double betaOrbitalEnergy(unsigned const i) const;

         QString description(Data::SurfaceInfo const&, bool const tooltip);

      private Q_SLOTS:
         void firstOrderDensityMatrixFinished();

      private:
         void computeDensityVectors();
         Data::ComplexOrbitals& m_complexOrbitals;  // true cast of Orbitals::m_orbitals

         // Temporary stuff for the grid product
         QProgressDialog* m_progressDialog;
         GridProduct* m_gridProduct;
         Vector m_values;
   };

} } // End namespace IQmol::Layer 
