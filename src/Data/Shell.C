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

#include "Shell.h"
#include "Util/QsLog.h"
#include <QDebug>
#include <cmath>
#include <limits>


using qglviewer::Vec;

namespace IQmol {
namespace Data {


// ----- Static Functions -----

QString Shell::toString(AngularMomentum const L)
{
   QString s;
   switch (L) {
      case S:    s = "S";    break;
      case P:    s = "P";    break;
      case SP:   s = "SP";   break;
      case D5:   s = "D5";   break;
      case D6:   s = "D6";   break;
      case F7:   s = "F7";   break;
      case F10:  s = "F10";  break;
      case G9:   s = "G9";   break;
      case G15:  s = "G15";  break;
      case H11:  s = "H11";  break;
      case H21:  s = "H21";  break;
   }
   return s;
}


unsigned Shell::nFunctions(AngularMomentum const L)
{
   unsigned n(0);
   switch (L) {
      case S:    n =  1;  break;
      case P:    n =  3;  break;
      case SP:   n =  4;  break;
      case D5:   n =  5;  break;
      case D6:   n =  6;  break;
      case F7:   n =  7;  break;
      case F10:  n = 10;  break;
      case G9:   n =  9;  break;
      case G15:  n = 15;  break;
      case H11:  n = 11;  break;
      case H21:  n = 21;  break;
   }
   return n;
}


/* https://www.theochem.ru.nl/molden/molden_format.html
         
   From the Molden homepage, this is the expected basis function
   ordering, which I believe is the same as the Gaussian ordering.
   These are consistent with libqints molden ordering, but not with
   the ordering in reorderMatMOs.C, which appers to be that from mwfn.

   5D: D 0, D+1, D-1, D+2, D-2
   6D: xx, yy, zz, xy, xz, yz

   7F: F 0, F+1, F-1, F+2, F-2, F+3, F-3
  10F: xxx, yyy, zzz, xyy, xxy, xxz, xzz, yzz, yyz, xyz

   9G: G 0, G+1, G-1, G+2, G-2, G+3, G-3, G+4, G-4
  15G: xxxx yyyy zzzz xxxy xxxz yyyx yyyz zzzx zzzy xxyy xxzz yyzz xxyz yyxz zzxy

  11H: H 0, H+1, H-1, H+2, H-2, H+3, H-3, H+4, H-4, H+5, H-5
  21H: ???

  See also: https://github.com/MolSSI/QCSchema/issues/45, although this appears
  to indicate the FChk and Molden formats are not the same.
*/

QString Shell::label(AngularMomentum const L, unsigned const index) 
{
   QString label;

   switch (L) {

      case S:
         switch (index) {
            case 0: label = "s";  break;
         }
         break;

      case P:
         switch (index) {
            case 0:  label = "px";  break;
            case 1:  label = "py";  break;
            case 2:  label = "pz";  break;
         }
         break;

      case SP:
         switch (index) {
            case 0:  label = "s";   break;
            case 1:  label = "px";  break;
            case 2:  label = "py";  break;
            case 3:  label = "pz";  break;
         }
         break;

      case D5:
         // 3ZZ-RR  XZ  YZ  XX-YY  XY
         switch (index) {
            case 0:  label = QString::fromWCharArray(L"dz\x00B2"); break;
            case 1:  label = QString::fromWCharArray(L"dxz");      break;
            case 2:  label = QString::fromWCharArray(L"dyz");      break;
            case 3:  label = QString::fromWCharArray(L"dx\x00B2-y\x00B2"); break;
            case 4:  label = QString::fromWCharArray(L"dxy"); break;
         }
         break;

      case D6:
         // XX  YY  ZZ  XY  XZ  YZ
         switch (index) {
            case 0:  label = QString::fromWCharArray(L"dx\x00B2"); break;
            case 1:  label = QString::fromWCharArray(L"dy\x00B2"); break;
            case 2:  label = QString::fromWCharArray(L"dz\x00B2"); break;
            case 3:  label = QString::fromWCharArray(L"dxy"); break;
            case 4:  label = QString::fromWCharArray(L"dxz"); break;
            case 5:  label = QString::fromWCharArray(L"dyz"); break;
         }
         break;

      case F7:
         // ZZZ-ZRR  XZZ-XRR  YZZ-YRR  XXZ-YYZ  XYZ  XXX-XYY  XXY-YYY
         switch (index) {
            case 0:  label = QString::fromWCharArray(L"fz(z\x00B2-r\x00B2)"); break;
            case 1:  label = QString::fromWCharArray(L"fx(z\x00B2-r\x00B2)"); break;
            case 2:  label = QString::fromWCharArray(L"fy(z\x00B2-r\x00B2)"); break;
            case 3:  label = QString::fromWCharArray(L"fz(x\x00B2-y\x00B2)"); break;
            case 4:  label = QString::fromWCharArray(L"fxyz");                break;
            case 5:  label = QString::fromWCharArray(L"fx(x\x00B2-y\x00B2)"); break;
            case 6:  label = QString::fromWCharArray(L"fy(x\x00B2-y\x00B2)"); break;
         }
         break;

      case F10:
         // XXX  YYY  ZZZ  XYY  XXY  XXZ  XZZ  YZZ  YYZ  XYZ
         switch (index) {
            case 0:  label = QString::fromWCharArray(L"fx\x00B3");  break;
            case 1:  label = QString::fromWCharArray(L"fy\x00B3");  break;
            case 2:  label = QString::fromWCharArray(L"fz\x00B3");  break;
            case 3:  label = QString::fromWCharArray(L"fxy\x00B2"); break;
            case 4:  label = QString::fromWCharArray(L"fx\x00B2y"); break;
            case 5:  label = QString::fromWCharArray(L"fx\x00B2z"); break;
            case 6:  label = QString::fromWCharArray(L"fxz\x00B2"); break;
            case 7:  label = QString::fromWCharArray(L"fyz\x00B2"); break;
            case 8:  label = QString::fromWCharArray(L"fy\x00B2z"); break;
            case 9:  label = QString::fromWCharArray(L"fxyz");      break;
         }
         break;

      case G9:
         label = "g";
         break;

      case G15:
         // This is molden format
         // XXXX YYYY ZZZZ XXXY XXXZ XYYY YYYZ ZZZX ZZZY XXYY XXZZ YYZZ XXYZ XYYZ XYZZ
         switch (index) {
            case  0:  label = QString::fromWCharArray(L"gx\x2074");         break;
            case  1:  label = QString::fromWCharArray(L"gy\x2074");         break;
            case  2:  label = QString::fromWCharArray(L"gz\x2074");         break;
            case  3:  label = QString::fromWCharArray(L"gx\x00B3y");        break;
            case  4:  label = QString::fromWCharArray(L"gx\x00B3z");        break;
            case  5:  label = QString::fromWCharArray(L"gxy\x00B3");        break;
            case  6:  label = QString::fromWCharArray(L"gy\x00B3z");        break;
            case  7:  label = QString::fromWCharArray(L"gxz\x00B3");        break;
            case  8:  label = QString::fromWCharArray(L"gyz\x00B3");        break;
            case  9:  label = QString::fromWCharArray(L"gx\x00B2y\x00B2");  break;
            case 10:  label = QString::fromWCharArray(L"gx\x00B2z\x00B2");  break;
            case 11:  label = QString::fromWCharArray(L"gy\x00B2z\x00B2");  break;
            case 12:  label = QString::fromWCharArray(L"gx\x00B2yz");       break;
            case 13:  label = QString::fromWCharArray(L"gxy\x00B2z");       break;
            case 14:  label = QString::fromWCharArray(L"gxyz\x00B2");       break;
         }
         break;

      case H11:
         label = "h";
         break;

      case H21:
         switch (index) {
            case  0:  label = QString::fromWCharArray(L"gx\x2075");         break;
            case  1:  label = QString::fromWCharArray(L"gy\x2075");         break;
            case  2:  label = QString::fromWCharArray(L"gz\x2075");         break;

            default:  label = "h";
         }
         break;
   }

   return label;
}


// ----- Member Functions -----

Shell::Shell(AngularMomentum L, unsigned const atomIndex, Vec const& position, 
   QList<double> const& exponents, QList<double> const& contractionCoefficients) 
 : m_angularMomentum(L), m_atomIndex(atomIndex), m_position(position), 
   m_exponents(exponents), m_contractionCoefficients(contractionCoefficients),
   m_significantRadiusSquared(std::numeric_limits<double>::max())
{
   if (m_exponents.size() > m_contractionCoefficients.size()) {
      qDebug() << "Inconsistent array lengths in Shell constructor";
      while (m_exponents.size() > m_contractionCoefficients.size()) {
         m_contractionCoefficients.append(0.0);
      }
   }

   m_values.reserve(nFunctions(L));
   // This is nasty.  All input data are in angstroms, but the contraction
   // coefficients need to be tweaked based on the exponent (in angstroms)
   // so we do it here.
   normalizeToAngstrom();
}


void Shell::normalizeToAngstrom()
{
   static double Ns  = std::pow(2.0/M_PI, 0.75);
   double pf(0.0), ex(0.0);

   switch (m_angularMomentum) {
      case S:    pf =      Ns;  ex = 0.75;  break;
      case P:    pf =  2.0*Ns;  ex = 1.25;  break;
      case SP:   pf =  2.0*Ns;  ex = 1.25;  break;
      case D5:   pf =  4.0*Ns;  ex = 1.75;  break;
      case D6:   pf =  4.0*Ns;  ex = 1.75;  break;
      case F7:   pf =  8.0*Ns;  ex = 2.25;  break;
      case F10:  pf =  8.0*Ns;  ex = 2.25;  break;
      case G9:   pf = 16.0*Ns;  ex = 2.75;  break;
      case G15:  pf = 16.0*Ns;  ex = 2.75;  break;
      case H11:  pf = 32.0*Ns;  ex = 3.25;  break;
      case H21:  pf = 32.0*Ns;  ex = 3.25;  break;
   }   

   for (int i = 0; i < m_exponents.size(); ++i) {
       m_contractionCoefficients[i] *= pf * std::pow(m_exponents[i], ex);
   }   
}

  
// Attempts to give an approximate size of the basis function for use in cutoffs
double Shell::computeSignificantRadius(double const thresh) 
{
   double radius(0.0);
   double alpha, coeff, r, f;
   double const step(0.5);

   int L(m_angularMomentum);
   L = std::abs(L);

   for (int i = 0; i < m_exponents.size(); ++i) {
       alpha = m_exponents[i];
       coeff = std::abs(m_contractionCoefficients[i]);
	   // This is the turning point of the primitive, things should all be
	   // downhill from here.
       r = std::sqrt(0.5*L/alpha);
       f = coeff * std::pow(r, L) * std::exp(-alpha*r*r);

	   // This is a little crusty, but to do things analytically 
	   // requires product logs
       while ((f > thresh) && (r < 100.0)) {
          r += step;
          f = coeff * std::pow(r, L) * std::exp(-alpha*r*r);
       }
       radius = std::max(radius, r);
   }

   return radius;
}


void Shell::boundingBox(Vec& min, Vec& max, double const thresh) 
{
   double r(computeSignificantRadius(thresh));

   Vec d(r, r, r);
   min = m_position - d;
   max = m_position + d;

   m_significantRadiusSquared = r*r;
}


// returns a null pointer if grid point is outside the significant radius
double const* Shell::evaluate(double const gx, double const gy, double const gz)
{
   static const double half   = 0.5;
   static const double quart  = 0.25;
   static const double eighth = 0.125;
   static const double rt3    = std::sqrt(3.0);
   static const double rt5    = std::sqrt(5.0);
   static const double rt7    = std::sqrt(7.0);
   static const double rt15   = std::sqrt(15.0);
   static const double rt35   = std::sqrt(35.0);
   static const double rt70   = std::sqrt(70.0);

   static const double rt3o8  = std::sqrt(3.0/8.0);
   static const double rt5o8  = std::sqrt(5.0/8.0);
   static const double rt35o3 = std::sqrt(35.0/3.0);
   static const double hrt3   = half*rt3;
   static const double hrt15  = half*rt15;

   // bail early if the basis function does not reach the grid point.
   double x(gx-m_position.x);
   double y(gy-m_position.y);
   double z(gz-m_position.z);
   double r2(x*x + y*y + z*z);

   if (r2 > m_significantRadiusSquared) return 0;

   double s(0.0);
   for (int i = 0; i < m_exponents.size(); ++i) {
       s += m_contractionCoefficients.at(i) * std::exp(-m_exponents.at(i) * r2);
   }

   switch (m_angularMomentum) {

      case S:
         m_values[0] = s;
         break;

      case P:
         // X Y Z
         m_values[0] = s * x;
         m_values[1] = s * y;
         m_values[2] = s * z;
         break;

      case SP:
         // S X Y Z
         m_values[0] = s;
         m_values[1] = s * x;
         m_values[2] = s * y;
         m_values[3] = s * z;
         break;

      case D5:
         // 3ZZ-RR  XZ  YZ  XX-YY  XY
         m_values[0] = s * (3*z*z - r2) * half;
         m_values[1] = s * (x*z)        *  rt3;
         m_values[2] = s * (y*z)        *  rt3;
         m_values[3] = s * (x*x - y*y)  * hrt3;
         m_values[4] = s * (x*y)        *  rt3;
         break;

      case D6:
         // XX  YY  ZZ  XY  XZ  YZ
         m_values[0] = s * (x*x)      ;
         m_values[1] = s * (y*y)      ;
         m_values[2] = s * (z*z)      ;
         m_values[3] = s * (x*y) * rt3;
         m_values[4] = s * (x*z) * rt3;
         m_values[5] = s * (y*z) * rt3;
         break;

      case F7:
         // ZZZ-ZRR  XZZ-XRR  YZZ-YRR  XXZ-YYZ  XYZ  XXX-XYY  XXY-YYY
         m_values[0] = s * z * (5*z*z - 3*r2 ) * half ;
         m_values[1] = s * x * (5*z*z -   r2 ) * rt3o8;
         m_values[2] = s * y * (5*z*z -   r2 ) * rt3o8;
         m_values[3] = s * z * (  x*x -   y*y) * hrt15;
         m_values[4] = s * x*y*z               * rt15 ;
         m_values[5] = s * x * (  x*x - 3*y*y) * rt5o8;
         m_values[6] = s * y * (3*x*x -   y*y) * rt5o8;
      break;

      case F10:
         // XXX  YYY  ZZZ  XYY  XXY  XXZ  XZZ  YZZ  YYZ  XYZ
         m_values[0] = s * (x*x*x)       ;
         m_values[1] = s * (y*y*y)       ;
         m_values[2] = s * (z*z*z)       ;
         m_values[3] = s * (x*y*y) * rt5 ;
         m_values[4] = s * (x*x*y) * rt5 ;
         m_values[5] = s * (x*x*z) * rt5 ;
         m_values[6] = s * (x*z*z) * rt5 ;
         m_values[7] = s * (y*z*z) * rt5 ;
         m_values[8] = s * (y*y*z) * rt5 ;
         m_values[9] = s * (x*y*z) * rt15;
         break;

      case G9: {
         double x2(x*x), y2(y*y), z2(z*z);
         m_values[0] = s * (3*r2*r2 - 30*r2*z2 + 35*z2*z2) * eighth     ;
         m_values[1] = s *  x*z      * (7*z2 - 3*r2)       * rt5o8      ;
         m_values[2] = s *  y*z      * (7*z2 - 3*r2)       * rt5o8      ;
         m_values[3] = s * (x2 - y2) * (7*z2 -   r2)       * rt5*quart  ; 
         m_values[4] = s *  x*y      * (7*z2 -   r2)       * rt5*half   ; 
         m_values[5] = s *  x*z      * (  x2 - 3*y2)       * rt70*quart ;
         m_values[6] = s *  y*z      * (3*x2 -   y2)       * rt70*quart ;
         m_values[7] = s * (x2*x2 - 6*x2*y2 + y2*y2)       * rt35*eighth;
         m_values[8] = s *  x*y      * (  x2 -   y2)       * rt35*half  ;
      }  break;

      case G15:
         // XXXX YYYY ZZZZ XXXY XXXZ XYYY YYYZ ZZZX ZZZY XXYY XXZZ YYZZ XXYZ XYYZ XYZZ
         m_values[ 0] = s * (x*x*x*x)         ;
         m_values[ 1] = s * (y*y*y*y)         ;
         m_values[ 2] = s * (z*z*z*z)         ;
         m_values[ 3] = s * (x*x*x*y) * rt7   ;
         m_values[ 4] = s * (x*x*x*z) * rt7   ;
         m_values[ 5] = s * (x*y*y*y) * rt7   ;
         m_values[ 6] = s * (y*y*y*z) * rt7   ;
         m_values[ 7] = s * (x*z*z*z) * rt7   ;
         m_values[ 8] = s * (y*z*z*z) * rt7   ;
         m_values[ 9] = s * (x*x*y*y) * rt35o3;
         m_values[10] = s * (x*x*z*z) * rt35o3;
         m_values[11] = s * (y*y*z*z) * rt35o3;
         m_values[12] = s * (x*x*y*z) * rt35  ;
         m_values[13] = s * (x*y*y*z) * rt35  ;
         m_values[14] = s * (x*y*z*z) * rt35  ;
         break;

      case H11:
         // Need the ordering of these
         m_values[ 0] = 0;
         m_values[ 1] = 0;
         m_values[ 2] = 0;
         m_values[ 3] = 0;
         m_values[ 4] = 0;
         m_values[ 5] = 0;
         m_values[ 6] = 0;
         m_values[ 7] = 0;
         m_values[ 8] = 0;
         m_values[ 9] = 0;
         m_values[10] = 0;
         break;
 
      case H21:
         // Need the ordering of these
         m_values[ 0] = 0;
         m_values[ 1] = 0;
         m_values[ 2] = 0;
         m_values[ 3] = 0;
         m_values[ 4] = 0;
         m_values[ 5] = 0;
         m_values[ 6] = 0;
         m_values[ 7] = 0;
         m_values[ 8] = 0;
         m_values[ 9] = 0;
         m_values[10] = 0;
         m_values[11] = 0;
         m_values[12] = 0;
         m_values[13] = 0;
         m_values[14] = 0;
         m_values[15] = 0;
         m_values[16] = 0;
         m_values[17] = 0;
         m_values[18] = 0;
         m_values[19] = 0;
         m_values[20] = 0;
         // Too many warnings
         //QLOG_WARN() << "Unhandled angular momentum in Shell::evaluate";
         break;
   }

   return &m_values[0];
}


void Shell::dump() const
{
   qDebug() << "Shell data:";
   qDebug() << "   L =" << toString(m_angularMomentum) << "   K =" << m_exponents.size() 
            << "   r^2 =" << QString::number(m_significantRadiusSquared,'f',3)
            << "   at (" << m_position.x << "," << m_position.y << "," << m_position.z << ")";

   for (int i = 0; i < m_exponents.size(); ++i) {
       qDebug() << "      " <<  m_exponents[i] << "   " << m_contractionCoefficients[i];
   }
}

} } // end namespace IQmol::Data
