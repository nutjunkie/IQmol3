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

#include "Group.h"


namespace IQmol {
namespace Data {

   // Data class representing a large system
   class MacroMolecule: public Base {

      public:
         MacroMolecule(QString const& label) : m_charge(0), m_label(label) { }

         Type::ID typeID() const { return Type::MacroMolecule; }

         QString const& label() const { return m_label; }

      
         QList<Data::Group*> const& groups() const { return m_groups; }

         void append(Data::Group* group) 
         {
            m_charge += group->charge();
            m_groups.append(group);
         }
         
         void serialize(InputArchive& ar, unsigned const version = 0)  { }
         void serialize(OutputArchive& ar, unsigned const version = 0)  { }

         void dump() const 
         {  
             qDebug() <<  m_label;
             for (auto group : m_groups) group->dump();
         }

      private:
         int m_charge;
         QString m_label;
         QList<Data::Group*> m_groups;
   };

} } // end namespace IQmol::Data
