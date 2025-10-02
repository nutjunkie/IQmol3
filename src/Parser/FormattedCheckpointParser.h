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

#include "Parser.h"

#include "Data/Shell.h"
#include "Data/Density.h"
#include "Data/Geometry.h"
#include "Data/Orbitals.h"
#include "Data/ExcitedStates.h"


namespace IQmol {

namespace Data {
   class GeminalOrbitals;
}

namespace Parser {

   class FormattedCheckpoint : public Base {

      public:
         bool parse(TextStream&);

      private:
         QList<int> readIntegerArray(TextStream&, unsigned nTokens);
         QList<double> readDoubleArray(TextStream&, unsigned nTokens);
         QList<unsigned> readUnsignedArray(TextStream&, unsigned nTokens);
         bool toInt(unsigned& n, QStringList const&, unsigned const index);
         bool toDouble(double& x, QStringList const&, unsigned const index);

         struct GeomData {
               QList<unsigned> atomicNumbers;
               QList<double> coordinates;
               int charge;
               unsigned multiplicity;
         };

         Data::Geometry* makeGeometry(GeomData const&);

         bool dataAreConsistent(Data::ShellData const&, unsigned const nAtoms);

         struct OrbitalData {
            Data::Orbitals::OrbitalType orbitalType;
            int stateIndex;
            QString label;

            QStringList   labels;
            QList<double> alphaCoefficients;
            QList<double> betaCoefficients;
            QList<double> alphaEnergies;
            QList<double> betaEnergies;
         };

         struct ComplexOrbitalData {
            Data::Orbitals::OrbitalType orbitalType;
            int stateIndex;
            QString label;

            QList<double> alphaRealCoefficients;
            QList<double> betaRealCoefficients;
            QList<double> alphaImaginaryCoefficients;
            QList<double> betaImaginaryCoefficients;

            QList<double> alphaEnergies;
            QList<double> betaEnergies;
         };

         void clear(OrbitalData&);
         Data::Orbitals* makeOrbitals(unsigned const nAlpha, unsigned const nBeta,
            OrbitalData const&, Data::ShellData const&, Data::Geometry const&,
            Data::DensityList densities = Data::DensityList()); 

         Data::Orbitals* makeComplexOrbitals(unsigned const nAlpha, unsigned const nBeta,
            ComplexOrbitalData const&, Data::ShellData const&, Data::Geometry const&,
            Data::DensityList densities = Data::DensityList());

         struct GmoData {
            QList<double> alphaCoefficients;
            QList<double> betaCoefficients;
            QList<double> geminalEnergies;
            QList<double> geminalCoefficients;
            QList<int>    geminalMoMap;
         };

         void clear(GmoData&);
         Data::GeminalOrbitals* makeGeminalOrbitals(unsigned const nAlpha, 
            unsigned const nBeta, GmoData const&, Data::ShellData const&, Data::Geometry const&);
            
         struct ExtData {
           unsigned nState;
           Data::ExcitedStates::ExcitedStatesT extType;
           QList<double> excitationEnergies;
           QList<double> oscillatorStrengths;
           QList<double> alphaAmplitudes;
           QList<double> alphaYAmplitudes;
           QList<double> betaAmplitudes;
           QList<double> betaYAmplitudes;
           QList<int>    alphaSparseJ;
           QList<int>    alphaSparseI;
           QList<int>    betaSparseJ;
           QList<int>    betaSparseI;
         };

         void clear(ExtData&);
         bool installExcitedStates(unsigned const nAlpha, unsigned const nBeta, 
            ExtData &extData, OrbitalData const&);
   };

} } // end namespace IQmol::Parser
