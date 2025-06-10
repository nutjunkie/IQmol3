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

#include "GromacsDialog.h"


#include "GromacsConfigDialog.h"
#include "Util/Preferences.h"



namespace IQmol {
namespace Gmx { 


GromacsConfigDialog::GromacsConfigDialog(QWidget* parent) : QDialog(parent)
{
   m_dialog.setupUi(this);
   m_dialog.topology->setText(Preferences::GromacsTopologyFile());
   m_dialog.positions->setText(Preferences::GromacsPositionsFile());

}

QString GromacsConfigDialog::getTopology() const
{
   return m_dialog.topology->text();
}

QString GromacsConfigDialog::getPositions() const
{
   return m_dialog.positions->text();
}

} } // end namespace IQmol::Gmx
