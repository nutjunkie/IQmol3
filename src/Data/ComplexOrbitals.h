#pragma once
/*******************************************************************************

  Copyright (C) 2025 Andrew Gilbert

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
#include "Density.h"


namespace IQmol {
namespace Data {

   /// Data class for molecular orbital information
   class ComplexOrbitals : public Orbitals {

      public:
         Type::ID typeID() const override { return Type::ComplexOrbitals; };

         ComplexOrbitals(unsigned const nAlpha, unsigned const nBeta, ShellList const& shells, 
            QList<double> const& alphaRealCoefficients, 
            QList<double> const& alphaImaginaryCoefficients, 
            QList<double> const& alphaEnergies, 
            QList<double> const& betaRealCoefficients, 
            QList<double> const& betaImaginaryCoefficients, 
            QList<double> const& betaEnergies, 
            QString const& label);

         Matrix const& alphaRealCoefficients() const { return alphaCoefficients(); }
         Matrix const& betaRealCoefficients()  const { return betaCoefficients(); }

         Matrix const& alphaImaginaryCoefficients() const { return m_alphaImaginaryCoefficients; }
         Matrix const& betaImaginaryCoefficients()  const { return m_betaImaginaryCoefficients; }

         DensityList const& densityList() const { return m_densityList; }
         void appendDensities(Data::DensityList const& densities) {
            m_densityList << densities;
         }

         unsigned nAlpha() const override { return m_nAlpha; }
         unsigned nBeta()  const override { return m_nBeta;  }

         double alphaOrbitalEnergy(unsigned i) const;
         double betaOrbitalEnergy(unsigned i) const;

         double alphaOccupancy(unsigned i) const { return (i < m_nAlpha ? 1.0 : 0.0); }
         double betaOccupancy(unsigned i)  const { return (i < m_nBeta  ? 1.0 : 0.0); }

         QString label(unsigned index, bool alpha = true) const override;
         bool consistent() const override;

         void dump() const override;

      private:
         unsigned m_nAlpha;
         unsigned m_nBeta;

         QList<double> m_alphaEnergies;
         QList<double> m_betaEnergies;

         Matrix m_alphaImaginaryCoefficients;
         Matrix m_betaImaginaryCoefficients;
         DensityList   m_densityList;
   };

} } // end namespace IQmol::Data
