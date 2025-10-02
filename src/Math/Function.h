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
using MultiFunction3D = std::function<Vector const& (double const, double const, double const)>;
using IndexMap = std::function<int (int const)>;

static Function3D NullFunction3D;


// Adaptor to convert a Funtion3D into a MultiFunction3D
inline MultiFunction3D MultiFunctionAdaptor(Function3D f)
{
    struct Holder {
        Function3D f;
        mutable Vector buf; 

        explicit Holder(Function3D g) : f(std::move(g)) {
            buf.resize({1});
        }

        // const: safe to call through a const lambda capture
        Vector const& eval(double x, double y, double z) const {
            buf[0] = f(x,y,z);
            return buf;
        }
    };

    // Capture the holder by value so the buffer lives with the callable
    return [h = Holder{std::move(f)}](double x, double y, double z) -> Vector const& {
        return h.eval(x,y,z);
    };
}

} // end namespace IQmol
