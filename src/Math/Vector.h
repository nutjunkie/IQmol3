#ifndef IQMOL_MATH_VECTOR_H
#define IQMOL_MATH_VECTOR_H
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

#include <cmath>
#include <QDebug>

namespace IQmol {

class Vec3 {

   public:
      union {
         struct {
            double x, y, z;
         };
         double v[3];
      }; 

      Vec3() : x(0.0), y(0.0), z(0.0) { }
   
      Vec3(double const X, double const Y, double const Z) : x(X), y(Y), z(Z) { }

      Vec3(Vec3 const& that) 
      {
         copy(that);
      }

      Vec3& operator=(Vec3 const& that)
      {
         if (this != &that) copy (that);
         return *this;
      }

      double operator[](unsigned i) const 
      {
         return v[i];
      }

      double norm() const 
      {
         return std::sqrt(x*x + y*y + z*z);
      }

      QString format() const
      {
          QString str("(");
          str += QString::number(x) + "," + QString::number(y) +"," + QString::number(z);
          str += ")";
          return str;
      }

      void dump() const
      {
         qDebug() << format();
      }

   private:
      void copy(Vec3 const& that)
      {
         this->x = that.x;
         this->y = that.y;
         this->z = that.z;
      }
};


} // end namespace IQmol

#endif
