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

#include "Data.h"
#include "Spin.h"
#include <QList>


namespace IQmol {
namespace Data {

   class OrbitalSymmetries : public Base {

      public:
         OrbitalSymmetries() : m_nAlpha(0), m_nBeta(0) { }

         Type::ID typeID() const { return Type::OrbitalSymmetries; }

         void append(Spin const, double const energy, QString const& symmetry);
         void setOccupied(Spin const, unsigned const nOrb);

         unsigned nOrbitals() const { return m_alphaEnergies.size(); }
         unsigned nAlpha() const { return m_nAlpha; }
         unsigned nBeta()  const { return m_nBeta; }

         double energy(Spin const, unsigned const n) const;
         QString symmetry(Spin const, unsigned const n) const;

         void dump() const;

      private:
         unsigned m_nAlpha;
         unsigned m_nBeta;
         QList<double>  m_alphaEnergies;
         QList<double>  m_betaEnergies;
         QList<QString> m_alphaSymmetries;
         QList<QString> m_betaSymmetries;
   };

} } // end namespace IQmol::Data
