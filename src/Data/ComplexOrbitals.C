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

#include "ComplexOrbitals.h"
#include <QDebug>


namespace IQmol {
namespace Data {

ComplexOrbitals::ComplexOrbitals(
   unsigned const nAlpha, 
   unsigned const nBeta, 
   ShellList const& shells, 
   QList<double> const& alphaRealCoefficients, 
   QList<double> const& alphaImaginaryCoefficients, 
   QList<double> const& alphaEnergies, 
   QList<double> const& betaRealCoefficients, 
   QList<double> const& betaImaginaryCoefficients, 
   QList<double> const& betaEnergies, 
   QString const& label)
    : Orbitals(Orbitals::Complex, shells, alphaRealCoefficients, betaRealCoefficients, label),
      m_nAlpha(nAlpha), m_nBeta(nBeta),
      m_alphaEnergies(alphaEnergies), m_betaEnergies(betaEnergies)
{
   size_t nBasis = shells.nBasis();
   size_t nOrbitals = alphaRealCoefficients.size() / m_nBasis;
   m_alphaImaginaryCoefficients.resize({nOrbitals,nBasis});
   m_betaImaginaryCoefficients.resize({nOrbitals,nBasis});

    unsigned k(0);
    for (unsigned i = 0; i < m_nOrbitals; ++i) {
        for (unsigned j = 0; j < m_nBasis; ++j, ++k) {
            m_alphaImaginaryCoefficients(i,j) = alphaImaginaryCoefficients[k];
            m_betaImaginaryCoefficients(i,j)  = betaImaginaryCoefficients[k];
         }
    }
}


double ComplexOrbitals::alphaOrbitalEnergy(unsigned i) const 
{
   return ((int)i < m_alphaEnergies.size()) ? m_alphaEnergies[i] : 0.0;
}


double ComplexOrbitals::betaOrbitalEnergy(unsigned i) const 
{
   double energy(0.0);
   if (m_restricted) {
      energy = alphaOrbitalEnergy(i);
   }else if ((int)i < m_betaEnergies.size()) {
      energy = m_betaEnergies[i];
   }   
   return energy;
}


bool ComplexOrbitals::consistent() const
{
   bool consistent(m_nBasis == m_shellList.nBasis());

   consistent = consistent  &&  
      m_nOrbitals > 0       &&  
      m_nBasis    > 0       &&  
      m_nOrbitals <= m_nBasis;

   consistent = consistent 
     && m_alphaCoefficients.shape() == m_betaCoefficients.shape() 
     && m_alphaImaginaryCoefficients.shape() == m_betaImaginaryCoefficients.shape();

   if (!consistent) {
      qDebug() << "Inconsistent complex orbital information";
      qDebug() << "Orbitals:    " << m_nOrbitals;
      qDebug() << "Basis:       " << m_nBasis;
   }   

   return consistent;
}


QString ComplexOrbitals::label(unsigned index, bool alpha) const
{
   unsigned n(alpha ? m_nAlpha : m_nBeta);
   QString  s(alpha ? "Complex Alpha " : "Complex Beta ");
   s += QString::number(index+1);

   if (n == index+2) {
      s += " (HOMO-1)";
   }else if (n == index+1) {
      s += " (HOMO)";
   }else if (n == index) {
      s += " (LUMO)";
   }else if (index == n+1) {
      s += " (LUMO+1)";
   }   

   return s;
} 


void ComplexOrbitals::dump() const
{
   Matrix::Shape s = alphaCoefficients().shape();
   qDebug() << "Real alpha size" << s[0] << s[1];
   s = alphaImaginaryCoefficients().shape();
   qDebug() << "Imag alpha size" << s[0] << s[1];

   s = betaCoefficients().shape();
   qDebug() << "Real beta  size" << s[0] << s[1];
   s = betaImaginaryCoefficients().shape();
   qDebug() << "Imag beta  size" << s[0] << s[1];

   Orbitals::dump();
}

} } // end namespace IQmol::Data
