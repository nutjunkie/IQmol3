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


#include "GromacsConfigDialog.h"
#include "GromacsDialog.h"
#include "Util/QMsgBox.h"
#include "Util/QsLog.h"
#include "Util/Preferences.h"

#include <QComboBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QDebug>

#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QHttpMultiPart>
#include <QFile>
#include <QTextStream>

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




   //Add path to opened molecule default should be pdb file
}


GromacsDialog::~GromacsDialog()
{
   if (m_networkAccessManager) delete m_networkAccessManager;
   
}


void GromacsDialog::enableRequestWidgets(bool tf)
{
   m_dialog.generateBoxButton->setEnabled(tf);
}

/*
void GromacsDialog::stageCalculation()
{
   qDebug() << "Generating ID";
   QString url(Preferences::GromacsServerAddress());
   url += "/stagecalculation";
   qDebug() << "URL set to " << url;
   
   QNetworkRequest request;
   request.setUrl(url);
   QJsonDocument json;
   QNetworkReply *reply = m_networkAccessManager->get(request);
   //QString strReply = (QString)reply->readAll();
   
   


  
   QJsonArray json_array = readToJson();
   qDebug() << "size of array is " << json_array.size();
   foreach (const QJsonValue &value, json_array) {
      QJsonObject json_obj = value.toObject();
      qDebug() << "id is " << json_obj["id"].toString();
   //QString id2 =  QString(m_networkReply->rawHeaderList().size());
   connect(reply, SIGNAL(readyRead()), this, SLOT(readToJson()) );
   }


}*/

/*QJsonArray GromacsDialog::readToJson()
{
   QJsonDocument jsonResponse;
   if (!reply) return(jsonResponse);
   QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->readAll());
   QJsonArray json_array = jsonResponse.array();
   return(json_array);
}
*/
QString GromacsDialog::readToString()
{
   QString s;
   if (!m_networkReply) return(s);
   qint64 size(m_networkReply->bytesAvailable());
   s = m_networkReply->read(size);
   qDebug() << "Reading" << size << " bytes to string";
   QString qPath("1AKI_newbox.gro");
   QFile qFile(qPath);
   if (qFile.open(QIODevice::WriteOnly)) {
      QTextStream out(&qFile);
      qFile.close();
   }
   return(s);
   
}

void GromacsDialog::on_solvateButton_clicked(bool)
{
   enableRequestWidgets(false);    
   if (m_networkReply) {
      QString msg("Request already in progress");
      QMsgBox::warning(this, "IQmol", msg);
      return;
   }
   qDebug() << "Solvating";
   QString url(Preferences::GromacsServerAddress());
   url += "/solvate";
   qDebug() << "URL set to " << url;


   QNetworkRequest request;
   request.setUrl(url);
   QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
   QHttpPart jsonPart;
   jsonPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"json\""));
   QJsonObject payload(solvateRequestPayload());
   QJsonDocument json;
   json.setObject(payload);
   //qDebug() << "Request body:" << json.toJson();
   jsonPart.setBody(json.toJson());

   QHttpPart filePart;
   filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("file/gro"));
   filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\""));
   QFile *file = new QFile("1AKI_newbox.gro");
   file->open(QIODevice::ReadOnly);
   filePart.setBodyDevice(file);
   file->setParent(multiPart); 
   
   QHttpPart topologyPart;
   topologyPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("file/top"));
   topologyPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"top\""));
   QFile *file2 = new QFile(Preferences::GromacsTopologyFile());
   file2->open(QIODevice::ReadOnly);
   topologyPart.setBodyDevice(file2);
   file2->setParent(multiPart); 




   multiPart->append(jsonPart);
   multiPart->append(filePart);
   multiPart->append(topologyPart);
   m_networkReply = m_networkAccessManager->post(request,multiPart);
   qDebug() << "Request pending" ;
   connect(m_networkReply, SIGNAL(finished()),  this, SLOT(solvateRequestFinished()) );
   connect(m_networkReply, SIGNAL(readyRead()), this, SLOT(readToString()) );

}

void GromacsDialog::on_generateBoxButton_clicked(bool)
{
   if (m_networkReply) {
      QString msg("Request already in progress");
      QMsgBox::warning(this, "IQmol", msg);
      return;
   }

   //stageCalculation();
   qDebug() << "Generating Box";
   QString url(Preferences::GromacsServerAddress());
   url = url + "/editconf";
   qDebug() << "URL in gromacs set test to " << url;
   QString gromacsJobType = "editconf";
   QString jobType ="gromacs";
   QNetworkRequest request;
   request.setUrl(url);
   QJsonObject payload(boxRequestPayload());
   //json.setObject(payload);
   m_gromacsJobInfo.set("gromacsJobType",gromacsJobType);
   m_gromacsJobInfo.set("JsonPayload",payload);
   m_gromacsJobInfo.set("Url",url);
   m_gromacsJobInfo.set("Header",QVariant("form-data; name=\"json\""));
   m_gromacsJobInfo.set("ServerName","Gromacs");
   m_gromacsJobInfo.set("jobType",jobType);
   submitGromacsJobRequest(m_gromacsJobInfo);

   qDebug() << "Gromacs request submitted" ;
   //enableRequestWidgets(false);    
   //connect(m_networkReply, SIGNAL(finished()),  this, SLOT(boxRequestFinished()) );
   //connect(m_networkReply, SIGNAL(readyRead()), this, SLOT(readToString()) );
}


QJsonObject GromacsDialog::boxRequestPayload()
{
   QJsonObject json;
   json.insert("jobType","EditConf");

   // Box type
   BoxType bt(Cubic);
   if (m_dialog.truncatedOctahedronButton->isChecked()) bt = TruncatedOctahedron;
   if (m_dialog.hexagonalPrismButton->isChecked())      bt = HexagonalPrism;
   if (m_dialog.rhombicDodecahedronButton->isChecked()) bt = RhombicDodecahedron;
   json.insert("boxType", toString(bt));

   json.insert("-f","1AKI_processed.gro");
   json.insert("-o","1AKI_newbox.gro");
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



QJsonObject GromacsDialog::solvateRequestPayload()
{
   QJsonObject json;
   QString solvateType(m_dialog.solvationtype->currentText());
   json.insert("solvatetype", solvateType);
   QString topology = Preferences::GromacsTopologyFile();
   json.insert("topology",topology);
   json.insert("-cp","1AKI_newbox.gro");
   json.insert("-o","1AKI_solv.gro");




   return json;
}


void GromacsDialog::solvateRequestFinished()
{
   enableRequestWidgets(true);
   if(m_networkReply->error())
    {
      qDebug() << "ERROR!";
      qDebug() << m_networkReply->errorString();
      QString msg(m_networkReply->errorString());
      QMsgBox::information(this, "IQmol", msg);

    }else{  
      QString msg("Request finished");
      QMsgBox::information(this, "IQmol", msg);
      QString outputdata = readToString();
      // do stuff
    }

   delete m_networkReply;
   m_networkReply = 0;
}

void GromacsDialog::boxRequestFinished()
{
   enableRequestWidgets(true);
   if(m_networkReply->error())
    {
      qDebug() << "ERROR!";
      qDebug() << m_networkReply->errorString();
      QString msg(m_networkReply->errorString());
      QMsgBox::information(this, "IQmol", msg);

    }else{  
      QString msg("Request finished");
      QMsgBox::information(this, "IQmol", msg);
      QString outputdata = readToString();
  }

      // do stuff

   delete m_networkReply;
   m_networkReply = 0;
}



} } // end namespace IQmol::Gmx
