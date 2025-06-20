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

#include "HttpConnection.h"
#include "HttpReply.h"
#include "SshConnection.h"
#include "SshReply.h"
#include "QsLog.h"
#include "QMsgBox.h"
#include "NetworkException.h"
#include <QEventLoop>
#include <QRegularExpressionValidator>
#include "Network.h"

#ifdef WIN32
#include <ws2tcpip.h>
#include <windows.h>
#else
#include <arpa/inet.h>
#include <netdb.h>
#endif


namespace IQmol {
namespace Network {


QString ToString(Status const status)
{
   QString s;

   switch (status) {
      case Closed:         s = "Closed";         break;
      case Opened:         s = "Opened";         break;
      case Authenticated:  s = "Authenticated";  break;
      case Error:          s = "Error";          break;
   }

   return s;
}


QString ToString(ConnectionT const type)
{
   QString s;

   switch (type) {
      case Local:  s = "Local";  break;
      case SSH:    s = "SSH";    break;
      case SFTP:   s = "SFTP";   break;
      case HTTP:   s = "HTTP";   break;
      case HTTPS:  s = "HTTPS";  break;
      case AWS:    s = "AWS";  break;
   }

   return s;
}


QString ToString(AuthenticationT const authentication)
{
   QString s;

   switch (authentication) {
      case Anonymous:            s = "Anonymous";             break;
      case Agent:                s = "Agent";                 break;
      case HostBased:            s = "Host Based";            break;
      case KeyboardInteractive:  s = "Keyboard Interactive";  break;
      case Password:             s = "Password";              break;
      case PublicKey:            s = "Public Key";            break;
   }

   return s;
}


Status ToStatus(QString const& s)
{
   Status status(Error);

   if (s.contains(ToString(Closed), Qt::CaseInsensitive)) {
      status = Closed;
   }else if (s.contains(ToString(Opened), Qt::CaseInsensitive)) {
      status = Opened;
   }else if (s.contains(ToString(Authenticated), Qt::CaseInsensitive)) {
      status = Authenticated;
   }

   return status;
}


ConnectionT ToConnectionT(QString const& s)
{
   ConnectionT conn(Local);

   if (s.contains(ToString(SSH), Qt::CaseInsensitive)) {
      conn = SSH;
   }else if (s.contains(ToString(SFTP), Qt::CaseInsensitive)) {
      conn = SFTP;
   }else if (s.contains(ToString(HTTPS), Qt::CaseInsensitive)) {
      conn = HTTPS;
   }else if (s.contains(ToString(HTTP), Qt::CaseInsensitive)) {
      conn = HTTP;
   }

   return conn;
}


AuthenticationT ToAuthenticationT(QString const& s)
{
   AuthenticationT auth(Anonymous);

   if (s.contains(ToString(Agent), Qt::CaseInsensitive)) {
      auth = Agent;
   }else if (s.contains(ToString(HostBased), Qt::CaseInsensitive)) {
      auth = HostBased;
   }else if (s.contains(ToString(KeyboardInteractive), Qt::CaseInsensitive)) {
      auth = KeyboardInteractive;
   }else if (s.contains(ToString(Password), Qt::CaseInsensitive)) {
      auth = Password;
   }else if (s.contains(ToString(PublicKey), Qt::CaseInsensitive)) {
      auth = PublicKey;
   }

   return auth;
}



   //Network::HttpConnection* http = new Network::HttpConnection("cactus.nci.nih.gov");
   //if (http->openConnection()) {
   //   http->request("chemical/structure/Benzene/smiles");
   //   http->request("chemical/structure/C1=CC=CC=C1/names");
  // }



bool TestNetworkConnection()
{
   bool okay(true);

   try {
      HttpConnection http("iqmol.q-chem.com");
      http.open();

      QEventLoop loop;
      Reply* reply;

      reply = http.get("download?cookie=4b6d8c1c5b3743efbc2acd550887b7a4&jobid=22dbd72c647211e4a1d190b11c4a068c&file=input.FChk");

      QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
      loop.exec();

      okay = (reply->status() == Reply::Finished);
      if (okay) {
         QLOG_DEBUG() << "--------- SUCCESS -------------";
         QLOG_DEBUG() << reply->message();
         QLOG_DEBUG() << "-------------------------------";
      }
      else {
         QLOG_DEBUG() << "----------- ERROR -------------";
         QLOG_DEBUG() << reply->message();
         QLOG_DEBUG() << "-------------------------------";
      }

      delete reply;

      reply = http.get("list?jobid=db33b0f063b511e4bda190b11c4a068c&cookie=4b6d8c1c5b3743efbc2acd550887b7a4");

      QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
      loop.exec();

      okay = (reply->status() == Reply::Finished);
      if (okay) {
         QLOG_DEBUG() << "--------- SUCCESS -------------";
         QLOG_DEBUG() << reply->message();
         QLOG_DEBUG() << "-------------------------------";
      }
      else {
         QLOG_DEBUG() << "----------- ERROR -------------";
         QLOG_DEBUG() << reply->message();
         QLOG_DEBUG() << "-------------------------------";
      }

      delete reply;

   // These catch the connection errors, not the reply errors
   }catch (AuthenticationError& err) {
      QMsgBox::warning(0, "IQmol", "Invalid username or password");

   }catch (Exception& err) {
      okay = false;
      QMsgBox::warning(0, "IQmol", err.what());
   }

   return okay;
}


in_addr_t HostLookup(QString const& hostname)
{
   in_addr_t address(INADDR_NONE);

#ifdef WIN32

   // This can only handle IPv4 addresses and should only be used when
   // inet_ntop is unavailable.
   QString octet("(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])");
   QRegularExpressionValidator ipv4Validator(0);
   ipv4Validator.setRegularExpression(QRegularExpression(
      "^" + octet + "\\." + octet + "\\." + octet + "\\." + octet + "$"));

   int pos;
   QString tmp(hostname);

   if (ipv4Validator.validate(tmp,pos) == QValidator::Acceptable) {
      address = inet_addr(hostname.toLatin1().data());
      if (address == INADDR_ANY || address == INADDR_NONE) {
         QLOG_ERROR() << "Invalid hostname: " + hostname;
         address = INADDR_NONE;
      }
   }else {
      struct hostent* host;
      host = gethostbyname(hostname.toLatin1().data());
      if (host) {
         if (host->h_addrtype == AF_INET6) {
            QLOG_ERROR() << "IPv6 addresses are not supported";
            address = INADDR_NONE;
         }else {
            address = *(in_addr_t*)host->h_addr;
         }
      }
   }

#else

   struct addrinfo hints, *res;
   int errcode;
   char addrstr[100];
   void *ptr(0);

   memset(&hints, 0, sizeof (hints));
   hints.ai_family = PF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags |= AI_CANONNAME;

   errcode = getaddrinfo(hostname.toLatin1().data(), NULL, &hints, &res);
   if (errcode == 0) {
      inet_ntop(res->ai_family, res->ai_addr->sa_data, addrstr, 100);

      switch (res->ai_family) {
         case AF_INET:
            ptr = &((struct sockaddr_in *) res->ai_addr)->sin_addr;
            break;
         case AF_INET6:
            ptr = &((struct sockaddr_in6 *) res->ai_addr)->sin6_addr;
            break;
      }

      inet_ntop(res->ai_family, ptr, addrstr, 100);
      address = inet_addr(addrstr);

      QString ipv((res->ai_family == PF_INET6) ? "IPv6 address:" : "IPv4 address:");
      QLOG_DEBUG() << ipv << QString(addrstr) << "=>" << QString(res->ai_canonname);

   }else {
      QLOG_ERROR() << "Invalid hostname: " + hostname;
      address = INADDR_NONE;
   }

#endif

   return address;
}

} } // end namespace IQmol::Network
