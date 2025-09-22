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

#include "DataList.h"
#include "QGLViewer/vec.h"


namespace IQmol {
namespace Data {

   /// Data structure representing a charge. 
   class PointCharge : public Base {

      public:
         PointCharge(double const charge = 0.0, 
           qglviewer::Vec const& position = qglviewer::Vec()) 
           : m_charge(charge), m_position(position) { }

         Type::ID typeID() const { return Type::PointCharge; }

         double value() const { return m_charge; }
         qglviewer::Vec const& position() const { return m_position; }

         void dump() const;

      private:
         double m_charge;
         qglviewer::Vec m_position;
   };


   class PointChargeList : public List<PointCharge> { 
      public:
          Type::ID typeID() const { return Type::PointChargeList; }
   };

} } // end namespace IQmol::Data
