#pragma once
/*******************************************************************************

  Copyright (C) 2022-2025 Andrew Gilbert

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

#include "Vector.h"
#include <functional>


namespace IQmol {

using Function3D = std::function<double (double const, double const, double const)>;
using MultiFunction3D = std::function<void (double const, double const, double const, Vector&)>;
using IndexMap = std::function<int (int const)>;

static Function3D NullFunction3D;


// Adaptor to convert a Funtion3D into a fill-style MultiFunction3D
inline MultiFunction3D MultiFunctionAdaptor(Function3D f)
{
    return [f = std::move(f)](double x, double y, double z, Vector& values) {
        if (values.size() != 1) {
            values.resize({1});
        }
        values[0] = f(x,y,z);
    };
}

} // end namespace IQmol
