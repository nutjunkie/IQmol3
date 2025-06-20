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

#include "ServerRegistry.h"
#include "Preferences.h"
#include "Server.h"
#include "QMsgBox.h"
#include "Exception.h"
#include <QDir>


namespace IQmol {
namespace Process {

ServerRegistry* ServerRegistry::s_instance = 0;
QList<Server*>  ServerRegistry::s_servers;
QList<Server*>  ServerRegistry::s_deletedServers;


ServerRegistry& ServerRegistry::instance() 
{
   if (s_instance == 0) {
      s_instance = new ServerRegistry();

      loadFromPreferences();
      atexit(ServerRegistry::destroy);
   }
   return *s_instance;
}


QStringList ServerRegistry::availableServers() const
{
   QStringList list;
   QList<Server*>::iterator iter;
   for (iter = s_servers.begin(); iter != s_servers.end(); ++iter) {
       list.append((*iter)->name());
   }
   return list;
}


Server* ServerRegistry::addServer(ServerConfiguration& config)
{
   QString name(config.value(ServerConfiguration::ServerName));
   int count(0);

   while (find(name)) {
      ++count;
      name = config.value(ServerConfiguration::ServerName) + "_" + QString::number(count);
   }

   config.setValue(ServerConfiguration::ServerName, name);

   Server* server(new Server(config));
   s_servers.append(server);
   save();

   return server;
}


void ServerRegistry::closeAllConnections()
{
   QList<Server*>::iterator iter;
   for (iter = s_servers.begin(); iter != s_servers.end(); ++iter) {
       (*iter)->closeConnection();
   }
}


void ServerRegistry::connectServers(QStringList const& servers)
{
   QStringList::const_iterator iter;
   for (iter = servers.begin(); iter != servers.end(); ++iter) {
       Server* server(find(*iter));
       if (server) {
          server->open();
       }
   }
}


Server* ServerRegistry::find(QString const& serverName)
{
   int index(indexOf(serverName));
   if (index >= 0) return s_servers[index];
   return 0;
}


void ServerRegistry::remove(QString const& serverName)
{
   int index(indexOf(serverName));
   if (index >= 0) s_deletedServers.append(s_servers.takeAt(index));
   save();
}


void ServerRegistry::remove(Server* server)
{
   int index(s_servers.indexOf(server));
   if (index >= 0) s_deletedServers.append(s_servers.takeAt(index));
   save();
}


void ServerRegistry::moveUp(QString const& serverName)
{
   int index(indexOf(serverName));
#if QT_VERSION < QT_VERSION_CHECK(5,14,0)
   if (index > 0) s_servers.swap(index, index-1);
#else
   if (index > 0) s_servers.swapItemsAt(index, index-1);
#endif
   save();
}


void ServerRegistry::moveDown(QString const& serverName)
{
   int index(indexOf(serverName));
#if QT_VERSION < QT_VERSION_CHECK(5,14,0)
   if (index < s_servers.size() -1) s_servers.swap(index, index+1);
#else
   if (index < s_servers.size() -1) s_servers.swapItemsAt(index, index+1);
#endif
   save();
}


int ServerRegistry::indexOf(QString const& serverName) 
{
   for (int i = 0; i < s_servers.size(); ++i) {
      if (s_servers[i]->name() == serverName) return i;
   }
   return -1;
}


void ServerRegistry::destroy() 
{
   QList<Server*>::iterator iter;
   for (iter = s_servers.begin(); iter != s_servers.end(); ++iter) {
       delete (*iter);
   }
   for (iter = s_deletedServers.begin(); iter != s_deletedServers.end(); ++iter) {
       delete (*iter);
   }
}


void ServerRegistry::loadFromPreferences()
{
   QVariantList list(Preferences::ServerConfigurationList());
   QVariantList::iterator iter;

   bool qchemServerFound(false);

   try {
      for (iter = list.begin(); iter != list.end(); ++iter) {
          ServerConfiguration config(*iter);
          s_servers.append(new Server(config));
          if (config.isWebBased() && 
             config.value(ServerConfiguration::HostAddress) == "iqmol.q-chem.com") {
             qchemServerFound = true;
          }
      }

      // Look for default servers in the share directory
      if (s_servers.isEmpty()) {
         QDir dir(Preferences::ServerDirectory());
         QLOG_TRACE() << "Server Directory set to: " << dir.absolutePath();
         if (dir.exists()) {
            QStringList filters;
            filters << "*.cfg";
            QStringList listing(dir.entryList(filters, QDir::Files));
         
            QStringList::iterator iter;
            for (iter = listing.begin(); iter != listing.end(); ++iter) {
                QFileInfo info(dir, *iter);
                QLOG_TRACE() << "Reading server configutation from: " << info.absoluteFilePath();
                ServerConfiguration serverConfiguration;
                if (serverConfiguration.loadFromFile(info.absoluteFilePath())) {
                   addServer(serverConfiguration);
                }
            }
         }
      }

      // Finally, if the qchem server does not exist, add it to the end
      if (qchemServerFound) {
         QLOG_DEBUG() << "Found existing Q-Chem server configuration";
      }else {
         QLOG_DEBUG() << "Appending Q-Chem server";
         ServerConfiguration config;
         addServer(config);
      }

   } catch (Exception& ex) {
      QString msg("Problem loading servers from Preferences file:\n");
      msg += ex.what();
      QMsgBox::warning(0, "IQmol", msg);
   }
}


void ServerRegistry::saveToPreferences()
{
   QVariantList list;
   QList<Server*>::const_iterator iter;
   for (iter = s_servers.begin(); iter != s_servers.end(); ++iter) {
       list.append((*iter)->configuration().toQVariant());
   }
   Preferences::ServerConfigurationList(list);
}


} } // end namespace IQmol::Process
