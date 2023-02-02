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
#include "Util/QMsgBox.h"
#include "Util/QsLog.h"
#include "Util/Preferences.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QDebug>


namespace IQmol {
namespace Gmx { 

QString toString(BoxType type)
{
   QString s;
   switch (type) {
       case Cubic:
          s = "cubic";
          break;
       case TruncatedOctahedron:
          s = "truncatedOctahetron";
          break;
       case HexagonalPrism:
          s = "hexagonalPrism";
          break;
       case RhombicDodecahedron:
          s = "rhombicDodecahedron";
          break;
   }
   return s;
}


/*
host configurable:

POST
header:
jobid empty is new job
jobname

host:/editconf 
host:/solvate
*/


GromacsDialog::GromacsDialog(QWidget* parent) : QDialog(parent), m_networkReply(0)
{
   m_dialog.setupUi(this);
   m_dialog.boundary->setSuffix(" \u212B");
   m_networkAccessManager = new QNetworkAccessManager(this);
}


GromacsDialog::~GromacsDialog()
{
   if (m_networkAccessManager) delete m_networkAccessManager;
}


void GromacsDialog::enableRequestWidgets(bool tf)
{
   m_dialog.generateBoxButton->setEnabled(tf);
}


void GromacsDialog::readToString()
{
   if (!m_networkReply) return;
   qint64 size(m_networkReply->bytesAvailable());
   QString s = m_networkReply->read(size);
   qDebug() << "Reading" << size << " bytes to string" << s;
} 



void GromacsDialog::on_generateBoxButton_clicked(bool)
{
   enableRequestWidgets(false);    
   if (m_networkReply) {
      QString msg("Request already in progress");
      QMsgBox::warning(this, "IQmol", msg);
      return;
   }

   qDebug() << "Generating Box";
   QString url(Preferences::GromacsServerAddress());
   url += "/editconf";
   qDebug() << "URL set to " << url;

   QNetworkRequest request;
   request.setUrl(url);

   // Set header
   request.setRawHeader("jobId", "");

   QJsonObject payload(boxRequestPayload());
   QJsonDocument json;
   json.setObject(payload);
   qDebug() << "Request body:" << json.toJson();

   m_networkReply = m_networkAccessManager->post(request,json.toJson());

   connect(m_networkReply, SIGNAL(finished()),  this, SLOT(boxRequestFinished()) );
   connect(m_networkReply, SIGNAL(readyRead()), this, SLOT(readToString()) );
}


QJsonObject GromacsDialog::boxRequestPayload()
{
   QJsonObject json;

   // Box type
   BoxType bt(Cubic);
   if (m_dialog.truncatedOctahedronButton->isChecked()) bt = TruncatedOctahedron;
   if (m_dialog.hexagonalPrismButton->isChecked())      bt = HexagonalPrism;
   if (m_dialog.rhombicDodecahedronButton->isChecked()) bt = RhombicDodecahedron;
   json.insert("boxType", toString(bt));

   // Centered
   if (m_dialog.centerButton->isChecked()) {
      json.insert("centered", "true");
   }else {
      json.insert("centered", "false");
   }

   // Boundary
   double boundary(m_dialog.boundary->value());
   json.insert("boundary", boundary);

   return json;
}


void GromacsDialog::boxRequestFinished()
{
   enableRequestWidgets(true);    
   QString msg("Request finished");
   QMsgBox::information(this, "IQmol", msg);

   // do stuff

   delete m_networkReply;
   m_networkReply = 0;
}

} } // end namespace IQmol::Gmx
