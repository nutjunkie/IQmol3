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

#include "Data.h"
#include <QDebug>
#include "QGLViewer/vec.h"


namespace IQmol {
namespace Data {

   class DipoleMoment : public Base {

      public:
         DipoleMoment(qglviewer::Vec const& dipole = qglviewer::Vec()) : m_dipole(dipole) { }
         Type::ID typeID() const { return Type::DipoleMoment; }
         void setValue(double const x, double const y, double const z) { 
            m_dipole.setValue(x,y,z); 
         } 

         qglviewer::Vec const& value() const { return m_dipole; }

         void dump() const {
            qDebug() << "  mu = (" << m_dipole.x << "," << m_dipole.y << "," 
                     << m_dipole.z << ")" << "  = " << m_dipole.norm() << "Debye";
         }

      private:
         qglviewer::Vec m_dipole;
   };

} } // end namespace IQmol::Data
