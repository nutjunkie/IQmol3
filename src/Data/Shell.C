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


/* 
   The ordering of basis functions is taken from the Molden homepage: 

      https://www.theochem.ru.nl/molden/molden_format.html

   This, I believe, is the same as the Gaussian ordering, but I have not
   confirmed this.  These orderings are consistent with the Molden ordering in
   libqints, but not with the ordering in reorderMatMOs.C.
   
   The molden order appears to go from most axial to least axial, and then
   alphabetical within a group.  This was used to infer the ordering for h
   functions, which I could not find online.

     5D: D 0, D+1, D-1, D+2, D-2
     6D: xx, yy, zz, xy, xz, yz

     7F: F 0, F+1, F-1, F+2, F-2, F+3, F-3
    10F: xxx, yyy, zzz, xyy, xxy, xxz, xzz, yzz, yyz, xyz

     9G: G 0, G+1, G-1, G+2, G-2, G+3, G-3, G+4, G-4
    15G: xxxx yyyy zzzz                       (4,0,0)
         xxxy xxxz xyyy yyyz xzzz yzzz        (3,1,0)
         xxyy xxzz yyzz                       (2,2,0)
         xxyz xyyz xyzz                       (2,1,1)

    11H: H 0, H+1, H-1, H+2, H-2, H+3, H-3, H+4, H-4, H+5, H-5
    21H: xxxxx yyyyy zzzzz                    (5,0,0)
         xxxxy xxxxz xyyyy yyyyz xzzzz yzzzz  (4,1,0)
         xxxyy xxxzz xxyyy yyyzz xxzzz yyzzz  (3,2,0)
         xxxyz xyyyz xyzzz                    (3,1,1)
         xxyyz xxyzz xyyzz                    (2,2,1)

   See also: https://github.com/MolSSI/QCSchema/issues/45, although this
   appears to indicate the FChk and Molden formats are not the same.
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
         switch (index) {
            case 0:  label = QString::fromWCharArray(L"dz\x00B2");          break; // d0
            case 1:  label = QString::fromWCharArray(L"dxz");               break; // d+1
            case 2:  label = QString::fromWCharArray(L"dyz");               break; // d-1
            case 3:  label = QString::fromWCharArray(L"dx\x00B2-y\x00B2");  break; // d+2
            case 4:  label = QString::fromWCharArray(L"dxy");               break; // d-2
         }
         break;

      case D6:
         switch (index) {
            case 0:  label = QString::fromWCharArray(L"dx\x00B2");  break; // xx
            case 1:  label = QString::fromWCharArray(L"dy\x00B2");  break; // yy
            case 2:  label = QString::fromWCharArray(L"dz\x00B2");  break; // zz
            case 3:  label = QString::fromWCharArray(L"dxy");       break; // xy
            case 4:  label = QString::fromWCharArray(L"dxz");       break; // xz
            case 5:  label = QString::fromWCharArray(L"dyz");       break; // yz
         }
         break;

      case F7:
         switch (index) {
            case 0:  label = QString::fromWCharArray(L"f\x2080");        break; // f0
            case 1:  label = QString::fromWCharArray(L"f\x208A\x2081");  break; // f+1
            case 2:  label = QString::fromWCharArray(L"f\x208B\x2081");  break; // f-1
            case 3:  label = QString::fromWCharArray(L"f\x208A\x2082");  break; // f+2
            case 4:  label = QString::fromWCharArray(L"f\x208B\x2082");  break; // f-2
            case 5:  label = QString::fromWCharArray(L"f\x208A\x2083");  break; // f+3
            case 6:  label = QString::fromWCharArray(L"f\x208B\x2083");  break; // f-3
         }
         break;

      case F10:
         
         switch (index) {
            case 0:  label = QString::fromWCharArray(L"fx\x00B3");   break; // xxx
            case 1:  label = QString::fromWCharArray(L"fy\x00B3");   break; // yyy
            case 2:  label = QString::fromWCharArray(L"fz\x00B3");   break; // zzz
            case 3:  label = QString::fromWCharArray(L"fxy\x00B2");  break; // xyy
            case 4:  label = QString::fromWCharArray(L"fx\x00B2y");  break; // xxy
            case 5:  label = QString::fromWCharArray(L"fx\x00B2z");  break; // xxz
            case 6:  label = QString::fromWCharArray(L"fxz\x00B2");  break; // xzz
            case 7:  label = QString::fromWCharArray(L"fyz\x00B2");  break; // yzz
            case 8:  label = QString::fromWCharArray(L"fy\x00B2z");  break; // yyz
            case 9:  label = QString::fromWCharArray(L"fxyz");       break; // xyz
         }
         break;

      case G9:
         switch (index) {
            case 0:  label = QString::fromWCharArray(L"g\x2080");        break; // g0
            case 1:  label = QString::fromWCharArray(L"g\x208A\x2081");  break; // g+1
            case 2:  label = QString::fromWCharArray(L"g\x208B\x2081");  break; // g-1
            case 3:  label = QString::fromWCharArray(L"g\x208A\x2082");  break; // g+2
            case 4:  label = QString::fromWCharArray(L"g\x208B\x2082");  break; // g-2
            case 5:  label = QString::fromWCharArray(L"g\x208A\x2083");  break; // g+3
            case 6:  label = QString::fromWCharArray(L"g\x208B\x2083");  break; // g-3
            case 7:  label = QString::fromWCharArray(L"g\x208A\x2084");  break; // g+4
            case 8:  label = QString::fromWCharArray(L"g\x208B\x2084");  break; // g-4
         }
         break;

      case G15:
         switch (index) {
            case  0:  label = QString::fromWCharArray(L"gx\x2074");         break; // xxxx
            case  1:  label = QString::fromWCharArray(L"gy\x2074");         break; // yyyy
            case  2:  label = QString::fromWCharArray(L"gz\x2074");         break; // zzzz
            case  3:  label = QString::fromWCharArray(L"gx\x00B3y");        break; // xxxy
            case  4:  label = QString::fromWCharArray(L"gx\x00B3z");        break; // xxxz
            case  5:  label = QString::fromWCharArray(L"gxy\x00B3");        break; // xyyy
            case  6:  label = QString::fromWCharArray(L"gy\x00B3z");        break; // yyyz
            case  7:  label = QString::fromWCharArray(L"gxz\x00B3");        break; // zzzx
            case  8:  label = QString::fromWCharArray(L"gyz\x00B3");        break; // zzzy
            case  9:  label = QString::fromWCharArray(L"gx\x00B2y\x00B2");  break; // xxyy
            case 10:  label = QString::fromWCharArray(L"gx\x00B2z\x00B2");  break; // xxzz
            case 11:  label = QString::fromWCharArray(L"gy\x00B2z\x00B2");  break; // yyzz
            case 12:  label = QString::fromWCharArray(L"gx\x00B2yz");       break; // xxyz
            case 13:  label = QString::fromWCharArray(L"gxy\x00B2z");       break; // xyyz
            case 14:  label = QString::fromWCharArray(L"gxyz\x00B2");       break; // xyzz
         }
         break;

      case H11:
         switch (index) {
            case  0:  label = QString::fromWCharArray(L"h\x2080");        break; // h0
            case  1:  label = QString::fromWCharArray(L"h\x208A\x2081");  break; // h+1
            case  2:  label = QString::fromWCharArray(L"h\x208B\x2081");  break; // h-1
            case  3:  label = QString::fromWCharArray(L"h\x208A\x2082");  break; // h+2
            case  4:  label = QString::fromWCharArray(L"h\x208B\x2082");  break; // h-2
            case  5:  label = QString::fromWCharArray(L"h\x208A\x2083");  break; // h+3
            case  6:  label = QString::fromWCharArray(L"h\x208B\x2083");  break; // h-3
            case  7:  label = QString::fromWCharArray(L"h\x208A\x2084");  break; // h+4
            case  8:  label = QString::fromWCharArray(L"h\x208B\x2084");  break; // h-4
            case  9:  label = QString::fromWCharArray(L"h\x208A\x2085");  break; // h+5
            case 10:  label = QString::fromWCharArray(L"h\x208B\x2085");  break; // h-5
         }
         break;

      case H21:
         switch (index) {
            case  0:  label = QString::fromWCharArray(L"hx\x2075");         break; // xxxxx
            case  1:  label = QString::fromWCharArray(L"hy\x2075");         break; // yyyyy
            case  2:  label = QString::fromWCharArray(L"hz\x2075");         break; // zzzzz
            case  3:  label = QString::fromWCharArray(L"hx\x2074y");        break; // xxxxy
            case  4:  label = QString::fromWCharArray(L"hx\x2074z");        break; // xxxxz
            case  5:  label = QString::fromWCharArray(L"hxy\x2074");        break; // xyyyy
            case  6:  label = QString::fromWCharArray(L"hy\x2074z");        break; // yyyyz
            case  7:  label = QString::fromWCharArray(L"hxz\x2074");        break; // xzzzz
            case  8:  label = QString::fromWCharArray(L"hyz\x2074");        break; // yzzzz
            case  9:  label = QString::fromWCharArray(L"hx\x00B3y\x00B2");  break; // xxxyy
            case 10:  label = QString::fromWCharArray(L"hx\x00B3z\x00B2");  break; // xxxzz
            case 11:  label = QString::fromWCharArray(L"hx\x00B2y\x00B3");  break; // xxyyy
            case 12:  label = QString::fromWCharArray(L"hy\x00B3z\x00B2");  break; // yyyzz
            case 13:  label = QString::fromWCharArray(L"hx\x00B2z\x00B3");  break; // xxzzz
            case 14:  label = QString::fromWCharArray(L"hy\x00B2z\x00B3");  break; // yyzzz
            case 15:  label = QString::fromWCharArray(L"hx\x00B3yz");       break; // xxxyz
            case 16:  label = QString::fromWCharArray(L"hxy\x00B3z");       break; // xyyyz
            case 17:  label = QString::fromWCharArray(L"hxyz\x00B3");       break; // xyzzz
            case 18:  label = QString::fromWCharArray(L"hx\x00B2y\x00B2z"); break; // xxyyz
            case 19:  label = QString::fromWCharArray(L"hx\x00B2yz\x00B2"); break; // xxyzz
            case 20:  label = QString::fromWCharArray(L"hxy\x00B2z\x00B2"); break; // xyyzz
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

   m_values.resize(nFunctions(L));
   // This is nasty.  All input data are in angstroms, except the contraction
   // coefficients as they need to be tweaked based on the exponent (in
   // angstroms), so we do it here.
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


// Returns a null pointer if grid point is outside the significant radius
// Pure forms taken from Appendix A in The Theory of Intermolecular Forces
// by Anthony Stone
double const* Shell::evaluate(double const gx, double const gy, double const gz)
{
   static const double f2     = 0.5;
   static const double f4     = 0.25;
   static const double f8     = 0.125;
   static const double f16    = 0.0625;

   static const double rt3    = std::sqrt(3.0);
   static const double rt5    = std::sqrt(5.0);
   static const double rt6    = std::sqrt(6.0);
   static const double rt7    = std::sqrt(7.0);
   static const double rt10   = std::sqrt(10.0);
   static const double rt14   = std::sqrt(14.0);
   static const double rt15   = std::sqrt(15.0);
   static const double rt21   = std::sqrt(21.0);
   static const double rt35   = std::sqrt(35.0);
   static const double rt63   = std::sqrt(63.0);
   static const double rt70   = std::sqrt(70.0);
   static const double rt105  = std::sqrt(105.0);
   static const double rt35o3 = std::sqrt(35.0/3.0);

   double x(gx-m_position.x);
   double y(gy-m_position.y);
   double z(gz-m_position.z);
   double r2(x*x + y*y + z*z);

   // bail early if the basis function does not reach the grid point.
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
         m_values[0] = s * x;
         m_values[1] = s * y;
         m_values[2] = s * z;
         break;

      case SP:
         // These are converted to s and p shells, so should never be called
         m_values[0] = s;
         m_values[1] = s * x;
         m_values[2] = s * y;
         m_values[3] = s * z;
         break;

      case D5:
         m_values[0] = s * (3*z*z - r2) * f2    ; // d0
         m_values[1] = s * (x*z)        * rt3   ; // d+1
         m_values[2] = s * (y*z)        * rt3   ; // d-1
         m_values[3] = s * (x*x - y*y)  * rt3*f2; // d+2
         m_values[4] = s * (x*y)        * rt3   ; // d-2
         break;

      case D6:
         m_values[0] = s * (x*x)      ; // xx
         m_values[1] = s * (y*y)      ; // yy
         m_values[2] = s * (z*z)      ; // zz
         m_values[3] = s * (x*y) * rt3; // xy
         m_values[4] = s * (x*z) * rt3; // xz
         m_values[5] = s * (y*z) * rt3; // yz
         break;

      case F7:
         m_values[0] = s * z * (5*z*z - 3*r2 ) * f2     ; // f0
         m_values[1] = s * x * (5*z*z -   r2 ) * f4*rt6 ; // f+1
         m_values[2] = s * y * (5*z*z -   r2 ) * f4*rt6 ; // f-1
         m_values[3] = s * z * (  x*x -   y*y) * f2*rt15; // f+2
         m_values[4] = s * x*y*z               * rt15   ; // f-2
         m_values[5] = s * x * (  x*x - 3*y*y) * f4*rt10; // f+3
         m_values[6] = s * y * (3*x*x -   y*y) * f4*rt10; // f-3
      break;

      case F10:
         m_values[0] = s * (x*x*x)       ; // xxx
         m_values[1] = s * (y*y*y)       ; // yyy
         m_values[2] = s * (z*z*z)       ; // zzz
         m_values[3] = s * (x*y*y) * rt5 ; // xyy
         m_values[4] = s * (x*x*y) * rt5 ; // xxy
         m_values[5] = s * (x*x*z) * rt5 ; // xxz
         m_values[6] = s * (x*z*z) * rt5 ; // xzz
         m_values[7] = s * (y*z*z) * rt5 ; // yzz
         m_values[8] = s * (y*y*z) * rt5 ; // yyz
         m_values[9] = s * (x*y*z) * rt15; // xyz
         break;

      case G9: {
         double x2(x*x), y2(y*y), z2(z*z);
         m_values[0] = s * (35*z2*z2 - 30*z2*r2 + 3*r2*r2) * f8     ; // g0
         m_values[1] = s *  x*z      * (7*z2 - 3*r2)       * f4*rt10; // g+1
         m_values[2] = s *  y*z      * (7*z2 - 3*r2)       * f4*rt10; // g-1
         m_values[3] = s * (x2 - y2) * (7*z2 -   r2)       * f4*rt5 ; // g+2
         m_values[4] = s *  x*y      * (7*z2 -   r2)       * f2*rt5 ; // g-2
         m_values[5] = s *  x*z      * (  x2 - 3*y2)       * f4*rt70; // g+3
         m_values[6] = s *  y*z      * (3*x2 -   y2)       * f4*rt70; // g-3
         m_values[7] = s * (x2*x2 - 6*x2*y2 + y2*y2)       * f8*rt35; // g+4
         m_values[8] = s *  x*y      * (  x2 -   y2)       * f2*rt35; // g-4
      }  break;

      case G15:
         m_values[ 0] = s * (x*x*x*x)         ; // xxxx
         m_values[ 1] = s * (y*y*y*y)         ; // yyyy
         m_values[ 2] = s * (z*z*z*z)         ; // zzzz
         m_values[ 3] = s * (x*x*x*y) * rt7   ; // xxxy
         m_values[ 4] = s * (x*x*x*z) * rt7   ; // xxxz
         m_values[ 5] = s * (x*y*y*y) * rt7   ; // xyyy
         m_values[ 6] = s * (y*y*y*z) * rt7   ; // yyyz
         m_values[ 7] = s * (x*z*z*z) * rt7   ; // xzzz
         m_values[ 8] = s * (y*z*z*z) * rt7   ; // yzzz
         m_values[ 9] = s * (x*x*y*y) * rt35o3; // xxyy
         m_values[10] = s * (x*x*z*z) * rt35o3; // xxzz
         m_values[11] = s * (y*y*z*z) * rt35o3; // yyzz
         m_values[12] = s * (x*x*y*z) * rt35  ; // xxyz
         m_values[13] = s * (x*y*y*z) * rt35  ; // xyyz
         m_values[14] = s * (x*y*z*z) * rt35  ; // xyzz
         break;

      case H11: {
         double x2(x*x),   y2(y*y),   z2(z*z);
         double x4(x2*x2), y4(y2*y2), z4(z2*z2), r4(r2*r2);
         // Need the ordering of these
         m_values[ 0] = s * z * (63*z4 - 70*z2*r2 + 15*r4)                * f8        ; // h0
         m_values[ 1] = s * x * (21*z4 - 14*z2*r2 +    r4)                * f8*rt15   ; // h+1
         m_values[ 2] = s * y * (21*z4 - 14*z2*r2 +    r4)                * f8*rt15   ; // h-1
         m_values[ 3] = s * z * (3*z2*(x2-y2) - x2*(x2-y2))               * f4*rt105  ; // h+2
         m_values[ 4] = s * x*y*z * (3*z2-r2)                             * f2*rt105  ; // h-2
         m_values[ 5] = s * x * ( 9*x2*z2 - 27*y2*z2 -   x2*r2 + 3*y2*r2) * f16*rt70  ; // h+3
         m_values[ 6] = s * y * (27*x2*z2 -  9*y2*z2 - 3*x2*r2 +   y2*r2) * f16*rt70  ; // h-3
         m_values[ 7] = s * z * (x4 - 6*x2*y2+ y4)                        * f8*rt35*3 ; // h+4
         m_values[ 8] = s * x*y*z * (x2-y2)                               * f2*rt35*3 ; // h+4
         m_values[ 9] = s * x * (  x4 - 10*x2*y2 + 5*y4)                  * f16*rt14*3; // h+5
         m_values[10] = s * y * (5*x4 - 10*x2*y2 +   y4)                  * f16*rt14*3; // h-5
      }  break;
 
      case H21:
         m_values[ 0] = s * x*x*x*x*x        ; // xxxxx
         m_values[ 1] = s * y*y*y*y*y        ; // yyyyy
         m_values[ 2] = s * z*z*z*z*z        ; // zzzzz
         m_values[ 3] = s * x*x*x*x*y * 3    ; // xxxxy
         m_values[ 4] = s * x*x*x*x*z * 3    ; // xxxxz
         m_values[ 5] = s * x*y*y*y*y * 3    ; // xyyyy
         m_values[ 6] = s * y*y*y*y*z * 3    ; // yyyyz
         m_values[ 7] = s * x*z*z*z*z * 3    ; // xzzzz
         m_values[ 8] = s * y*z*z*z*z * 3    ; // yzzzz
         m_values[ 9] = s * x*x*x*y*y * rt21 ; // xxxyy
         m_values[10] = s * x*x*x*z*z * rt21 ; // xxxzz
         m_values[11] = s * x*x*y*y*y * rt21 ; // xxyyy
         m_values[12] = s * y*y*y*z*z * rt21 ; // yyyzz
         m_values[13] = s * x*x*z*z*z * rt21 ; // xxzzz
         m_values[14] = s * y*y*z*z*z * rt21 ; // yyzzz
         m_values[15] = s * x*x*x*y*z * rt63 ; // xxxyz
         m_values[16] = s * x*y*y*y*z * rt63 ; // xyyyz
         m_values[17] = s * x*y*z*z*z * rt63 ; // xyzzz
         m_values[18] = s * x*x*y*y*z * rt105; // xxyyz
         m_values[19] = s * x*x*y*z*z * rt105; // xxyzz
         m_values[20] = s * x*y*y*z*z * rt105; // xyyzz
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
