#pragma once
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

#include "Matrix.h"

#include <functional>


namespace IQmol {

typedef std::function<double (double const, double const, double const)> Function3D;

typedef std::function<Vector const& (double const, double const, double const)> MultiFunction3D;

typedef std::function<int (int const)> IndexMap;

static Function3D NullFunction3D;

static IndexMap IdentityMap;

} // end namespace IQmol
