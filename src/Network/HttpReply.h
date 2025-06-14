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

#include "Reply.h"
#include "HttpConnection.h"
#include <QNetworkReply>
#include <QStringList>
#include <QTimer>


class QFile;
class QHttpMultiPart;

typedef QMap<QString, QString> QStringMap;

namespace IQmol {
namespace Network {

   class HttpReply : public Reply {

      Q_OBJECT

      public:
         HttpReply(HttpConnection*);
         virtual ~HttpReply();

         QString headerValue(QString const& headerName);
         void setHeader(QString const& header, QString const& value);

      public Q_SLOTS:
         void interrupt();

      Q_SIGNALS:
         void interrupted();

      protected:
         HttpConnection* m_connection;
         unsigned m_timeout;
         QNetworkReply* m_networkReply;  // We take ownership of this
         QStringMap m_headers;
         QTimer m_timer;
         bool m_https;
         QUrl m_url;

         // This takes care of all the http:// crap
         void setUrl(QString const& path = QString()); 
 
      protected Q_SLOTS:
         void readToString();
         void finishedSlot();
         void errorSlot(QNetworkReply::NetworkError);
         void dumpHeader();

      private Q_SLOTS:
         void timeout();

      private:
         QString headerAsString();
   };


   class HttpGet : public HttpReply {

      Q_OBJECT

      friend class HttpGetFiles;
      friend class HttpConnection;

      public:
         HttpGet(HttpConnection*, QString const& sourcePath);
         HttpGet(HttpConnection*, QString const& sourcePath, QString const& destinationPath);

      protected Q_SLOTS:
         void run();

      private Q_SLOTS:
         void readToFile();
         void closeFile();

      private:
         QFile* m_file;
   };


   class HttpGetFiles : public HttpReply {

      Q_OBJECT

      public:
         HttpGetFiles(HttpConnection*, QStringList const& fileList, 
            QString const& destinationPath);

      protected Q_SLOTS:
         void run();

      private Q_SLOTS:
         void replyFinished();

      private:
         QStringList m_fileList;
         QString m_destinationPath;
         QList<HttpGet*> m_replies;
         int  m_totalReplies;
         bool m_allOk;
   };


   class HttpPost : public HttpReply {

      Q_OBJECT

      friend class HttpConnection;

      public:
         HttpPost(HttpConnection*, QString const& path, QString const& postData);

      protected Q_SLOTS:
         void run();

      private:
         QString m_postData;
         
   };

   class HttpJsonPost : public HttpReply {

      Q_OBJECT

      friend class HttpConnection;

      public:
         HttpJsonPost(HttpConnection*, QString const& path, QHttpMultiPart* postData);

      protected Q_SLOTS:
         void run();

      private:
         QHttpMultiPart* m_postData;
         
   };

} } // end namespace IQmol::Network
