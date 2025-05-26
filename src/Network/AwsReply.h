#ifndef IQMOL_NETWORK_AWSREPLY_H
#define IQMOL_NETWORK_AWSREPLY_H
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
#include <QNetworkReply>
#include <QStringList>
#include <QTimer>


class QNetworkReply;
class QFile;

typedef QMap<QString, QString> QStringMap;

namespace IQmol {
namespace Network {

   class AwsConnection;

   class AwsReply : public Reply 
   {
      Q_OBJECT

      public:
         AwsReply(AwsConnection*);
         virtual ~AwsReply();

         QString headerValue(QString const& headerName);
         void setHeader(QString const& header, QString const& value);

      public Q_SLOTS:
         void interrupt();

      Q_SIGNALS:
         void interrupted();

      protected:
         AwsConnection* m_connection;
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

      private Q_SLOTS:
         void timeout();
   };


   class AwsGet : public AwsReply 
   {

      Q_OBJECT

      friend class AwsGetFiles;

      public:
         AwsGet(AwsConnection*, QString const& sourcePath);
         AwsGet(AwsConnection*, QString const& sourcePath, QString const& destinationPath);

      protected Q_SLOTS:
         void run();

      private Q_SLOTS:
         void readToFile();
         void closeFile();

      private:
         QByteArray m_base64;
         QFile* m_file;
   };


   class AwsGetFiles : public AwsReply 
   {
      Q_OBJECT

      public:
         AwsGetFiles(AwsConnection*, QStringList const& fileList, 
            QString const& destinationPath);

      protected Q_SLOTS:
         void run();

      private Q_SLOTS:
         void replyFinished();

      private:
         QStringList m_fileList;
         QString m_destinationPath;
         QList<AwsGet*> m_replies;
         int  m_totalReplies;
         bool m_allOk;
   };



   class AwsPost : public AwsReply 
   {
      Q_OBJECT

      public:
         AwsPost(AwsConnection*, QString const& path, QByteArray const& postData);

      protected Q_SLOTS:
         void run();

      private:
         QByteArray m_postData;
         
   };

} } // end namespace IQmol::Network

#endif
