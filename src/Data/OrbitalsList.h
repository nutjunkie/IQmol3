#ifndef IQMOL_DATA_ORBITALSLIST_H
#define IQMOL_DATA_ORBITALSLIST_H
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

#include "Orbitals.h"


namespace IQmol {
namespace Data {

   class OrbitalsList : public Data::List<Data::Orbitals> {

      public:
         Type::ID typeID() const { return Type::OrbitalsList; }

         OrbitalsList() : m_defaultIndex(0) { }

         /// Sets which set of data should be considered the default in the
         /// list.  An index of -1 corresponds to the final set.
         void setDefaultIndex(int index);
         unsigned defaultIndex() const { return m_defaultIndex; }

         virtual void serialize(InputArchive& ar, unsigned int const version = 0) 
         {
            serializeList(ar, version);
            ar & m_defaultIndex;
         }

         virtual void serialize(OutputArchive& ar, unsigned int const version = 0) 
         {
            serializeList(ar, version);
            ar & m_defaultIndex;
         }

         void dump() const;

      private:
         unsigned m_defaultIndex; 
   };

} } // end namespace IQmol::Data

#endif
