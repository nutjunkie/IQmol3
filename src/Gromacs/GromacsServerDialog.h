#ifndef IQMOL_GROMACSSERVERDIALOG_H
#define IQMOL_GROMACSSERVERDIALOG_H
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
 
#include <QDialog>
#include "Gromacs/ui_GromacsServerDialog.h"


namespace IQmol {

namespace Gmx {

   class GromacsServerDialog: public QDialog {

      Q_OBJECT

      public:
         GromacsServerDialog(QWidget* parent);

         QString getAddress() const;
      
      private:
         Ui::GromacsServerDialog  m_dialog;
   };

} } // end namespace IQmol::Gmx

#endif
