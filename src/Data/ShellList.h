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

#include "QGLViewer/vec.h"
#include "Data/DataList.h"
#include "Data/Shell.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"


namespace IQmol {
namespace Data {

    class Geometry;

   // This is a POD structure to accumulate the data in the parsers (taken from
   // FormattedCheckpointParser).
   struct ShellData {
      QList<int>      shellTypes;
      QList<unsigned> shellToAtom;
      QList<unsigned> shellPrimitives;
      QList<double>   exponents;
      QList<double>   contractionCoefficients;
      QList<double>   contractionCoefficientsSP;
      QList<double>   overlapMatrix;
      unsigned        nBasis;   // recomputed once the ShellList has been formed
   };

   class ShellList : public List<Shell> {

      public:
         ShellList() { }

         ShellList(ShellData const& shellData, Geometry const& geometry);

         ~ShellList();

         Type::ID typeID() const { return Type::ShellList; }

         /// Returns the (-1,-1,-1) and (1,1,1) octant corners of a rectangular
         /// box that encloses the significant region of the Shells where 
         /// significance is determined by thresh.  
         void boundingBox(qglviewer::Vec& min, qglviewer::Vec& max, 
            double const thresh = 0.001);

         unsigned nBasis() const;

         Vector const& overlapMatrix() const { return m_overlapMatrix; }

         void setOverlapMatrix(QList<double> const& overlapMatrix) {
            unsigned nElements(overlapMatrix.size());
            m_overlapMatrix.resize({nElements});
            for (unsigned i = 0; i < nElements; ++i) {
                m_overlapMatrix(i) = overlapMatrix[i];
            }
         }

         /// Allocates the memory for evaluating the shells/shell pairs on a grid
         /// point.  This should be called after the last Shell has been appended
         /// to the list and before shellValues is called.
         void resize();

         Vector const& shellValues(double const x, double const y, double const z);

		 // Initializes the list of densities to be evaluated a grid points
		 // with subsequent densityValues calls.
         void setDensityVectors(QList<Vector const*> const& densities);

         // Returns a list of the densities evaulated at the given grid point
         // Density vectors are upper triangular
         Vector const& densityValues(double const x, double const y, double const z);

		 // Initializes the list of orbitals to be evaluated a grid points
		 // with subsequent orbitalValues calls.
         void setOrbitalVectors(Matrix const& coefficients, QList<int> const& indices);

         // Returns a list of the orbitals evaulated at the given grid point
         Vector const& orbitalValues(double const x, double const y, double const z);

         // Shell offset for each atom
         QList<unsigned> shellAtomOffsets() const;

         // Basis offset for each atom
         QList<unsigned> basisAtomOffsets() const;

         // Kludge to account for the ordering Q-Chem prints the shell functions,
         // for Dyson orbitals printed to the output file.
         void reorderFromQChem(Matrix& C);

         void dump() const;

      private:
         unsigned m_nBasis;
         Vector   m_overlapMatrix;   // upper triangular

         // These arrays are workspace buffers for gridpoint evaluations;
         unsigned* m_sigBasis;
         Vector    m_basisValues;
         Vector    m_densityValues;
         Vector    m_orbitalValues;

         Matrix const*        m_orbitalCoefficients;
         QList<int>           m_orbitalIndices;
         QList<Vector const*> m_densityVectors;

         Vector    m_basisPairValues;  // Deprecate
   };

} } // end namespace IQmol::Data
