/*******************************************************************************

  Copyright (C) 2023 Andrew Gilbert

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

#include "Spline.h"
#include <cmath>


namespace IQmol {

inline float RoundPlaces(float a, int p)
{
   const float powersOfTen[] = {
      1e0f,  1e1f,  1e2f,  1e3f,  1e4f,  1e5f,  1e6f,  1e7f,  
      1e8f,  1e9f, 1e10f, 1e11f, 1e12f, 1e13f, 1e14f, 1e15f, 1e16f 
   };
 
   float s = powersOfTen[p];
   return std::roundf(a*s) / s;
}


inline void RoundPlaces(Vec3 &v, int p)
{
   v[0] = RoundPlaces(v[0], p);
   v[1] = RoundPlaces(v[1], p);
   v[2] = RoundPlaces(v[2], p);
}



inline float InCirc(float t) 
{
   return -1 * (sqrtf(1-t*t) - 1);
}


void spline(Vec3*& result, 
   Vec3 const& vec1, 
   Vec3 const& vec2, 
   Vec3 const& vec3, 
   Vec3 const& vec4, int n) 
{
   float n1 = (float)(n);
   float n2 = (n1 * n1);
   float n3 = (n1 * n1 * n1);

   Matrix s({4,4});
   s.zero();
   s(0,0) = 6 / n3;
   s(1,0) = 6 / n3; s(1,1) = 2 / n2;
   s(2,0) = 1 / n3; s(2,1) = 1 / n2; s(2,2) =  1 / n1;
   s(3,3) = 1.0f;

   float oos = 1.0f / 6.0f;
   Matrix b({4,4});
   b.zero();
   b(0,0) = -1 * oos;  b(0,1) =  3 * oos; b(0,2) = -3 * oos; b(0,3) = 1 * oos;
   b(1,0) =  3 * oos;  b(1,1) = -6 * oos; b(1,2) =  3 * oos;
   b(2,0) = -3 * oos;  b(2,2) =  3 * oos;
   b(3,0) = oos;       b(3,1) =  4 * oos; b(3,2) = oos;

   Matrix g({4,4});
   g(0,0) = vec1[0];  g(0,1) = vec1[1];  g(0,2) = vec1[2];  g(0,3) = 1.0;
   g(1,0) = vec2[0];  g(1,1) = vec2[1];  g(1,2) = vec2[2];  g(1,3) = 1.0;
   g(2,0) = vec3[0];  g(2,1) = vec3[1];  g(2,2) = vec3[2];  g(2,3) = 1.0;
   g(3,0) = vec4[0];  g(3,1) = vec4[1];  g(3,2) = vec4[2];  g(3,3) = 1.0;

   Matrix m({4,4});
   m = product(product(s,b),g);

   Vec3 v {m(3,0) / m(3,3), m(3,1) / m(3,3), m(3,2) / m(3,3)};
   RoundPlaces(v,10);
   result[0] = v;

   for(int k=0;k<n;k++){
       m(3,0) = m(3,0) + m(2,0);
       m(3,1) = m(3,1) + m(2,1);
       m(3,2) = m(3,2) + m(2,2);
       m(3,3) = m(3,3) + m(2,3);
       m(2,0) = m(2,0) + m(1,0);
       m(2,1) = m(2,1) + m(1,1);
       m(2,2) = m(2,2) + m(1,2);
       m(2,3) = m(2,3) + m(1,3);
       m(1,0) = m(1,0) + m(0,0);
       m(1,1) = m(1,1) + m(0,1);
       m(1,2) = m(1,2) + m(0,2);
       m(1,3) = m(1,3) + m(0,3);
       v = Vec3{m(3,0) / m(3,3), m(3,1) / m(3,3), m(3,2) / m(3,3)};
       RoundPlaces(v,10);
       result[k+1] =  v;
   }
}

} // end namespace IQmol

