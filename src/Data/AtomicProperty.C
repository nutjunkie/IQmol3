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

#include "AtomicProperty.h"
#include <openbabel/elements.h>
#include <vector>
#include <QDebug>


namespace IQmol {
namespace Data {

//  ---------- AtomicSymbol ----------
void AtomicSymbol::setDefault(int const Z)
{
   if (Z > 0) m_value = QString(OpenBabel::OBElements::GetSymbol(Z));
}



//  ---------- ScalarProperty ----------
void ScalarProperty::dump() const
{
   qDebug() << m_value;
}



//  ---------- Mass ----------
void Mass::setDefault(int const Z)
{
   if (Z > 0) m_value = OpenBabel::OBElements::GetMass(Z);
}



//  ---------- VdwRadius ----------
void VdwRadius::setDefault(int const Z)
{
   if (Z > 0) m_value = OpenBabel::OBElements::GetVdwRad(Z);
}



//  ---------- AtomColor ----------
void AtomColor::setDefault(int const Z)
{
   if (Z > 0) {
      double r, g, b;
      OpenBabel::OBElements::GetRGB(Z, m_color, m_color+1, m_color+2);
      m_color[3] = 1.0;
   }
}
  

void AtomColor::set(double const red, double const green, double const blue)
{
   m_color[0] = red;
   m_color[1] = blue;
   m_color[2] = green;
}


QColor AtomColor::get() const
{
   QColor color;
   color.setRgbF(m_color[0], m_color[1], m_color[2]);
   return color;
}


void AtomColor::dump() const
{
   qDebug() << "Red ="   << QString::number(m_color[0], 'f', 3)
            << "Green =" << QString::number(m_color[1], 'f', 3)
            << "Blue ="  << QString::number(m_color[2], 'f', 3);
}

} } // end namespace IQmol::Data
