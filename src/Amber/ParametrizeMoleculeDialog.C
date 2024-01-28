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

#include "ParametrizeMoleculeDialog.h"
#include "Layer/MoleculeLayer.h"
#include "Util/QMsgBox.h"
#include "Util/Preferences.h"
#include "Amber/AmberConfigDialog.h"

namespace IQmol {

ParametrizeMoleculeDialog::ParametrizeMoleculeDialog(QWidget* parent, 
   Layer::Molecule* molecule) : QDialog(parent), m_molecule(molecule)
{
   m_dialog.setupUi(this);
   m_dialog.chargeSpin->setValue(m_molecule->totalCharge());
   m_dialog.multiplicitySpin->setValue(m_molecule->multiplicity());

   // set up the run button
   QPushButton* runButton = m_dialog.buttonBox->button(QDialogButtonBox::Apply);
   runButton->setText(tr("Run"));
   QPushButton* killButton = m_dialog.buttonBox->addButton("Cancel", QDialogButtonBox::ActionRole);
   killButton->setEnabled(false);

   connect(runButton, &QPushButton::clicked, this, &ParametrizeMoleculeDialog::request);
   connect(runButton, &QPushButton::clicked, runButton, [runButton]() { runButton->setEnabled(false); });
   connect(runButton, &QPushButton::clicked, killButton, [killButton]() { killButton->setEnabled(true); });

   connect(killButton, &QPushButton::clicked, this, &ParametrizeMoleculeDialog::killed);
   connect(killButton, &QPushButton::clicked, runButton, [runButton]() { runButton->setEnabled(true); });
   connect(killButton, &QPushButton::clicked, killButton, [killButton]() { killButton->setEnabled(false); });

   connect(this, &ParametrizeMoleculeDialog::rejected, this, &ParametrizeMoleculeDialog::killed);

   connect(this, &ParametrizeMoleculeDialog::finished, runButton, [runButton]() { runButton->setEnabled(true); });
   connect(this, &ParametrizeMoleculeDialog::finished, killButton, [killButton]() { killButton->setEnabled(false); });
}

void ParametrizeMoleculeDialog::on_chargeSpin_valueChanged(int value)
{
   m_molecule->chargeAvailable(value);
   m_dialog.multiplicitySpin->setValue(m_molecule->multiplicity());
}


void ParametrizeMoleculeDialog::on_multiplicitySpin_valueChanged(int value)
{
   m_molecule->multiplicityAvailable(value);
   m_dialog.chargeSpin->setValue(m_molecule->totalCharge());
}

void ParametrizeMoleculeDialog::request()
{
   // gather data
   forceField = m_dialog.forceField->currentText();

   // check if the Amber directory is set
   QString AmberDirectory = Preferences::AmberDirectory();
   if (AmberDirectory.isEmpty()) {
      QMessageBox::warning(this, tr("Amber directory not set"),
         tr("The Amber directory is not set. Please set it in the Edit Amber Config dialog."));
      Amber::AmberConfigDialog dialog(this);
      dialog.exec();
      if (dialog.result() == QDialog::Accepted) {
         Preferences::AmberDirectory(dialog.getDirectory());
      }
      return;
   }

   emit requested();
}

} // end namespace IQmol
