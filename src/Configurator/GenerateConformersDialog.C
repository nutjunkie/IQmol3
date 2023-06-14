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

#include "GenerateConformersDialog.h"
#include "Layer/MoleculeLayer.h"
#include <QProgressDialog>


using namespace qglviewer;


namespace IQmol {

GenerateConformersDialog::GenerateConformersDialog(QWidget* parent, 
   Layer::Molecule* molecule) : QDialog(parent), m_molecule(molecule)
{
   m_dialog.setupUi(this);
}


void GenerateConformersDialog::accept()
{
   // gather data
   sortEnergy = m_dialog.sortEnergy->isChecked();
   noBreakyBonds = m_dialog.noBreakyBonds->isChecked();
   numberOfConformers = m_dialog.numberOfConformers->value();
   numberOfChildren = m_dialog.numberOfChildren->value();
   mutability = m_dialog.mutability->value();
   maximumGenerations = m_dialog.maximumGenerations->value();
   
   QDialog::accept();
}

} // end namespace IQmol
