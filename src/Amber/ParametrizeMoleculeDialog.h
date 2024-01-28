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

#include "Amber/ui_ParametrizeMoleculeDialog.h"

#include <QProcess>

namespace IQmol {

   namespace Layer {
      class Molecule;
   }

   /// Dialog allowing the user to parametrize a molecule
   class ParametrizeMoleculeDialog : public QDialog {

      Q_OBJECT

      friend class Layer::Molecule;

      public:
         ParametrizeMoleculeDialog(QWidget* parent, Layer::Molecule*);

      protected:
         QString forceField;

      Q_SIGNALS:
         void killed();
         void finished();

      private Q_SLOTS:
         void on_chargeSpin_valueChanged(int);
         void on_multiplicitySpin_valueChanged(int);
         void run();
         void runAntechamber();
         void antechamberFinished(int, QProcess::ExitStatus);
         void runParmchk2();
         void parmchk2Finished(int, QProcess::ExitStatus);

      private:
         Layer::Molecule* m_molecule;
         Ui::ParametrizeMoleculeDialog m_dialog;
   };

} // end namespace IQmol
