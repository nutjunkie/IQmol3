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
#include "Data.h"
#include <vector>


namespace IQmol {
namespace Data {

   // Shell class representing a contracted shell of a particular angular
   // momentum.  Note that all the input values are assumed to be in angstrom,
   // and are stored in angstrom as this is the native unit of the Viewer.
   // Note also the coefficients are given for the normalized basis functions.
   // Basis functions are stored in Molden format 
   //    https://www.theochem.ru.nl/molden/molden_format.html

   class Shell : public Base {

      public:
         enum AngularMomentum { 
              H11 = -5,
              G9  = -4,
              F7  = -3,
              D5  = -2,
              SP  = -1,
              S   =  0,
              P   =  1,
              D6  =  2,
              F10 =  3,
              G15 =  4,
              H21 =  5 
         };

         static QString toString(AngularMomentum const);
         static unsigned nFunctions(AngularMomentum const);
         static QString label(AngularMomentum const, unsigned const);

         Type::ID typeID() const { return Type::Shell; }

         Shell(AngularMomentum const angularMomentum = S, 
            unsigned const atomIndex = 0,
            qglviewer::Vec const& position = qglviewer::Vec(), 
            QList<double> const& exponents = QList<double>(), 
            QList<double> const& contractionCoefficients = QList<double>());

         AngularMomentum angularMomentum() const { return m_angularMomentum; }

         QString label(unsigned i) const { return label(m_angularMomentum, i); }

         unsigned nBasis() const { return nFunctions(m_angularMomentum); }

		 /// Returns the (-1,-1,-1) and (1,1,1) octant corners of a rectangular
		 /// box that encloses the significant region of the Shell where 
		 /// significance is determined by thresh.  Note that for surfaces this 
         /// value is rather large to ensure the bounding box as small as possible. 
         /// The value should be consistent with the minimum isosurface value
         /// in the SurfaceConfigurator class.  Note that a large value may 
         /// not be appropriate for other wavefunction analyses.
         void boundingBox(qglviewer::Vec& min, qglviewer::Vec& max, 
            double const thresh = 0.001);

		 // Returns a reference to an array containing the values of the basis
		 // functions evaluated at the given position.  Returns a null pointer
         // if the position is beyond the significant radius.
         double const* evaluate(double const x, double const y, double const z);

         unsigned atomIndex() const { return m_atomIndex; }

         void dump() const;


      private:
		 /// Shell values are stored in this static array, the length of which
		 /// is sufficient for up to h angular momentum.  This could cause 
         /// problems if Shells are ever used in parallel.
         std::vector<double> m_values;

		 /// Computes and saves the significant radius of the shell,
		 /// as determined by thresh.  Note that this is set to
		 /// numeric_limits<double>::max until the bounding box is 
         /// requested with a threshold which is passed on to this.
         double computeSignificantRadius(double const thresh);

         void normalizeToAngstrom();

         AngularMomentum m_angularMomentum;
         unsigned        m_nFunctions;
         unsigned        m_atomIndex;
         qglviewer::Vec  m_position;
         QList<double>   m_exponents;
         QList<double>   m_contractionCoefficients;
         double          m_significantRadiusSquared;
   };


} } // end namespace IQmol::Data
