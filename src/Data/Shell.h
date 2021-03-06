#ifndef IQMOL_DATA_SHELL_H
#define IQMOL_DATA_SHELL_H
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


namespace IQmol {
namespace Data {

   /// Basic Shell class representing a contracted shell of a particular
   /// angular momentum.  Note that we assume all the input values are in
   /// angstrom, not bohr, and that the coefficients are given for the
   /// normalized basis functions.
   class Shell : public Base {

      friend class boost::serialization::access;

      public:
         enum AngularMomentum { S, P, D5, D6, F7, F10, G9, G15 };
         static QString toString(AngularMomentum const);

         Type::ID typeID() const { return Type::Shell; }

         Shell(AngularMomentum const angularMomentum = S, 
            unsigned const atomIndex = 0,
            qglviewer::Vec const& position = qglviewer::Vec(), 
            QList<double> const& exponents = QList<double>(), 
            QList<double> const& contractionCoefficients = QList<double>());

		 /// Returns the (-1,-1,-1) and (1,1,1) octant corners of a rectangular
		 /// box that encloses the significant region of the Shell where 
		 /// significance is determined by thresh.  Note that for surfaces this 
         /// value is rather large to ensure the bounding box as small as possible. 
         /// The value should be consistent with the minimum isosurface value
         /// in the SurfaceConfigurator class.  Note that a large value may 
         /// not be appropriate for other wavefunction analyses.
         void boundingBox(qglviewer::Vec& min, qglviewer::Vec& max, 
            double const thresh = 0.001);

		 // Returns a pointer to an array containing the values of the basis
		 // functions at the given position.
         double const* evaluate(qglviewer::Vec const& gridPoint) const;
         double const* evaluate(double const x, double const y, double const z) const;

         AngularMomentum angularMomentum() const { return m_angularMomentum; }

         unsigned atomIndex() const { return m_atomIndex; }

         unsigned nBasis() const;

         QString label(unsigned const) const;

         void serialize(InputArchive& ar, unsigned int const version = 0) {
            privateSerialize(ar, version);
         }  
         
         void serialize(OutputArchive& ar, unsigned int const version = 0) {
            privateSerialize(ar, version); 
         }  


         void dump() const;

      private:
		 /// Shell values are stored in this static array, the length of which
		 /// is sufficient for up to g angular momentum.  This could cause 
         /// problems if Shells are ever used in parallel.
         static double s_values[15];
         static double s_zeroValues[15];

		 /// Computes and saves the significant radius of the shell,
		 /// as determined by thresh.  Note that this is set to
		 /// numeric_limits<double>::max until the bounding box is 
         /// requested with a threshold which is passed on to this.
         double computeSignificantRadius(double const thresh);
         void normalize();

         template <class Archive>
         void privateSerialize(Archive& ar, unsigned const) {
            ar & m_angularMomentum;
            ar & m_atomIndex;
            ar & m_position;
            ar & m_exponents;
            ar & m_contractionCoefficients;
            ar & m_significantRadiusSquared;
         }

         AngularMomentum m_angularMomentum;
         unsigned        m_atomIndex;
         qglviewer::Vec  m_position;
         QList<double>   m_exponents;
         QList<double>   m_contractionCoefficients;
         double          m_significantRadiusSquared;
   };


} } // end namespace IQmol::Data

#endif
