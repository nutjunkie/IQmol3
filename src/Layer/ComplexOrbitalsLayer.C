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

#include <QProgressDialog>
#include "ComplexOrbitalsLayer.h"
#include "Grid/GridProduct.h"
#include "Util/QsLog.h"


using namespace qglviewer;

namespace IQmol {
namespace Layer {

ComplexOrbitals::ComplexOrbitals(Data::ComplexOrbitals& complexOrbitals)
 : Orbitals(complexOrbitals), m_complexOrbitals(complexOrbitals), m_progressDialog(0),
   m_gridProduct(0)
{
   if (orbitalType() == Data::Orbitals::Complex) {
      computeDensityVectors();
   }
   m_availableDensities.append(m_complexOrbitals.densityList());
   qDebug() << "Number of available densities" << m_availableDensities.size();
}



double ComplexOrbitals::alphaOrbitalEnergy(unsigned const i) const 
{ 
   return m_complexOrbitals.alphaOrbitalEnergy(i);
}



double ComplexOrbitals::betaOrbitalEnergy(unsigned const i) const 
{ 
   return m_complexOrbitals.betaOrbitalEnergy(i);
}



void ComplexOrbitals::computeDensityVectors()
{
   size_t N(nBasis());
   size_t Na(nAlpha());
   size_t Nb(nBeta());

   Matrix coeffs({Na, N});

   // Alpha
   Matrix Pa({N, N});
   Matrix const& Car = m_complexOrbitals.alphaRealCoefficients();
   Matrix const& Cai = m_complexOrbitals.alphaImaginaryCoefficients();

   for (size_t i = 0; i < Na; ++i) {
       for (size_t j = 0; j < N; ++j) {
           coeffs(i,j) = Car(i,j);  
       }
   }

   Pa = product(transpose(coeffs), coeffs);

   for (size_t i = 0; i < Na; ++i) {
       for (size_t j = 0; j < N; ++j) {
           coeffs(i,j) = Cai(i,j);  
       }
   }

   Pa += product(transpose(coeffs), coeffs);

   Data::SurfaceType alpha(Data::SurfaceType::AlphaDensity);
   m_availableDensities.append(new Data::Density(alpha, Pa, "Alpha Density"));

   // Beta
   Matrix Pb({N, N});
   Matrix const& Cbr = m_complexOrbitals.betaRealCoefficients();
   Matrix const& Cbi = m_complexOrbitals.betaImaginaryCoefficients();

   for (size_t i = 0; i < Nb; ++i) {
       for (size_t j = 0; j < N; ++j) {
           coeffs(i,j) = Cbr(i,j);  
       }
   }

   Pb = product(transpose(coeffs), coeffs);

   for (size_t i = 0; i < Nb; ++i) {
       for (size_t j = 0; j < N; ++j) {
           coeffs(i,j) = Cbi(i,j);  
       }
   }

   Pb -= product(transpose(coeffs), coeffs);

   Data::SurfaceType beta(Data::SurfaceType::BetaDensity);
   m_availableDensities.append(new Data::Density(beta, Pb, "Beta Density"));

   Data::SurfaceType total(Data::SurfaceType::TotalDensity);
   m_availableDensities.append(new Data::Density(total, Pa+Pb, "Total Density"));

   Data::SurfaceType spin(Data::SurfaceType::SpinDensity);
   m_availableDensities.append(new Data::Density(spin, Pa-Pb, "Spin Density"));
}



QString ComplexOrbitals::description(Data::SurfaceInfo const& info, 
   bool const tooltip)
{
   Data::SurfaceType const& type(info.type());
   QString label;

   if (type.isOrbital()) {
      unsigned index(type.index());
      bool     isAlpha(type.kind() == Data::SurfaceType::AlphaOrbital);

      label = m_complexOrbitals.label(index, isAlpha);

      if (tooltip) {
         double orbitalEnergy(0.0);
         if (isAlpha) {
            orbitalEnergy = m_complexOrbitals.alphaOrbitalEnergy(index);
         }else {
            orbitalEnergy = m_complexOrbitals.betaOrbitalEnergy(index);
         }
         label += "\nEnergy   = " + QString::number(orbitalEnergy, 'f', 3);
       }
   }else {
      // density
      label = type.toString();
   }

   if (tooltip) {
      if (info.isovalueIsPercent()) {
         label += "\nIsovalue = " + QString::number(info.isovalue(), 'f', 0) + "%";
      }else {
         label += "\nIsovalue = " + QString::number(info.isovalue(), 'f', 3);
      }
   }
 
   return label;
}



void ComplexOrbitals::computeFirstOrderDensityMatrix()
{
   QLOG_WARN() << "ComplexOrbitals::computeFirstOrderDensityMatrix NYI";
}



void ComplexOrbitals::firstOrderDensityMatrixFinished()
{
   QLOG_WARN() << "ComplexOrbitals::firstOrderDensityMatrixFinished NYI";
}


} } // end namespace IQmol::Layer
