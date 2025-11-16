#pragma once
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

#include <cmath>
#include <sstream>
#include <cassert>
#include <ostream>

#include "Array.h"


namespace IQmol {

struct Vec3 : public ArrayOps<Vec3> 
{
    double x, y, z;

    // Ctors
    constexpr Vec3() noexcept : x(0), y(0), z(0) {}
    constexpr Vec3(double X, double Y, double Z) noexcept : x(X), y(Y), z(Z) { }

    // Rule of zero: default copy/move is fine
    Vec3(const Vec3&)            = default;
    Vec3& operator=(const Vec3&) = default;

    // Element access
    double& operator[](std::size_t i) noexcept {
        assert(i < 3);
        return (i == 0 ? x : (i == 1 ? y : z));
    }

    const double& operator[](std::size_t i) const noexcept {
        assert(i < 3);
        return (i == 0 ? x : (i == 1 ? y : z));
    }

    // Note: Standard doesn't *guarantee* no padding between members, but on
    // all real-world ABIs for a POD of three doubles this is contiguous.
    double const* data() const noexcept { return &x; }
    double*       data()       noexcept { return &x; }

    constexpr size_t size() const noexcept { return 3; }

    double norm2() const noexcept { return x*x + y*y + z*z; }
    double norm()  const noexcept { return std::sqrt(norm2()); }

    std::string format() const {
        std::ostringstream ss;
        ss << '(' << x << ',' << y << ',' << z << ')';
        return ss.str();
    }

    void normalize()
    {
       double n(norm());
       if (n < 1e-6) return;
       x /= n;
       y /= n;
       z /= n;
    }
};


inline std::ostream& operator<<(std::ostream& os, const Vec3& v) 
{
   return os << v.format();
}


inline constexpr double dot(const Vec3& a, const Vec3& b) noexcept 
{
   return a.x*b.x + a.y*b.y + a.z*b.z;
}


inline constexpr Vec3 cross(const Vec3& a, const Vec3& b) noexcept 
{
    return { a.y*b.z - a.z*b.y,
             a.z*b.x - a.x*b.z,
             a.x*b.y - a.y*b.x };
}


template<class T>
inline T dot(const Array<1, T>& a, const Array<1, T>& b)
{
    assert(a.size() == b.size());

    const std::size_t n = a.size();
    const T* pa = a.data();
    const T* pb = b.data();

    T acc{};
    for (std::size_t i = 0; i < n; ++i) {
        acc += pa[i] * pb[i];
    }
    return acc;
}

typedef Array<1,double> Vector;
typedef Array<1,const double> ConstVector;

} // end namespace IQmol
