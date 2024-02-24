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
 
#include "Amber/ui_AmberSystemBuilderDialog.h"

#include <QDialog>
#include <QProcess>

namespace IQmol {

   namespace Layer {
      class System;
      class Molecule;
   }

namespace Amber {

   class SystemBuilderDialog: public QDialog {

      Q_OBJECT

      public:
         SystemBuilderDialog(QWidget* parent, Layer::System*);

      Q_SIGNALS:
         void killed();
         void finished();

      private Q_SLOTS:
         void on_sourceAddButton_clicked();
         void on_sourceRemoveButton_clicked();
         void on_ion1ComboBox_currentIndexChanged(int);
         void findParameterFile(const QString&);
         void browseParameterFile();
         void updateTleapInput();
         void runTleap();
         void tleapFinished(int, QProcess::ExitStatus);

      private:
         void addMolecule(const Layer::Molecule*);

         Layer::System* m_system;
         Ui::SystemBuilderDialog m_dialog;
   };

} } // end namespace IQmol::Amber
