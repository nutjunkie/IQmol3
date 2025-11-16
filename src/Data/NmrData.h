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

#include "Data/DataList.h"
#include "Data/NmrReference.h"
#include "Math/Matrix.h"


namespace IQmol {
namespace Data {

   class Nmr: public Base {

      public:
         Type::ID typeID() const { return Type::Nmr; }

         void setAtomLabels(QStringList const& labels) {
            m_atomLabels = labels;
         }

         QList<QString> const& atomLabels() const { 
            return m_atomLabels; 
         }

         void setShieldings(QList<double> const& shieldings) {
            m_shieldings = shieldings;
         }

         QList<double> const& shieldings() const {
            return m_shieldings; 
         }

         void setCouplings(Matrix const& couplings) {
            m_couplings = couplings;
         }

         Matrix const& couplings() const { 
            return m_couplings; 
         }

         void setReference(NmrReference const& reference) {
            m_reference = reference;
         }

         NmrReference const& reference() const { 
            return m_reference; 
         }

         void setShifts(QList<double> const& shifts) {
            m_shifts = shifts;
         }

         QList<double> const& shifts() const { 
            return m_shifts; 
         }

         QList<double> shifts(QString const& isotope) {
            return shifts(isotope, m_reference);
         }

         QList<double> shifts(QString const& isotope, NmrReference const&) const;


         bool haveCouplings();
         bool haveReference();

         void setMethod(QString const& method) { m_method = method; }
         QString method() const { return m_method; }

         void dump() const;

      private:
         QList<QString> m_atomLabels;
         QList<double>  m_shieldings;
         NmrReference   m_reference;
         Matrix         m_couplings;
         QList<double>  m_shifts;
         QString        m_method;
   };

} } // end namespace IQmol::Data
