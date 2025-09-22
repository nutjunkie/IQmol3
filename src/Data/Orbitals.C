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
#include "Util/QsLog.h"
#include <QDebug>
#include <cmath>


namespace IQmol {
namespace Data {

QString Orbitals::toString(OrbitalType const type)
{
   QString s;

   switch (type) {
      case Undefined:          s = "Undefined";                    break;
      case Canonical:          s = "Canonical Orbitals";           break;
      case Localized:          s = "Localized Orbitals";           break;
      case NaturalTransition:  s = "Natural Transition Orbitals";  break;
      case NaturalBond:        s = "Natural Bond Orbitals";        break;
      case Dyson:              s = "Dyson Orbitals";               break;
      case Complex:            s = "Complex Orbitals";             break;
      case Generic:            s = "Generic Orbitals";             break;
   }

   return s;
}


Orbitals::Orbitals(
   OrbitalType const orbitalType,
   ShellList const& shellList,
   QList<double> const& alphaCoefficients, 
   QList<double> const& betaCoefficients,
   QString const& title)
 : m_orbitalType(orbitalType), m_title(title), m_nBasis(0), m_nOrbitals(0),
   m_shellList(shellList)
{
   if (m_shellList.isEmpty() || alphaCoefficients.isEmpty()) {
      QLOG_WARN() << "Empty data in Orbitals constructor";  
      return;
   }

   if (m_title.isEmpty()) m_title = toString(orbitalType);

   m_nBasis     = m_shellList.nBasis();
   m_nOrbitals  = alphaCoefficients.size() / m_nBasis;
   m_restricted = (betaCoefficients.size() != alphaCoefficients.size());

   if (alphaCoefficients.size() != m_nBasis*m_nOrbitals) {
      QLOG_WARN() << "Inconsist alpha orbital data" << toString(m_orbitalType);
      m_nOrbitals = 0;
      return;
   }

   m_alphaCoefficients.resize({m_nOrbitals, m_nBasis});

   unsigned ka(0);
   for (unsigned i = 0; i < m_nOrbitals; ++i) {
       for (unsigned j = 0; j < m_nBasis; ++j, ++ka) {
           m_alphaCoefficients(i,j) = alphaCoefficients[ka];
       }
   }

   if (m_restricted) return;

   if (betaCoefficients.size() != m_nBasis*m_nOrbitals) {
      QLOG_WARN() << "Inconsist beta orbital data" << toString(m_orbitalType);
      m_nOrbitals = 0;
      return;
   }

   m_betaCoefficients.resize({m_nOrbitals, m_nBasis});
   unsigned kb(0);
   for (unsigned i = 0; i < m_nOrbitals; ++i) {
       for (unsigned j = 0; j < m_nBasis; ++j, ++kb) {
           m_betaCoefficients(i,j) = betaCoefficients[kb];
       }
   }
}


bool Orbitals::areOrthonormal() const
{
   Vector const&  overlap(m_shellList.overlapMatrix());
   if (overlap.size() == 0) return true;

   Matrix S({m_nBasis, m_nBasis});
   Matrix T;

   unsigned k(0);
   for (unsigned i = 0; i < m_nBasis; ++i) {
       for (unsigned j = 0; j <=i; ++j, ++k) {
           S(i,j) = S(j,i) = overlap[k];
       }   
   }   

   T = product(S, transpose(m_alphaCoefficients));
   T = product(m_alphaCoefficients, T);
   
   bool pass(true);
   double thresh(1e-8);

   for (unsigned i = 0; i < m_nOrbitals && pass; ++i) {
       if (std::abs(1.0-std::abs(T(i,i))) > thresh) {
          QLOG_WARN() << "Element (" << i << "," << i << ") =" << T(i,i)
                      << "deviation exceeds threshold" << thresh;
          pass = false;
       }
       for (unsigned j = 0; j < i && pass; ++j) {
           if (std::abs(T(i,j)) > thresh) {
              QLOG_WARN() << "Element (" << i << "," << j << ") =" << T(i,j) 
                          << "exceeds threshold" << thresh;
              pass = false;
           }
       } 
   }


   return pass;
}


QString Orbitals::label(unsigned index, bool) const
{
   return QString::number(index+1);
}


QStringList Orbitals::labels(bool alpha) const
{
   unsigned n(m_alphaCoefficients.shape()[0]);
   if (!alpha && !m_restricted) n = m_betaCoefficients.shape()[0];

   QStringList list;
 
   for (unsigned i = 0; i < n; ++i) {
       list.append(label(i,alpha));
   }

   return list;
}


bool Orbitals::consistent() const 
{ 
   bool consistent(m_nBasis == m_shellList.nBasis());

   consistent = consistent  &&
      m_nOrbitals > 0       && 
      m_nBasis    > 0       && 
      m_nOrbitals <= m_nBasis;

   bool orthonormal(areOrthonormal());
   // disable this check for the time being as noise can cause problems
   // consistent = consistent && orthonormal;

   if (!consistent || !orthonormal) {
      qDebug() << "Inconsistent orbital information";
      qDebug() << "Orbitals:    " << m_nOrbitals;
      qDebug() << "Basis:       " << m_nBasis;
      qDebug() << "Orthonormal: " << orthonormal;
   }

   return consistent;
}


Matrix const& Orbitals::alphaCoefficients() const 
{ 
   return m_alphaCoefficients; 
}


Matrix const& Orbitals::betaCoefficients()  const 
{ 
   return restricted() ? m_alphaCoefficients :  m_betaCoefficients;
}


void Orbitals::reorderFromQChem(Matrix& C)
{
   m_shellList.reorderFromQChem(C);
}


void Orbitals::dump() const 
{
   qDebug() << "There are  " << m_nBasis << "basis functions and" << m_nOrbitals << "orbitals";
   qDebug() << "There ares " << m_shellList.size() << "shells";
   qDebug() << "Restricted:" << m_restricted;

   m_shellList.dump();

   QString check = (consistent() ? "OK" : "NOT OK");
   qDebug() << "Consistency check:    " << check;
}

} } // end namespace IQmol::Data
