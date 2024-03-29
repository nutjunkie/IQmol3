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

#include "NmrReference.h"
#include <QDebug>


namespace IQmol {
namespace Data {


void NmrReference::addElement(QString const& atom, double const shift, double const offset)
{
   m_shifts[atom] = shift - offset;
}


double NmrReference::shift(QString const& isotope) const
{
   return contains(isotope) ? m_shifts.value(isotope) : 0.0;
}


void NmrReference::dump() const 
{
   qDebug() << "System: " << m_system << "Theory:" << m_method;
   QMap<QString, double>::const_iterator iter;
   for (iter = m_shifts.begin(); iter != m_shifts.end(); ++iter) {
       qDebug() << "  " << iter.key() << "  " << iter.value();
   } 
}

} } // end namespace IQmol::Data
