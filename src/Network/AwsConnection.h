#ifndef IQMOL_NETWORK_AWSCONNECTION_H
#define IQMOL_NETWORK_AWSCONNECTION_H
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

#include "Connection.h"
#include <QJsonArray>
#include <QJsonObject>


class QNetworkAccessManager;
class QNetworkReply;

namespace IQmol {
namespace Network {

   struct AwsConfig {
      QString AwsRegion;
      QString CognitoUserPool; 
      QString CognitoAppClient;
      QString ApiGateway;
   };

   class AwsGet;

   class AwsConnection : public Connection {

      Q_OBJECT

      //friend class HttpPageRequest;
      friend class AwsGet;
      friend class AwsPost;

      public:
         AwsConnection(AwsConfig& config, int const port = 443);
         ~AwsConnection() { close(); }

         ConnectionT type() const { return AWS; }

         void open();
         void close();
         void authenticate(AuthenticationT const, QString& userName);

         bool blockingExecute(QString const&, QString*) { return false; } // unused 
         bool exists(QString const& ) { return false; }         // setup, not used
         bool makeDirectory(QString const&) { return true; }    // setup, not used
         bool removeDirectory(QString const&) { return true; }  // setup, not used

         Reply* execute(QString const& query);
         Reply* execute(QString const& query, QString const& /* Working directory, not used */); 
         Reply* execute(QString const& query, QStringMap const& headers);
         Reply* putFile(QString const& sourcePath, QString const& destinationPath);
         Reply* getFile(QString const& sourcePath, QString const& destinationPath);
         Reply* getFiles(QStringList const& fileList, QString const& destinationPath);
         Reply* get(QString const& query) { return execute(query); }
         Reply* post(QString const& path, QByteArray const&);

         // Returns Job::Status as a string
         QStringMap parseQueryMessage(QString const&);

      protected:
         QNetworkAccessManager* m_networkAccessManager;

      private:
         bool getCognitoJwks(QString const& region, QString const& userPoolId);
         bool authenticateUser(QString const& userName, QString const& password);
         bool resetPassword(QString const& username, QString const& password, QString const& session);
         bool respondToNewPasswordChallenge(const QString& username, const QString& newPassword, 
            const QString& session);
   
         QJsonObject parseReply(QNetworkReply*);
         bool extractTokens(QJsonObject const&);

         AwsConfig& m_config;
         QJsonArray m_jwksKeys;

         QString m_accessToken;
         QString m_refreshToken;
         QString m_idToken;
   };

} } // end namespace IQmol::Network

#endif
