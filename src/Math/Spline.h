#pragma once
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


#include "TMatrix.h"
#include <cmath>
#include "v3.h"


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


inline void RoundPlaces(v3 &v, int p)
{
    v.x = RoundPlaces(v.x, p);
    v.y = RoundPlaces(v.y, p);
    v.z = RoundPlaces(v.z, p);
}



inline float Linear(float t) {
    return t;
}


inline float InOutQuad(float t) {
    if (t < 0.5f) {
        return 2 * t * t;
    }
    t = 2*t - 1;
    return -0.5f * (t*(t-2) - 1);
}


inline float OutCirc(float t) {
    t = t-1;
    return sqrtf(1 - (t * t));
}


inline float InCirc(float t) {
    return -1 * (sqrtf(1-t*t) - 1);
}

void spline(v3 *&result, const v3 &vec1, const v3 &vec2, const v3 &vec3, const v3 &vec4, int n);

} // end namespace IQmol

