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

#include <QMap>
#include "DataList.h"


namespace IQmol {
namespace Data {

   class NmrReference : public Base {

      public:

         NmrReference(QString const& system = QString(), 
                      QString const& method = QString() ) : 
            m_system(system), m_method(method) { }

         Type::ID typeID() const { return Type::NmrReference; }

         void setSystem(QString const& system) { m_system = system; }
         void setMethod(QString const& method) { m_method = method; }

         QString const& system() const { return m_system; }
         QString const& method() const { return m_method; }

         bool contains(QString const& element) const { return m_shifts.contains(element); }
         void addElement(QString const& symbol, double const shfit, double const offset = 0.0);
         double shift(QString const& symbol) const;

         void dump() const;

      private:
         QString m_system;
         QString m_method;

         QMap<QString, double> m_shifts;
   };

   typedef Data::List<Data::NmrReference> NmrReferenceList;

} } // end namespace IQmol::Data
