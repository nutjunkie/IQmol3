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

#include "Data/ShellList.h"
#include "Data/Geometry.h"
#include "Util/Constants.h"
#include "Util/QsLog.h"
#include <QDebug>
#include <cmath>


namespace IQmol {
namespace Data {

ShellList::ShellList(ShellData const& shellData, Geometry const& geometry) : m_sigBasis(0),
   m_orbitalCoefficients(0)
{
   static double const convExponents(std::pow(Constants::BohrToAngstrom, -2.0));
   unsigned nShells(shellData.shellTypes.size());
   unsigned cnt(0);

   for (unsigned shell = 0; shell < nShells; ++shell) {

       QList<double> expts;
       QList<double> coefs;
       QList<double> coefsSP;

       unsigned atom(shellData.shellToAtom.at(shell)-1);
       qglviewer::Vec pos(geometry.position(atom));

       for (unsigned i = 0; i < shellData.shellPrimitives.at(shell); ++i, ++cnt) {
           // Convert exponents from bohr to angstrom.  The conversion factor
           // for the coefficients depends on the angular momentum and the 
           // conversion is effectively done in the  Shell constructor
           expts.append(shellData.exponents.at(cnt)*convExponents);

           coefs.append(shellData.contractionCoefficients.at(cnt));
           if (!shellData.contractionCoefficientsSP.isEmpty()) {
              coefsSP.append(shellData.contractionCoefficientsSP.at(cnt));
           }   
       }   

       // These cases are from the formatted checkpoint file format
       switch (shellData.shellTypes.at(shell)) {
          case 0:
             append( new Shell(Shell::S, atom, pos, expts, coefs) );
             break;
          case -1: 
             append( new Shell(Shell::S, atom, pos, expts, coefs) );
             append( new Shell(Shell::P, atom, pos, expts, coefsSP) );
             break;
          case 1:
             append( new Shell(Shell::P, atom, pos, expts, coefs) );
             break;
          case -2: 
             append( new Shell(Shell::D5, atom, pos, expts, coefs) );
             break;
          case 2:
             append( new Shell(Shell::D6, atom, pos, expts, coefs) );
             break;
          case -3: 
             append( new Shell(Shell::F7, atom, pos, expts, coefs) );
             break;
          case 3:
             append( new Shell(Shell::F10, atom, pos, expts, coefs) );
             break;
          case -4: 
             append( new Shell(Shell::G9, atom, pos, expts, coefs) );
             break;
          case 4:
             append( new Shell(Shell::G15, atom, pos, expts, coefs) );
             break;
          case -5: 
             append( new Shell(Shell::H11, atom, pos, expts, coefs) );
             break;
          case 5:
             append( new Shell(Shell::H21, atom, pos, expts, coefs) );
             break;

          default:
             QString msg("Unknown Shell type found at position ");
             msg += QString::number(shell);
             msg += ", type: "+ QString::number(shellData.shellTypes.at(shell));
             qDebug() << msg;
             break;
       }   
   }   

   unsigned n(nBasis());
   if (shellData.overlapMatrix.size() == (n+1)*n/2) {
      setOverlapMatrix(shellData.overlapMatrix);
   }   

   resize();
}


ShellList::~ShellList() 
{
   if (m_sigBasis) delete [] m_sigBasis;
}

unsigned ShellList::nBasis() const
{
    unsigned n(0);
    for (int i = 0; i < size(); ++i) {
        n += at(i)->nBasis();
    }
   return n;
}


void ShellList::boundingBox(qglviewer::Vec& min, qglviewer::Vec& max, double const thresh) 
{
   if (isEmpty()) {
      min.x = min.y = min.z = 0.0;
      max.x = max.y = max.z = 0.0;
      return;
   }

   at(0)->boundingBox(min, max, thresh);

   qglviewer::Vec tmin, tmax;
   ShellList::const_iterator shell;
   for (shell = begin(); shell != end(); ++shell) {
       (*shell)->boundingBox(tmin, tmax, thresh);
       min.x = std::min(tmin.x, min.x);
       min.y = std::min(tmin.y, min.y);
       min.z = std::min(tmin.z, min.z);
       max.x = std::max(tmax.x, max.x);
       max.y = std::max(tmax.y, max.y);
       max.z = std::max(tmax.z, max.z);
   }
}


void ShellList::dump() const
{
   unsigned n(0), s(0), p(0), sp(0), d5(0), d6(0), f7(0), f10(0);
   unsigned g9(0), g15(0), h11(0), h21(0);

   ShellList::const_iterator shell;
   for (shell = begin(); shell != end(); ++shell) {
       Shell::AngularMomentum L((*shell)->angularMomentum());
       switch (L) {
          case  Shell::S:    ++s;    break;
          case  Shell::P:    ++p;    break;   
          case  Shell::SP:   ++sp;   break;   
          case  Shell::D5:   ++d5;   break;
          case  Shell::D6:   ++d6;   break;   
          case  Shell::F7:   ++f7;   break;
          case  Shell::F10:  ++f10;  break;   
          case  Shell::G9:   ++g9;   break;
          case  Shell::G15:  ++g15;  break;   
          case  Shell::H11:  ++h11;  break;   
          case  Shell::H21:  ++h21;  break;   
       }   
       n += Shell::nFunctions(L);
   }   

   QString check("OK");
   if (n != nBasis()) check.prepend("NOT ");
   qDebug() << "Basis function check:     " << check;
   QString types("    S     P    SP    D5    D6    F7   F10  G9   G15  H11  H21");
   QString tally = QString("%1 %2 %3 %4 %5 %6").arg(  s,5).arg(  p,5).arg( sp,5).arg( d5,5)
                                               .arg( d6,5).arg( f7,5).arg(f10,5).arg( g9,5)
                                               .arg(g15,5).arg(h11,5).arg(h21,5);
   qDebug() << "Shell types:              " << types;
   qDebug() << "                          " << tally;
}


void ShellList::resize()
{
   m_nBasis = nBasis();
   m_basisValues.resize({m_nBasis});

   if (m_sigBasis) delete [] m_sigBasis;
   m_sigBasis = new unsigned[m_nBasis];

   unsigned size(m_nBasis*(m_nBasis+1)/2);
   if (2*size != m_nBasis*(m_nBasis+1)) {
      QLOG_WARN() << "Round error in ShellList::resize()";
      ++size;
   }
   m_basisPairValues.resize({size});

   qDebug() << shellAtomOffsets();
   qDebug() << basisAtomOffsets();
}


QList<unsigned> ShellList::shellAtomOffsets() const
{
   QList<unsigned> offsets;
   offsets.append(0);

   unsigned k(0);
   unsigned atomIndex(0);

   ShellList::const_iterator shell;
   for (shell = begin(); shell != end(); ++shell, ++k) {
       if ((*shell)->atomIndex() != atomIndex) {
          offsets.append(k);
          ++atomIndex;
       }
   }

   return offsets;
}

QList<unsigned> ShellList::basisAtomOffsets() const
{
   QList<unsigned> offsets;
   offsets.append(0);

   unsigned k(0);
   unsigned atomIndex(0);

   ShellList::const_iterator shell;
   for (shell = begin(); shell != end(); ++shell) {
       if ((*shell)->atomIndex() != atomIndex) {
          offsets.append(k);
          ++atomIndex;
       }
       k += (*shell)->nBasis();
   }

   return offsets;
}


// TODO: this could be batched over grid points so that matrix multiplications
// can be used later on.  Also, auxilary data structures could be employed to 
// make the computation more efficient.
Vector const& ShellList::shellValues(double const x, double const y, double const z)
{
   double const* values;
   unsigned offset(0);

   ShellList::const_iterator shell;
   for (shell = begin(); shell != end(); ++shell) {
       values = (*shell)->evaluate(x, y, z);
       if (values) {
          for (unsigned s = 0; s < (*shell)->nBasis(); ++s, ++offset) {
              m_basisValues(offset) = values[s];
          }
       }else{
          for (unsigned s = 0; s < (*shell)->nBasis(); ++s, ++offset) {
              m_basisValues(offset) = 0;
          }
       }
   }

   return m_basisValues;
}

// -----------------------------------------------------------------------------

void ShellList::setDensityVectors(QList<Vector const*> const& densityVectors)
{
   m_densityVectors = densityVectors;
   m_densityValues.resize({(size_t)m_densityVectors.length()});
}


Vector const& ShellList::densityValues(double const x, double const y, double const z)
{
   unsigned numbas, nSigBas(0), basoff(0);
   double const* values;

   // Determine the significant shells, and corresponding basis function indices
   ShellList::const_iterator shell;
   for (shell = begin(); shell != end(); ++shell) {
       values = (*shell)->evaluate(x,y,z);
       numbas = (*shell)->nBasis();

       if (values) { // only add the significant shells
          for (unsigned i = 0; i < numbas; ++i, ++nSigBas, ++basoff) {
              m_basisValues(nSigBas) = values[i];
              m_sigBasis[nSigBas]    = basoff;
          }
       }else {
          basoff += numbas;
       }
   }

   double   xi, xij; 
   unsigned ii, jj, Ti;
   unsigned nden(m_densityVectors.size());

   for (unsigned k = 0; k < nden; ++k) {
       m_densityValues(k) = 0.0;
   }

   // Now compute the basis function pair values on the grid
   for (unsigned i = 0; i < nSigBas; ++i) {
       xi = m_basisValues(i);
       ii = m_sigBasis[i];
       Ti = (ii*(ii+1))/2;
       for (unsigned j = 0; j < i; ++j) {
           xij = 2.0*xi*m_basisValues(j);
           jj  = m_sigBasis[j];

           for (unsigned k = 0; k < nden; ++k) {
               m_densityValues(k) += 2.0*xij*(*m_densityVectors[k])(Ti+jj);
           }

       }
       
       for (unsigned k = 0; k < nden; ++k) {
           m_densityValues(k) += xi*xi*(*m_densityVectors[k])(Ti+ii);
       }
   }

   return m_densityValues;
}

// -----------------------------------------------------------------------------

void ShellList::setOrbitalVectors(Matrix const& coefficients, QList<int> const& indices)
{
   m_orbitalIndices      = indices;
   m_orbitalCoefficients = &coefficients;
   m_orbitalValues.resize({(size_t)m_orbitalIndices.size()});
}


Vector const& ShellList::orbitalValues(double const x, double const y, double const z)
{
   unsigned norb(m_orbitalIndices.size());
   unsigned basoff(0);
   unsigned numbas;
   double const* values;

   m_orbitalValues.zero();

   // Determine the significant shells, and corresponding basis function indices
   ShellList::const_iterator shell;
   for (shell = begin(); shell != end(); ++shell) {
       values = (*shell)->evaluate(x,y,z);
       numbas = (*shell)->nBasis();

       if (values) { // only add the significant shells
          for (unsigned i = 0; i < numbas; ++i) {
              for (unsigned k = 0; k < norb; ++k) {
                  m_orbitalValues(k) += 
                      (*m_orbitalCoefficients)(m_orbitalIndices[k], basoff+i) * values[i];
              }
          }
       }
       basoff += numbas;
   }

   return m_orbitalValues;
}


void ShellList::reorderFromQChem(Matrix& C)
{
   unsigned offset(0);
   unsigned nOrbitals(C.shape()[0]);
   ShellList::iterator shell;

   for (shell = begin(); shell != end(); ++shell) {
       Shell::AngularMomentum L((*shell)->angularMomentum());
       switch (L) {

          case Shell::S:
          case Shell::P:
          case Shell::SP:
             break;

          case Shell::D5:
             for (unsigned i = 0; i < nOrbitals; ++i) {
                 double d2m( C(i, offset+0) );
                 double d1m( C(i, offset+1) );
                 double d0 ( C(i, offset+2) );
                 double d1p( C(i, offset+3) );
                 double d2p( C(i, offset+4) );
                 C(i, offset+0) = d0;
                 C(i, offset+1) = d1p;
                 C(i, offset+2) = d1m;
                 C(i, offset+3) = d2p;
                 C(i, offset+4) = d2m;
             }
             break;

          case Shell::D6:
             for (unsigned i = 0; i < nOrbitals; ++i) {
                 double dxx( C(i, offset+0) );
                 double dxy( C(i, offset+1) );
                 double dyy( C(i, offset+2) );
                 double dxz( C(i, offset+3) );
                 double dyz( C(i, offset+4) );
                 double dzz( C(i, offset+5) );
                 C(i, offset+0) = dxx;
                 C(i, offset+1) = dyy;
                 C(i, offset+2) = dzz;
                 C(i, offset+3) = dxy;
                 C(i, offset+4) = dxz;
                 C(i, offset+5) = dyz;
             }
             break;

          case Shell::F7:
             for (unsigned i = 0; i < nOrbitals; ++i) {
                 double f3m( C(i, offset+0) );
                 double f2m( C(i, offset+1) );
                 double f1m( C(i, offset+2) );
                 double f0 ( C(i, offset+3) );
                 double f1p( C(i, offset+4) );
                 double f2p( C(i, offset+5) );
                 double f3p( C(i, offset+6) );
                 C(i, offset+0) = f0;
                 C(i, offset+1) = f1p;
                 C(i, offset+2) = f1m;
                 C(i, offset+3) = f2p;
                 C(i, offset+4) = f2m;
                 C(i, offset+5) = f3p;
                 C(i, offset+6) = f3m;
             }
             break;

          case Shell::F10:
             for (unsigned i = 0; i < nOrbitals; ++i) {
                 double fxxx( C(i, offset+0) );
                 double fxxy( C(i, offset+1) );
                 double fxyy( C(i, offset+2) );
                 double fyyy( C(i, offset+3) );
                 double fxxz( C(i, offset+4) );
                 double fxyz( C(i, offset+5) );
                 double fyyz( C(i, offset+6) );
                 double fxzz( C(i, offset+7) );
                 double fyzz( C(i, offset+8) );
                 double fzzz( C(i, offset+9) );
                 C(i, offset+0) = fxxx;
                 C(i, offset+1) = fyyy;
                 C(i, offset+2) = fzzz;
                 C(i, offset+3) = fxyy;
                 C(i, offset+4) = fxxy;
                 C(i, offset+5) = fxxz;
                 C(i, offset+6) = fxzz;
                 C(i, offset+7) = fyzz;
                 C(i, offset+8) = fyyz;
                 C(i, offset+9) = fxyz;
             }
             break;

          case Shell::G9:
             for (unsigned i = 0; i < nOrbitals; ++i) {
                 double g4m( C(i, offset+0) );
                 double g3m( C(i, offset+1) );
                 double g2m( C(i, offset+2) );
                 double g1m( C(i, offset+3) );
                 double g0 ( C(i, offset+4) );
                 double g1p( C(i, offset+5) );
                 double g2p( C(i, offset+6) );
                 double g3p( C(i, offset+7) );
                 double g4p( C(i, offset+8) );
                 C(i, offset+0) = g0;
                 C(i, offset+1) = g1p;
                 C(i, offset+2) = g1m;
                 C(i, offset+3) = g2p;
                 C(i, offset+4) = g2m;
                 C(i, offset+5) = g3p;
                 C(i, offset+6) = g3m;
                 C(i, offset+7) = g4p;
                 C(i, offset+8) = g4m;
             }
             break;

          case Shell::G15:
             for (unsigned i = 0; i < nOrbitals; ++i) {
#if 0
                 double gxxxx( C(i, offset+ 0) );
                 double gxxxy( C(i, offset+ 1) );
                 double gxxyy( C(i, offset+ 2) );
                 double gxyyy( C(i, offset+ 3) );
                 double gyyyy( C(i, offset+ 4) );
                 double gxxxz( C(i, offset+ 5) );
                 double gxxyz( C(i, offset+ 6) );
                 double gxyyz( C(i, offset+ 7) );
                 double gyyyz( C(i, offset+ 8) );
                 double gxxzz( C(i, offset+ 9) );
                 double gxyzz( C(i, offset+10) );
                 double gyyzz( C(i, offset+11) );
                 double gxzzz( C(i, offset+12) );
                 double gyzzz( C(i, offset+13) );
                 double gzzzz( C(i, offset+14) );
#else
                 // This is the ordering in reorderMatMOs, weird, no?
                 double gzzzz( C(i, offset+ 0) );
                 double gyzzz( C(i, offset+ 1) );
                 double gyyzz( C(i, offset+ 2) );
                 double gyyyz( C(i, offset+ 3) );
                 double gyyyy( C(i, offset+ 4) );
                 double gxzzz( C(i, offset+ 5) );
                 double gxyzz( C(i, offset+ 6) );
                 double gxyyz( C(i, offset+ 7) );
                 double gxyyy( C(i, offset+ 8) );
                 double gxxzz( C(i, offset+ 9) );
                 double gxxyz( C(i, offset+10) );
                 double gxxyy( C(i, offset+11) );
                 double gxxxz( C(i, offset+12) );
                 double gxxxy( C(i, offset+13) );
                 double gxxxx( C(i, offset+14) );
#endif
                 C(i, offset+ 0) = gxxxx;
                 C(i, offset+ 1) = gyyyy;
                 C(i, offset+ 2) = gzzzz;
                 C(i, offset+ 3) = gxxxy;
                 C(i, offset+ 4) = gxxxz;
                 C(i, offset+ 5) = gxyyy;
                 C(i, offset+ 6) = gyyyz;
                 C(i, offset+ 7) = gxzzz;
                 C(i, offset+ 8) = gyzzz;
                 C(i, offset+ 9) = gxxyy;
                 C(i, offset+10) = gxxzz;
                 C(i, offset+11) = gyyzz;
                 C(i, offset+12) = gxxyz;
                 C(i, offset+13) = gxyyz;
                 C(i, offset+14) = gxyzz;
             }
             break;

          case Shell::H11:
             for (unsigned i = 0; i < nOrbitals; ++i) {
                 double h5m( C(i, offset+ 0) );
                 double h4m( C(i, offset+ 1) );
                 double h3m( C(i, offset+ 2) );
                 double h2m( C(i, offset+ 3) );
                 double h1m( C(i, offset+ 4) );
                 double h0 ( C(i, offset+ 5) );
                 double h1p( C(i, offset+ 6) );
                 double h2p( C(i, offset+ 7) );
                 double h3p( C(i, offset+ 8) );
                 double h4p( C(i, offset+ 9) );
                 double h5p( C(i, offset+10) );
                 C(i, offset+ 0) = h0;
                 C(i, offset+ 1) = h1p;
                 C(i, offset+ 2) = h1m;
                 C(i, offset+ 3) = h2p;
                 C(i, offset+ 4) = h2m;
                 C(i, offset+ 5) = h3p;
                 C(i, offset+ 6) = h3m;
                 C(i, offset+ 7) = h4p;
                 C(i, offset+ 8) = h4m;
                 C(i, offset+ 9) = h5p;
                 C(i, offset+10) = h5m;
             }
             break;

          case Shell::H21:
             for (unsigned i = 0; i < nOrbitals; ++i) {
                 double hzzzzz( C(i, offset+ 0) );
                 double hyzzzz( C(i, offset+ 1) );
                 double hyyzzz( C(i, offset+ 2) );
                 double hyyyzz( C(i, offset+ 3) );
                 double hyyyyz( C(i, offset+ 4) );
                 double hyyyyy( C(i, offset+ 5) );
                 double hxzzzz( C(i, offset+ 6) );
                 double hxyzzz( C(i, offset+ 7) );
                 double hxyyzz( C(i, offset+ 8) );
                 double hxyyyz( C(i, offset+ 9) );
                 double hxyyyy( C(i, offset+10) );
                 double hxxzzz( C(i, offset+11) );
                 double hxxyzz( C(i, offset+12) );
                 double hxxyyz( C(i, offset+13) );
                 double hxxyyy( C(i, offset+14) );
                 double hxxxzz( C(i, offset+15) );
                 double hxxxyz( C(i, offset+16) );
                 double hxxxyy( C(i, offset+17) );
                 double hxxxxz( C(i, offset+18) );
                 double hxxxxy( C(i, offset+19) );
                 double hxxxxx( C(i, offset+20) );
                 C(i, offset+ 0) = hxxxxx;
                 C(i, offset+ 1) = hyyyyy;
                 C(i, offset+ 2) = hzzzzz;
                 C(i, offset+ 3) = hxxxxy;
                 C(i, offset+ 4) = hxxxxz;
                 C(i, offset+ 5) = hxyyyy;
                 C(i, offset+ 6) = hyyyyz;
                 C(i, offset+ 7) = hxzzzz;
                 C(i, offset+ 8) = hyzzzz;
                 C(i, offset+ 9) = hxxxyy;
                 C(i, offset+10) = hxxxzz;
                 C(i, offset+11) = hxxyyy;
                 C(i, offset+12) = hyyyzz;
                 C(i, offset+13) = hxxzzz;
                 C(i, offset+14) = hyyzzz;
                 C(i, offset+15) = hxxxyz;
                 C(i, offset+16) = hxyyyz;
                 C(i, offset+17) = hxyzzz;
                 C(i, offset+18) = hxxyyz;
                 C(i, offset+19) = hxxyzz;
                 C(i, offset+20) = hxyyzz;
             }
             break;
       }
       offset += Shell::nFunctions(L);
   }
}

} } // end namespace IQmol::Data
