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

#include "AwsConnection.h"
#include "AwsReply.h"
#include "Exception.h"
#include "QsLog.h"
#include "QMsgBox.h"
#include "QtVersionHacks.h"

#include <QUrl>
#include <QFile>
#include <QEventLoop>
#include <QRegularExpression>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>


namespace IQmol {
namespace Network {

AwsConnection::AwsConnection(AwsConfig& config, int const port)
 : Connection("Q-Cloud", port), 
   m_networkAccessManager(0), 
   m_config(config)
{ 
   m_hostname = QString("%1.execute-api.%2.amazonaws.com")
                  .arg(m_config.ApiGateway).arg(m_config.AwsRegion);
   QLOG_DEBUG() << "Q-Cloud host set to: " << m_hostname;
}


void AwsConnection::open()
{
   m_networkAccessManager = new QNetworkAccessManager(this);
   if (!m_networkAccessManager) {
      m_message = "Failed to open HTTP connection to " + m_hostname;
      m_status = Error;
      QLOG_DEBUG() << m_message;
      return;
   }

   if (getCognitoJwks(m_config.AwsRegion, m_config.CognitoUserPool)) {
      // Don't open the connection unless we can obtain the JWKS
      m_status = Opened;

      for (const QJsonValue &key : m_jwksKeys) {
          //qDebug() << key.toObject();
      }
   }

   QLOG_DEBUG() << "Number of JWKS keys retrieved:" << m_jwksKeys.size();
}


bool AwsConnection::getCognitoJwks(QString const& region, QString const& userPoolId) 
{
    QString url = QString("https://cognito-idp.%1.amazonaws.com/%2/.well-known/jwks.json")
                    .arg(region)
                    .arg(userPoolId);
    QLOG_DEBUG() << "Fetching url:" << url;

    QNetworkRequest request{QUrl(url)};
    QNetworkReply  *reply = m_networkAccessManager->get(request);

    QObject::connect(reply, &QNetworkReply::finished, [this, reply]() 
    {
       if (reply->error() == QNetworkReply::NoError) {
          QByteArray response = reply->readAll();
          QJsonDocument doc = QJsonDocument::fromJson(response);
          if (doc.isObject()) {
              QJsonObject obj = doc.object();
              m_jwksKeys = obj.value("keys").toArray();
          }
       } else {
          QLOG_ERROR() << "Failed to fetch JWKS:" << reply->errorString();
       }
    });

    // Block until the reply is finished, not ideal but should be acceptable
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    return !m_jwksKeys.isEmpty();
}


void AwsConnection::close()
{
   if (m_networkAccessManager) {
      disconnect(m_networkAccessManager);
      delete m_networkAccessManager;
      m_networkAccessManager = 0;
   }
   
   m_status = Closed;
}


void AwsConnection::authenticate(AuthenticationT const auth, QString& userName)
{
    QString msg("Q-Cloud password for user: " );
    msg += userName;

    for (int count = 0; count < 3; ++count) {
        QString password = getPasswordFromUser(msg);
        if (password.isEmpty()) return; // User cancelled authentication 

        bool ok = authenticateUser(userName, password);
        if (ok) { 
           m_status = Network::Authenticated;
           return;
        }
    }
}


bool AwsConnection::authenticateUser(const QString &username, const QString &password) 
{
    QString url = QString("https://cognito-idp.%1.amazonaws.com/").arg(m_config.AwsRegion);

    QNetworkRequest request{QUrl(url)};
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-amz-json-1.1");
    request.setRawHeader("X-Amz-Target", "AWSCognitoIdentityProviderService.InitiateAuth");

    QJsonObject authParams{
        {"USERNAME", username},
        {"PASSWORD", password}
    };

    QJsonObject body{
        {"AuthFlow", "USER_PASSWORD_AUTH"},
        {"ClientId", m_config.CognitoAppClient},
        {"AuthParameters", authParams}
    };

    QJsonDocument doc(body);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    QNetworkReply* reply = m_networkAccessManager->post(request, data);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QJsonObject object = parseReply(reply);

    QString sessionToken = object.value("Session").toString();
    QString challenge = object.value("ChallengeName").toString();

    if (challenge  == "NEW_PASSWORD_REQUIRED") {
       resetPassword(username, password, sessionToken);
    }else {
       extractTokens(object);
    }

    return m_status == Network::Authenticated;
}



bool AwsConnection::extractTokens(QJsonObject const& obj)
{
    QJsonObject object = obj.value("AuthenticationResult").toObject();

    m_accessToken  = object.value("AccessToken").toString();
    m_refreshToken = object.value("RefreshToken").toString();
    m_idToken      = object.value("IdToken").toString();

    if (m_accessToken.isEmpty()  || 
        m_refreshToken.isEmpty() || 
        m_idToken.isEmpty() ) {
    }else {
        m_status = Network::Authenticated;
    }

    return m_status == Network::Authenticated;
}


void AwsConnection::expireToken()
{
qDebug() << "#############################################################";
qDebug() << "#############################################################";
qDebug() << "################# USING EPIRED TOKEN ########################";
qDebug() << "#############################################################";
qDebug() << "#############################################################";
   m_idToken = "eyJraWQiOiJpUDFiczYycm9wN1ZKUEFRQnkxb0hhUUZ1V1llUlZPb0F1WTJKSTJGdnU4PSIsImFsZyI6IlJTMjU2In0.eyJzdWIiOiIxNGQ4MzRjOC03MDYxLTcwMTctOGU4My01NjZiM2E3M2JhYzgiLCJlbWFpbF92ZXJpZmllZCI6dHJ1ZSwiaXNzIjoiaHR0cHM6XC9cL2NvZ25pdG8taWRwLnVzLWVhc3QtMS5hbWF6b25hd3MuY29tXC91cy1lYXN0LTFfNVhhekk4dEU4IiwiY29nbml0bzp1c2VybmFtZSI6ImJlcnQiLCJvcmlnaW5fanRpIjoiMmZjZWMyNGItMTY5Yi00NGVjLTk5YWYtYWY3MGEzNDFiMTE4IiwiYXVkIjoiNDQxZTVlMWsxaWlwbGIybXFpbWxibnZpcDgiLCJldmVudF9pZCI6IjIzY2JmM2MwLTZmYWQtNDRmNy05MTJhLTA2ZWMyMzZkNTU1NSIsInRva2VuX3VzZSI6ImlkIiwiYXV0aF90aW1lIjoxNzQ3ODI0MzI2LCJleHAiOjE3NDc4Mjc5MjYsImlhdCI6MTc0NzgyNDMyNiwianRpIjoiODcxY2Q0YTMtMDMxYi00ODExLWFiZGUtYjQwMzlkMzdkNzYzIiwiZW1haWwiOiJhbmRyZXcuaXFtb2xAZ21haWwuY29tIn0.U9w6ssJx0gUazW180e0NZ3mgYc4n2V875Ub93drpa6-weQ_wvJeJMLNKZfnYxQZERv2pQxahvEDaVNRTG8GF3NmefF1FPyEwXciQeQndEArEHK8AgMCkvhrI_Q97bU4-sHcv8ztE_1FsY6joEHaXoMjHTjKiL6LbX99bHq3tBhuw-pFHyoBe6aSf5RIQTQNEVlwQC0amkAK9Co0DHJFfGlplJXQ7tbl3F1SN1u2BK6zDqLo8WTXAIyyV_b_t7LZhRZqzOBDAVyffq-LRWews-KwdeRQslFx4DwzmV13gHSW8Vob6OR6WD7AHd8gwOKRmRLWx08gakYZcIdZiCurxhw";
}



bool AwsConnection::resetPassword(QString const& userName, QString const& oldPassword, 
   QString const& session ) 
{
    QString msg1("Q-Cloud password reset required.\nEnter new password for user: " );
    QString msg2("Confirm password");
    msg1 += userName;

    for (int count = 0; count < 3; ++count) {
        QString pw1 = getPasswordFromUser(msg1);
        if (pw1.isEmpty()) return false; // User cancelled authentication 

        QString pw2 = getPasswordFromUser(msg2);
        if (pw2.isEmpty()) return false; // User cancelled authentication 

        if (pw1 == pw2) {
           return respondToNewPasswordChallenge(userName, pw2, session);
        }else {
           QMsgBox::warning(0, "IQmol", "Passwords do not match");
        }
    }

    return false;
}


QJsonObject AwsConnection::parseReply(QNetworkReply* reply)
{
   QByteArray data = reply->readAll(); 
   QJsonParseError parseError;
   QJsonDocument document = QJsonDocument::fromJson(data, &parseError); 
   QJsonObject object;

   if (parseError.error != QJsonParseError::NoError) {
      qDebug() << "Server response data in parseReply: " << data;
      QString msg = QString("JSON parse error: ") + parseError.errorString();
      QMsgBox::warning(0, "IQmol", msg);
      return object;
   }

   object = document.object();

   // Check for incorrect password first as these trigger a Bad Request error
   QString type = object.value("__type").toString();
   if (type == "NotAuthorizedException" ||
       type == "InvalidPasswordException" ) {
      QString msg = object.value("message").toString();
      QMsgBox::warning(0, "IQmol", msg);
      return object;
   }

   if (reply->error() != QNetworkReply::NoError) {
      QString msg = QString("Authentication failed: ") + reply->errorString();
      QMsgBox::warning(0, "IQmol", msg);
   }

   return object;
}



bool AwsConnection::respondToNewPasswordChallenge(const QString& username, 
   const QString& newPassword, const QString& session)
{
    QString url = QString("https://cognito-idp.%1.amazonaws.com/").arg(m_config.AwsRegion);

    QNetworkRequest request{QUrl(url)};
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-amz-json-1.1");
    request.setRawHeader("X-Amz-Target", "AWSCognitoIdentityProviderService.RespondToAuthChallenge");

    QJsonObject challengeResponses{
        {"USERNAME", username},
        {"NEW_PASSWORD", newPassword}
    };

    QJsonObject body{
        {"ChallengeName", "NEW_PASSWORD_REQUIRED"},
        {"ClientId", m_config.CognitoAppClient},
        {"Session", session},
        {"ChallengeResponses", challengeResponses}
    };

    QJsonDocument doc(body);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    QNetworkReply* reply = m_networkAccessManager->post(request, data);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QJsonObject object = parseReply(reply);

    return extractTokens(object);
}


QStringMap AwsConnection::parseQueryMessage(QString const& message)
{
    QStringList lines(message.split(QRegularExpression("\\n"), IQmolSkipEmptyParts));

    if (lines.isEmpty()) return QStringMap{{"status", "Unknown"}, {"message", ""}};

    // Strategy is to check the last line for a status keyword, if we can't find one
    // we asume an error and that the last line elaborates on this error.  
    QString last(lines.last());
    QString status("Error");

    if (last.contains("BEGIN"))           { status = "Not Running"; } else
    if (last.contains("COPIED"))          { status = "Not Running"; } else
    if (last.contains("QUEUED"))          { status = "Queued";      } else
    if (last.contains("RUNNING"))         { status = "Running";     } else
    if (last.contains("FINISHED"))        { status = "Running";     } else // not copied yet
    if (last.contains("ARCHIVED"))        { status = "Finished";    } else
    if (last.contains("DOWNLOADED"))      { status = "Finished";    } else
    if (last.contains("CANCELLED"))       { status = "Killed";      }

//  Pass through handles these cases
//  if (last.contains("COPY FAILED"))     { status = "Error";       } else
//  if (last.contains("ERROR"))           { status = "Error";       } else
//  if (last.contains("ARCHIVED FAILED")) { status = "Error";       } else

    QString err;
    if (status == "Error") err = last;

    QStringMap ret = {{"status", status}, {"message", err}};

    return ret;
}



// ----------------------------------------------------------------------------


Reply* AwsConnection::execute(QString const& query)
{
   QByteArray auth = QString("Bearer %1").arg(m_idToken).toUtf8();
   AwsGet* reply(new AwsGet(this, query));
   reply->setHeader("Authorization", auth);
   return reply;
}


Reply* AwsConnection::execute(QString const& query, QString const&)
{
   return execute(query);
}


Reply* AwsConnection::execute(QString const& query, QStringMap const& headers)
{
   QByteArray auth = QString("Bearer %1").arg(m_idToken).toUtf8();
   AwsGet* reply(new AwsGet(this, query));
   reply->setHeader("Authorization", auth);

   QStringMap::const_iterator iter;
   for (iter = headers.begin(); iter != headers.end(); ++iter) {
       reply->setHeader(iter.key(), iter.value());
   }

   return reply;
}



Reply* AwsConnection::putFile(QString const& sourcePath, QString const& destinationPath)
{
   QFile file(sourcePath);
   QString content;


   if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      content = QString::fromUtf8(file.readAll());
      file.close();
   }else {
      QLOG_ERROR() << "Failed to open file: " << sourcePath;
   }

   QStringList lines = content.split('\n',IQmolSkipEmptyParts);
   QJsonArray jsonArray;
   for (QString const& line : lines) jsonArray.append(line.trimmed());

   QJsonObject json;
   json["input_file"] = jsonArray;

   QJsonDocument document(json);
   QByteArray buffer = document.toJson(QJsonDocument::Compact);
   
   AwsPost* reply(new AwsPost(this, destinationPath, buffer));
   reply->setHeader("Content-Type", "application/json");

   QByteArray auth = QString("Bearer %1").arg(m_idToken).toUtf8();
   reply->setHeader("Authorization", auth);
   return reply;
}


Reply* AwsConnection::getFiles(QStringList const& fileList, QString const& destinationPath)
{
   QByteArray auth = QString("Bearer %1").arg(m_idToken).toUtf8();
   AwsGetFiles* reply(new AwsGetFiles(this, fileList, destinationPath));
   reply->setHeader("Authorization", auth);
//expireToken();
   return reply;
}

             
Reply* AwsConnection::getFile(QString const& sourcePath, QString const& destinationPath)
{
   QByteArray auth = QString("Bearer %1").arg(m_idToken).toUtf8();
   AwsGet* reply(new AwsGet(this, sourcePath, destinationPath));
   reply->setHeader("Authorization", auth);
   return reply;
}


Reply* AwsConnection::post(QString const& path, QByteArray const& postData)
{
   QByteArray auth = QString("Bearer %1").arg(m_idToken).toUtf8();
   AwsPost* reply(new AwsPost(this, path, postData));
   reply->setHeader("Authorization", auth);
   return reply;
}

} } // end namespace IQmol::Network
