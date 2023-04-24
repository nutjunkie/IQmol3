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

#include "SystemLayer.h"

#include <QtDebug>


namespace IQmol {
namespace Layer {


System::System(QString const& label, QObject* parent) : Base(label, parent)
{
   setFlags( Qt::ItemIsEnabled | 
             Qt::ItemIsEditable |
             Qt::ItemIsSelectable | 
             Qt::ItemIsDropEnabled | 
             Qt::ItemIsUserCheckable );
   setCheckState(Qt::Checked);
}


System::~System()
{ 
   for (auto iter : m_componentList) delete iter;
}


void System::draw() const
{
   for (auto iter : m_componentList) iter->draw();
}


} } // end namespace IQmol::Layer
