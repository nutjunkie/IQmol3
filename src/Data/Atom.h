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

#include "DataList.h"


namespace IQmol {
namespace Data {

   // Base class for QM and MM atoms.  Note that position information is
   // treated separately in the Geometry object.
   class Atom : public Base 
   {
      public:
         Atom(unsigned const Z = 0, QString const& label = QString()) 
          : m_atomicNumber(Z), m_label(label) { }

         Atom(QString const& symbol, QString const& label = QString());

         Type::ID typeID() const { return Type::Atom; }

         unsigned atomicNumber() const { return m_atomicNumber; }

         QString getLabel() { return m_label; }
 
         static unsigned atomicNumber(QString const&);

      protected:
         unsigned m_atomicNumber;
         QString  m_label;
   };


   class AtomList : public List<Atom> 
   {
      public:
         // Why is a fully qualified namespace required for Type with Qt >= 6.0
         IQmol::Data::Type::ID typeID() const { return IQmol::Data::Type::AtomList; }
   };

} } // end namespace IQmol::Data
