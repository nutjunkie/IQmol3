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

#include "AwsReply.h"
#include "AwsConnection.h"
#include "QsLog.h"
#include "Zip.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QJsonObject>
#include <QJsonParseError>


// ********* Debug only ********* 
QString networkRequestToCurl(const QNetworkRequest &request,
                             const QByteArray &body = {},
                             const QString &method = "POST")
{
    QString curlCmd = "curl";

    if (method.toUpper() != "GET") curlCmd += QString(" -X %1").arg(method.toUpper());

    curlCmd += QString(" \"%1\"").arg(request.url().toString());

    // Add headers
    const auto headers = request.rawHeaderList();
    for (const QByteArray &headerName : headers) {
        QByteArray headerValue = request.rawHeader(headerName);
        curlCmd += QString(" -H \"%1: %2\"")
                       .arg(QString::fromUtf8(headerName),
                            QString::fromUtf8(headerValue));
    }

    if (!body.isEmpty()) {
        // Escape quotes for safe shell usage
        QString escapedBody = QString::fromUtf8(body)
                                  .replace("\"", "\\\"")
                                  .replace("\n", "\\n");
        curlCmd += QString(" -d \"%1\"").arg(escapedBody);
    }

    return curlCmd;
}


void networkReplyDump(const QNetworkReply& reply, QString const& body)
{
 // Status Code
    QVariant statusCode = reply.attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (statusCode.isValid())
        qDebug() << "Status Code:" << statusCode.toInt();

    // Reason Phrase (if available)
    QVariant reason = reply.attribute(QNetworkRequest::HttpReasonPhraseAttribute);
    if (reason.isValid())
        qDebug() << "Reason Phrase:" << reason.toString();

    // Headers
    const QList<QNetworkReply::RawHeaderPair> headers = reply.rawHeaderPairs();
    qDebug() << "Headers:";
    for (const auto &header : headers) {
        qDebug() << "  " << header.first << ":" << header.second;
    }

    // Body
    qDebug() << body;
}


namespace IQmol {
namespace Network {

AwsReply::AwsReply(AwsConnection* connection) 
 : m_connection(connection), 
   m_networkReply(0), 
   m_https(true)
{ 
   m_timeout = m_connection->timeout();
   m_timer.setInterval(m_timeout);

   connect(this, SIGNAL(finished()), &m_timer, SLOT(stop()));
   connect(&m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
}


AwsReply::~AwsReply()
{
   // The QNetworkAcessManager takes care of this.
   // if (m_networkReply) m_networkReply->deleteLater(); 
}


void AwsReply::finishedSlot()
{
   if (m_networkReply->error() != QNetworkReply::NoError) {
      m_message = QString("Request failed: ") + m_networkReply->errorString();
      m_status = Error;
      finished();
      return;
   }   
/*
*/

   if (m_status != Error) m_status = m_interrupt ? Interrupted : Finished;

   // Attempt to parse the return as JSON, but if this fails we go to text
   QByteArray data = m_message.toUtf8();

 networkReplyDump(*m_networkReply, data);

   QJsonParseError parseError;
   QJsonDocument document = QJsonDocument::fromJson(data, &parseError); 

   if (parseError.error == QJsonParseError::NoError) {
      QJsonObject object = document.object();

      // Check for expired token (how??)
      QString type = object.value("__type").toString();
      if (type == "NotAuthorizedException" ||
          type == "InvalidPasswordException" ) { 
          m_message = object.value("message").toString();
          m_connection->setStatus(Opened);
      } else if (m_status == Error) {
          m_message = object.value("message").toString();
      }

   }else {
      if (m_message.contains("Submitted job id")) { 
        // Do we need to do anything?
      }else {
        qDebug() << "Raw reply message:" << m_message;
      }
   }

   if (m_message.isEmpty()) m_message = "Q-Cloud server unavailable";
   finished();
}



void AwsReply::errorSlot(QNetworkReply::NetworkError /*error*/)
{
   // networkReplyDump(*m_networkReply, m_message);

   // Handle expired token here
   QVariant statusCode = m_networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
   if (statusCode.toInt() == 401) {
      // de-authorise the status
      m_connection->setStatus(Opened);
   }else {
      m_message += "\n" + m_networkReply->errorString();
   }
   m_status  = Error;
}


void AwsReply::readToString()
{
   qint64 size(m_networkReply->bytesAvailable());
   m_message += m_networkReply->read(size);
   qDebug() << "Reading" << size << " bytes to message:" << m_message;
}


void AwsReply::interrupt()
{
   m_interrupt = true;
   QLOG_TRACE() << "AwsReply interrupted" << m_connection->hostname();
   m_networkReply->abort();
   m_status = Interrupted;
   interrupted();
   finished();
}


void AwsReply::timeout()
{
   QLOG_TRACE() << "AwsReply timeout" << m_connection->hostname();
   m_networkReply->abort();
   m_status = TimedOut;
   finished();
}


void AwsReply::setUrl(QString const& path)
{
   int port(m_connection->port());

   QString url(path);
   url.prepend("/");
   if (port != 80) url.prepend(":" + QString::number(port));
   url.prepend(m_connection->hostname());
   url.replace("//", "/");
   url.prepend(m_https ? "https://" : "http://");

   m_url.setUrl(url);
   QLOG_DEBUG() << "Setting URL to" <<  m_url;
}


void AwsReply::setHeader(QString const& header, QString const& value)
{
   m_headers[header] = value;
}


QString AwsReply::headerValue(QString const& headerName)
{
   QByteArray data(headerName.toLatin1());
   QString header;
   
   if (m_networkReply->hasRawHeader(data)) {
       header = QString(m_networkReply->rawHeader(data));
   }

   return header;
}



// --------- AwsGet ---------

AwsGet::AwsGet(AwsConnection* connection,  QString const& sourcePath) 
 : AwsReply(connection), m_file(0)
{
   setUrl(sourcePath);
}


AwsGet::AwsGet(AwsConnection* connection,  QString const& sourcePath, 
   QString const& destinationPath) : AwsReply(connection), m_file(0)
{
   setUrl(sourcePath);

   m_message = destinationPath;
   m_file = new QFile(destinationPath+".tgz");

   // We assume we have already okay'd this with the user
   if (m_file->exists() && m_file->isWritable()) m_file->remove();

   if (!m_file->open(QIODevice::WriteOnly)) {
      m_message = "Failed to open file for write: " + destinationPath;
      m_status = Error;
      finished();
   }
}


void AwsGet::closeFile()
{
    QByteArray bin = QByteArray::fromBase64(m_base64);
    m_file->write(bin);
    m_file->flush();
    m_file->close();

    QFileInfo fileInfo(m_file->fileName());
    delete m_file;

    QByteArray source = fileInfo.filePath().toUtf8();
    QByteArray destination = fileInfo.path().toUtf8();
    Util::extract_tgz(source.data(),destination.data()); 

    QDir dir(fileInfo.path() +"/" + fileInfo.baseName());

    if (!dir.exists()) {
       qDebug() << "Failed to find directory for AwsGet" << dir.path();
       return;
    }

    QDir parentDir(dir.absolutePath() + "/..");
    parentDir.makeAbsolute();

    QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    for (const QFileInfo &fileInfo : fileList) {
        QString oldFilePath = fileInfo.absoluteFilePath();

        if (oldFilePath.contains("slurm-")) {
           QFile::remove(oldFilePath);
        }else {

           QString newFilePath = parentDir.absoluteFilePath(fileInfo.fileName());

           if (!QFile::rename(oldFilePath, newFilePath)) {
               qWarning() << "Failed to move:" << oldFilePath << "to" <<newFilePath;
           }
        }
    }

    QFile::remove(QString(source));
    if (dir.rmdir(dir.absolutePath())) {
       qDebug() << "Failed to remove directory" << dir.absolutePath();
    }
}


void AwsGet::run()
{
   if (!m_connection->m_networkAccessManager) {
      m_message = "Invalid Network Access Manager";
      m_status = Error;
      finished();
      return;
   }

   m_status = Running;
   QNetworkRequest request;
   request.setUrl(m_url);
   QLOG_DEBUG() << "Retrieving:" << m_url;

   //qDebug() << networkRequestToCurl(request);

   QStringMap::iterator iter;
   for (iter = m_headers.begin(); iter != m_headers.end(); ++iter) {
       request.setRawHeader(iter.key().toLatin1(), iter.value().toLatin1());
   }

   m_networkReply = m_connection->m_networkAccessManager->get(request);

   connect(m_networkReply, SIGNAL(readyRead()), &m_timer, SLOT(start()));

   if (m_file) {
      connect(m_networkReply, SIGNAL(readyRead()), this, SLOT(readToFile()));
      connect(m_networkReply, SIGNAL(finished()),  this, SLOT(closeFile()) );
   }else {
      connect(m_networkReply, SIGNAL(readyRead()), this, SLOT(readToString()));
   }

   connect(m_networkReply, SIGNAL(finished()),  this, SLOT(finishedSlot()) );
   connect(m_networkReply, SIGNAL(error(QNetworkReply::NetworkError)),
          this, SLOT(errorSlot(QNetworkReply::NetworkError)));

   m_timer.start();
}


void AwsGet::readToFile()
{
   qint64 size(m_networkReply->bytesAvailable());
   qDebug() << "Reading " << size << " bytes";
   copyProgress();
   m_base64.append(m_networkReply->read(size));
}


// --------- AwsGetFiles---------

AwsGetFiles::AwsGetFiles(AwsConnection* connection, QStringList const& fileList, 
   QString const& destinationPath) : AwsReply(connection), m_fileList(fileList), 
   m_destinationPath(destinationPath), m_totalReplies(0), m_allOk(true)
{
}


void AwsGetFiles::run()
{
   QRegularExpression rx("job_id=(.*)");
   m_status = Running;

   //qDebug() << m_headers;

   QString auth = m_headers.value("Authorization");

   QStringList::iterator iter;
   for (iter = m_fileList.begin(); iter != m_fileList.end(); ++iter) {
       QString source(*iter);
       QRegularExpressionMatch match(rx.match(source));

       if (match.hasMatch()) {
          QString destination(m_destinationPath);
          destination += "/" + match.captured(1);
          QLOG_DEBUG() << "Downloading file:" << source;

          AwsGet* reply(new AwsGet(m_connection, source, destination));
          reply->setHeader("Authorization", auth.toUtf8());

          m_replies.append(reply);
          connect(this, SIGNAL(interrupted()), this, SLOT(interrupt()));
          connect(reply, SIGNAL(finished()), this, SLOT(replyFinished()));
          //connect(reply, SIGNAL(copyProgress()), this, SIGNAL(copyProgress()));
       }
   }

   QList<AwsGet*> replies(m_replies);
   m_totalReplies = replies.size();
   QList<AwsGet*>::iterator reply;
   for (reply = replies.begin(); reply != replies.end(); ++reply) {
       (*reply)->run();
   }
}


void AwsGetFiles::replyFinished()
{
   AwsGet* reply(qobject_cast<AwsGet*>(sender()));
   m_replies.removeAll(reply);
   double progress(m_totalReplies-m_replies.size());
   if (m_totalReplies > 0) copyProgress(progress/m_totalReplies);
   m_allOk = m_allOk && reply->status() == Finished;

   if (m_replies.isEmpty()) {
      if (m_allOk) {
         m_status = m_interrupt ? Interrupted : Finished;
      }else {
         m_status = Error;
      }
      finished();
   }
}


// --------- AwsPost ---------

AwsPost::AwsPost(AwsConnection* connection, QString const& path,  
   QByteArray const& postData) : AwsReply(connection), m_postData(postData)
{
   setUrl(path);
}



void AwsPost::run()
{
   if (!m_connection->m_networkAccessManager) {
      m_message = "Invalid Network Access Manager";
      m_status = Error;
      finished();
      return;
   }

   m_status = Running;
   QNetworkRequest request;

   request.setUrl(m_url);
   request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

   QStringMap::iterator it;
   for (it = m_headers.begin(); it != m_headers.end(); ++it) {
       request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
   }

#if 0
qDebug() << "POST data:" << m_postData;
QString curl = networkRequestToCurl(request, m_postData);
qDebug() << "===========================================";
qDebug().noquote() << curl;
qDebug() << "===========================================";
qDebug().noquote() << QString::fromUtf8(m_postData); 
#endif

   m_networkReply = m_connection->m_networkAccessManager->post(request, m_postData);

   connect(m_networkReply, SIGNAL(readyRead()), &m_timer, SLOT(start()));
   connect(m_networkReply, SIGNAL(readyRead()), this, SLOT(readToString()));
   connect(m_networkReply, SIGNAL(finished()),  this, SLOT(finishedSlot()) );
   connect(m_networkReply, SIGNAL(error(QNetworkReply::NetworkError)),
          this, SLOT(errorSlot(QNetworkReply::NetworkError)));

   m_timer.start();
}


} } // end namespace IQmol::Network
