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

#include "Orbitals.h"


namespace IQmol {
namespace Data {

   class LocalizedOrbitals : public Orbitals {

      public:
         Type::ID typeID() const { return Type::LocalizedOrbitals; }

         LocalizedOrbitals() : Orbitals(), m_nAlpha(0), m_nBeta(0) { }

         LocalizedOrbitals(
            unsigned const nAlpha, 
            unsigned const nBeta, 
            ShellList const& shellList,
            QList<double> const& alphaCoefficients, 
            QList<double> const& betaCoefficients, 
            QString const& label) 
          : Orbitals(Orbitals::Localized, shellList, 
            alphaCoefficients, betaCoefficients, label), 
            m_nAlpha(nAlpha), m_nBeta(nBeta) { }
              
         unsigned nAlpha() const { return m_nAlpha; }
         unsigned nBeta()  const { return m_nBeta;  }

         QString label(unsigned index, bool alpha = true) const
         {
            unsigned n(alpha ? m_nAlpha : m_nBeta);
            QString  s(alpha ? "Alpha " : "Beta ");
            s += QString::number(index+1);
            if (index < n)  s += " (occ)";
            return s;
         }

         unsigned labelIndex(bool const alpha) const
         {
            int n(alpha ? m_nAlpha : m_nBeta);
            return std::max(0,n-1);
         }

      private:
         unsigned m_nAlpha;
         unsigned m_nBeta;
   };

} } // end namespace IQmol::Data
