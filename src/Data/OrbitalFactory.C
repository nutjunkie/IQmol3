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

#include "Data/OrbitalFactory.h"
#include "Data/CanonicalOrbitals.h"
#include "Data/LocalizedOrbitals.h"
#include "Data/NaturalTransitionOrbitals.h"
#include "Data/NaturalBondOrbitals.h"
#include "Data/DysonOrbitals.h"
#include "Util/QsLog.h"


namespace IQmol {
namespace Data {

Orbitals* OrbitalFactory(unsigned const nAlpha, unsigned const nBeta, 
   OrbitalData const& orbitalData, ShellData const& shellData, 
   Geometry const& geometry, DensityList densityList)
{
   if (orbitalData.alphaCoefficients.isEmpty()) return 0;
   Data::ShellList* shellList = new Data::ShellList(shellData, geometry);
   if (!shellList) return 0;

   Data::Orbitals* orbitals(0);
   QString surfaceTag;

   switch (orbitalData.orbitalType) {

      case Data::Orbitals::Canonical: {
         Data::CanonicalOrbitals* canonical = 
            new Data::CanonicalOrbitals(nAlpha, nBeta, *shellList,
                orbitalData.alphaCoefficients, orbitalData.alphaEnergies, 
                orbitalData.betaCoefficients,  orbitalData.betaEnergies, orbitalData.label);
         canonical->appendDensities(densityList); 
         orbitals = canonical;
      } break;

      case Data::Orbitals::Localized: {
         orbitals = new Data::LocalizedOrbitals(nAlpha, nBeta, *shellList, 
            orbitalData.alphaCoefficients, orbitalData.betaCoefficients, 
            orbitalData.label);
      } break;

      case Data::Orbitals::NaturalTransition: {
         orbitals = new Data::NaturalTransitionOrbitals(*shellList,
            orbitalData.alphaCoefficients, orbitalData.alphaEnergies, 
            orbitalData.betaCoefficients,  orbitalData.betaEnergies, orbitalData.label);
      } break;

      case Data::Orbitals::Dyson: {
         orbitals = new Data::DysonOrbitals(*shellList, orbitalData.alphaCoefficients, 
            orbitalData.betaCoefficients, orbitalData.alphaEnergies, orbitalData.labels);
      } break;

      case Data::Orbitals::NaturalBond: {
         orbitals = new Data::NaturalBondOrbitals(nAlpha, nBeta, *shellList,
            orbitalData.alphaCoefficients, orbitalData.alphaEnergies, 
            orbitalData.betaCoefficients,  orbitalData.betaEnergies, orbitalData.label);
      }  break;

      case Data::Orbitals::Generic: {
         orbitals = new Data::Orbitals(Data::Orbitals::Generic, *shellList, 
            orbitalData.alphaCoefficients, orbitalData.betaCoefficients, orbitalData.label);
            
      } break;

      default:
         QLOG_WARN() << "Unknown orbital type in FormattedCheckpoint::makeOrbitals";
         break;
   }

   if (orbitals && !orbitals->consistent()) {
      QString msg(Data::Orbitals::toString(orbitalData.orbitalType));
      msg += " data are inconsistent. Check shell types.";
      QLOG_WARN() << msg;
      delete orbitals;
      orbitals = 0;
   }

   return orbitals;
}

} } // end namespace IQmol::Data
