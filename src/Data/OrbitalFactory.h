#ifndef IQMOL_DATA_ORBITAL_FACTORY_H
#define IQMOL_DATA_ORBITAL_FACTORY_H
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

#include "Data/Orbitals.h"
#include "Data/Density.h"

namespace IQmol {
namespace Data {

struct OrbitalData {
   Orbitals::OrbitalType orbitalType;
   int stateIndex;
   QString label;

   QStringList   labels;
   QList<double> alphaCoefficients;
   QList<double> betaCoefficients;
   QList<double> alphaEnergies;
   QList<double> betaEnergies;
};


Orbitals* OrbitalFactory(unsigned const nAlpha, unsigned const nBeta, 
   OrbitalData const& orbitalData, ShellData const& shellData, 
   Geometry const& geometry, DensityList densityList);

} } // end namespace IQmol::Data

#endif
