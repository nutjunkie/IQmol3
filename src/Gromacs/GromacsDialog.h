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
 
#include "Gromacs/ui_GromacsDialog.h"
#include <QDialog>
#include <QJsonObject>
#include <QJsonArray>


class QNetworkAccessManager;
class QNetworkReply;

namespace IQmol {
namespace Gmx {

   enum BoxType { Cubic = 0, 
                  TruncatedOctahedron, 
                  HexagonalPrism,
                  RhombicDodecahedron };

   class GromacsDialog: public QDialog {

      Q_OBJECT

      public:
         GromacsDialog(QWidget* parent);
         ~GromacsDialog();

      private Q_SLOTS:
         void on_generateBoxButton_clicked(bool);
         //QJsonArray readToJson();
         //void stageCalculation();   
         void boxRequestFinished();
         QString readToString();
      
      private:
         QJsonObject boxRequestPayload();
 
         void enableRequestWidgets(bool);
         Ui::GromacsDialog  m_dialog;
         QNetworkAccessManager* m_networkAccessManager;
         QNetworkReply* m_networkReply; 
   };

} } // end namespace IQmol::Gmx
