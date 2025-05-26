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

#include "ServerConfigurationDialog.h"
#include "QueueOptionsDialog.h"
#include "AwsConfigurationDialog.h"
#include "QueueResourcesList.h"
#include "SshConnection.h"
#include "SshReply.h"
#include "HttpConnection.h"
#include "HttpReply.h"
#include "SshFileDialog.h"
#include "QMsgBox.h"
#include "YamlNode.h"
#include "ParseFile.h"
#include <QFileDialog>



namespace IQmol {
namespace Process {

ServerConfigurationDialog::ServerConfigurationDialog(ServerConfiguration& configuration, 
   QWidget* parent) : QDialog(parent), m_setDefaults(true),
   m_originalConfiguration(configuration)
{
   m_dialog.setupUi(this);
   m_currentConfiguration = m_originalConfiguration;

   copyFrom(m_originalConfiguration);

   connect(m_dialog.buttonBox, SIGNAL(accepted()), this, SLOT(finished()));
   connect(m_dialog.buttonBox, SIGNAL(rejected()), this, SLOT(close()));
}


void ServerConfigurationDialog::finished()
{
   if (!verify()) return;
   copyTo(m_currentConfiguration);
   m_originalConfiguration = m_currentConfiguration;
   accept();
}


/* Verifies the contents of the various fields are valid, does not verify m_serverConfiguration */
bool ServerConfigurationDialog::verify()
{
   if (m_dialog.serverName->text().trimmed().isEmpty()) {
      QMsgBox::warning(this, "IQmol", "Server name not set");
      return false;
   }

   Network::ConnectionT connection(Network::Local);
   if (m_dialog.sshRadioButton->isChecked()) {
      connection = Network::SSH;
   }else if (m_dialog.sftpRadioButton->isChecked()) {
      connection = Network::SFTP;
   }else if (m_dialog.httpRadioButton->isChecked()) {
      connection = Network::HTTP;
   }else if (m_dialog.httpsRadioButton->isChecked()) {
      connection = Network::HTTPS;
   }else if (m_dialog.awsRadioButton->isChecked()) {
      connection = Network::AWS;
   }

   if (connection != Network::Local) {
      if (m_dialog.hostAddress->text().trimmed().isEmpty()) {
         QMsgBox::warning(this, "IQmol", "Host address not set");
         return false;
      }
   }

   Network::AuthenticationT authentication = 
      Network::ToAuthenticationT(m_dialog.authentication->currentText());

   if (authentication == Network::Password) {
      if (m_dialog.userName->text().trimmed().isEmpty()) {
         QMsgBox::warning(this, "IQmol", "User name must be set");
         return false;
      }
   }

   if (m_dialog.workingDirectory->text().contains("~")) {
      QString msg("Use of ~ shortcut in working directory may not work\n");
      msg += "Suggest using $HOME or full path";
      QMsgBox::warning(this, "IQmol", msg);
      return false;
   }

   return true;
}



void ServerConfigurationDialog::copyTo(ServerConfiguration& config)
{
   Network::ConnectionT connection(Network::Local);

   if (m_dialog.sshRadioButton->isChecked()) {
      connection = Network::SSH;
   }else if (m_dialog.sftpRadioButton->isChecked()) {
      connection = Network::SFTP;
   }else if (m_dialog.httpRadioButton->isChecked()) {
      connection = Network::HTTP;
   }else if (m_dialog.httpsRadioButton->isChecked()) {
      connection = Network::HTTPS;
   }else if (m_dialog.awsRadioButton->isChecked()) {
      connection = Network::AWS;
   }

   config.setValue(ServerConfiguration::ServerName, m_dialog.serverName->text());
   config.setValue(ServerConfiguration::Connection, connection);

   QString queue(m_dialog.queueSystem->currentText());
   config.setValue(ServerConfiguration::QueueSystem, 
      ServerConfiguration::toQueueSystemT(queue));
   config.setValue(ServerConfiguration::HostAddress, "localhost");

   if (connection == Network::Local) return;

   config.setValue(ServerConfiguration::HostAddress, 
      m_dialog.hostAddress->text());

   config.setValue(ServerConfiguration::Port, 
      m_dialog.port->value());

   config.setValue(ServerConfiguration::Authentication, 
      Network::ToAuthenticationT(m_dialog.authentication->currentText()));

   config.setValue(ServerConfiguration::UserName, 
      m_dialog.userName->text());

   QString dirPath(m_dialog.workingDirectory->text());
   while (dirPath.endsWith("/")) { dirPath.chop(1); }
   while (dirPath.endsWith("\\")) { dirPath.chop(1); }
   
   config.setValue(ServerConfiguration::WorkingDirectory, dirPath);
}


void ServerConfigurationDialog::copyFrom(ServerConfiguration const& config)
{
   m_setDefaults = false;
   switch (config.connection()) {
      case Network::Local:  m_dialog.localRadioButton->setChecked(true);  break;
      case Network::SSH:    m_dialog.sshRadioButton->setChecked(true);    break;
      case Network::SFTP:   m_dialog.sftpRadioButton->setChecked(true);   break;
      case Network::HTTP:   m_dialog.httpRadioButton->setChecked(true);   break;
      case Network::HTTPS:  m_dialog.httpsRadioButton->setChecked(true);  break;
      case Network::AWS:    m_dialog.awsRadioButton->setChecked(true);    break;
   }

   m_dialog.queueSystem->setCurrentText(
      ServerConfiguration::toString(config.queueSystem()));

   m_dialog.serverName->setText(config.value(ServerConfiguration::ServerName));
   m_dialog.hostAddress->setText(config.value(ServerConfiguration::HostAddress));
   m_dialog.port->setValue(config.port());

   m_dialog.authentication->setCurrentText(
      Network::ToString(config.authentication()));

   m_dialog.userName->setText(config.value(ServerConfiguration::UserName));

   m_dialog.workingDirectory->setText(
      config.value(ServerConfiguration::WorkingDirectory));

   m_setDefaults = true;
}



void ServerConfigurationDialog::updateQueueSystemsCombo(
   Network::ConnectionT const connection)
{
   QComboBox* qs(m_dialog.queueSystem);
   QString currentText(qs->currentText());
   qs->clear();

   switch (connection) {
      case Network::HTTP:
         qs->addItem(ServerConfiguration::toString(ServerConfiguration::Web));
         break;
      case Network::HTTPS:
         qs->addItem(ServerConfiguration::toString(ServerConfiguration::Web));
         qs->addItem(ServerConfiguration::toString(ServerConfiguration::QCloud));
         break;
      case Network::AWS:
         qs->addItem(ServerConfiguration::toString(ServerConfiguration::AWS));
         break;
      case Network::Local:
      case Network::SSH:
      case Network::SFTP:
         qs->addItem(ServerConfiguration::toString(ServerConfiguration::Basic));
         qs->addItem(ServerConfiguration::toString(ServerConfiguration::PBS));
         qs->addItem(ServerConfiguration::toString(ServerConfiguration::SGE));
         qs->addItem(ServerConfiguration::toString(ServerConfiguration::SLURM));
         break;
   }

   int index(0);
   for (int i = 0; i < qs->count(); ++i) {
       if (qs->itemText(i) == currentText) {
          index = i;  break;
       }
   }
   qDebug()<< "setting current index to " << index;
   qs->setCurrentIndex(index);
   on_queueSystem_currentIndexChanged(qs->currentText());
   qDebug()<< "current text now" << qs->currentText();
}



void ServerConfigurationDialog::updateAuthenticationCombo(
   Network::ConnectionT const connection)
{
   QComboBox* auth(m_dialog.authentication);
   QString currentText(auth->currentText());
   auth->clear();

   switch (connection) {

      case Network::Local:
      case Network::HTTP:
         auth->addItem(Network::ToString(Network::Anonymous)); 
         // This is insecure and only here for testing
         auth->addItem(Network::ToString(Network::Password)); 
         break;

      case Network::HTTPS:
         auth->addItem(Network::ToString(Network::Anonymous)); 
         auth->addItem(Network::ToString(Network::Password)); 
         break;

      case Network::AWS:
         auth->addItem(Network::ToString(Network::Password)); 
         break;

      case Network::SSH:
      case Network::SFTP:
         auth->addItem(Network::ToString(Network::Anonymous)); 
         auth->addItem(Network::ToString(Network::Agent)); 
         auth->addItem(Network::ToString(Network::HostBased)); 
         auth->addItem(Network::ToString(Network::KeyboardInteractive));
         auth->addItem(Network::ToString(Network::Password)); 
         auth->addItem(Network::ToString(Network::PublicKey)); 
   }
    
   int index(0);
   for (int i = 0; i < auth->count(); ++i) {
       if (auth->itemText(i) == currentText) {
          index = i;  break;
       }
   }

   auth->setCurrentIndex(index);
}



void ServerConfigurationDialog::on_localRadioButton_toggled(bool tf)
{
   if (!tf) return;

   m_dialog.remoteHostGroupBox->setEnabled(false);
   m_dialog.configureSshButton->setEnabled(false);

   updateQueueSystemsCombo(Network::Local);
   updateAuthenticationCombo(Network::Local);

   if (m_setDefaults) {
      qDebug() << "Setting defaults for Local";
      m_currentConfiguration.setDefaults(Network::Local);
      copyFrom(m_currentConfiguration);
  }
}


void ServerConfigurationDialog::on_sshRadioButton_toggled(bool tf)
{
   if (!tf) return;

   m_dialog.remoteHostGroupBox->setEnabled(true);
   m_dialog.configureSshButton->setEnabled(true);
   m_dialog.authentication->setEnabled(true);
   m_dialog.userName->setEnabled(true);
   m_dialog.workingDirectory->setEnabled(true);
   m_dialog.workingDirectoryLabel->setEnabled(true);
   m_dialog.awsConfiguration->setVisible(false);
   m_dialog.hostAddress->setEnabled(true);

   updateQueueSystemsCombo(Network::SSH);
   updateAuthenticationCombo(Network::SSH);

   if (m_setDefaults) {
      qDebug() << "Setting defaults for SSH";
      m_currentConfiguration.setDefaults(Network::SSH);
      copyFrom(m_currentConfiguration);
   }
}


void ServerConfigurationDialog::on_sftpRadioButton_toggled(bool tf)
{
   if (!tf) return;

   m_dialog.remoteHostGroupBox->setEnabled(true);
   m_dialog.configureSshButton->setEnabled(true);
   m_dialog.authentication->setEnabled(true);
   m_dialog.userName->setEnabled(true);
   m_dialog.workingDirectory->setEnabled(true);
   m_dialog.workingDirectoryLabel->setEnabled(true);
   m_dialog.awsConfiguration->setVisible(false);
   m_dialog.hostAddress->setEnabled(true);

   updateQueueSystemsCombo(Network::SFTP);
   updateAuthenticationCombo(Network::SFTP);

   if (m_setDefaults) {
      qDebug() << "Setting defaults for SFTP";
      m_currentConfiguration.setDefaults(Network::SFTP);
      copyFrom(m_currentConfiguration);
   }
}


void ServerConfigurationDialog::on_httpRadioButton_toggled(bool tf)
{
   if (!tf) return;

   m_dialog.remoteHostGroupBox->setEnabled(true);
   m_dialog.configureSshButton->setEnabled(false);
   m_dialog.authentication->setEnabled(true);
   m_dialog.userName->setEnabled(false);
   m_dialog.workingDirectory->setEnabled(false);
   m_dialog.workingDirectoryLabel->setEnabled(false);
   m_dialog.awsConfiguration->setVisible(false);
   m_dialog.hostAddress->setEnabled(true);

   updateQueueSystemsCombo(Network::HTTP);
   updateAuthenticationCombo(Network::HTTP);

   if (m_setDefaults) {
      qDebug() << "Setting defaults for HTTP";
      m_currentConfiguration.setDefaults(Network::HTTP);
      copyFrom(m_currentConfiguration);
   }
}


void ServerConfigurationDialog::on_httpsRadioButton_toggled(bool tf)
{
   if (!tf) return;

   m_dialog.remoteHostGroupBox->setEnabled(true);
   m_dialog.configureSshButton->setEnabled(false);
   m_dialog.authentication->setEnabled(true);
   m_dialog.userName->setEnabled(true);
   m_dialog.workingDirectory->setEnabled(false);
   m_dialog.workingDirectoryLabel->setEnabled(false);
   m_dialog.awsConfiguration->setVisible(false);
   m_dialog.hostAddress->setEnabled(true);

   updateQueueSystemsCombo(Network::HTTPS);
   updateAuthenticationCombo(Network::HTTPS);

   if (m_setDefaults) {
      qDebug() << "Setting defaults for HTTPS";
      m_currentConfiguration.setDefaults(Network::HTTPS);
      copyFrom(m_currentConfiguration);
   }
}


void ServerConfigurationDialog::on_awsRadioButton_toggled(bool tf)
{
   if (!tf) return;

   m_dialog.remoteHostGroupBox->setEnabled(true);
   m_dialog.configureSshButton->setEnabled(false);
   m_dialog.authentication->setEnabled(true);
   m_dialog.userName->setEnabled(true);
   m_dialog.workingDirectory->setEnabled(false);
   m_dialog.workingDirectoryLabel->setEnabled(false);
   m_dialog.awsConfiguration->setVisible(true);
   m_dialog.hostAddress->setEnabled(false);

   updateQueueSystemsCombo(Network::AWS);
   updateAuthenticationCombo(Network::AWS);

   if (m_setDefaults) {
      qDebug() << "Setting defaults for AWS";
      m_currentConfiguration.setDefaults(Network::AWS);
      copyFrom(m_currentConfiguration);
   }
}



void ServerConfigurationDialog::on_configureSshButton_clicked(bool)
{
   SshFileDialog dialog(&m_currentConfiguration, this); 
   dialog.exec();
}


void ServerConfigurationDialog::on_configureQueueButton_clicked(bool)
{
   copyTo(m_currentConfiguration);
   QueueOptionsDialog dialog(&m_currentConfiguration, this);
   dialog.exec();
}


void ServerConfigurationDialog::on_awsConfiguration_clicked(bool)
{
   AwsConfigurationDialog dialog(m_currentConfiguration, this);
   dialog.exec();
}


void ServerConfigurationDialog::on_queueSystem_currentIndexChanged(QString const& queue)
{
   if (m_setDefaults) {
      ServerConfiguration::QueueSystemT queueT(ServerConfiguration::toQueueSystemT(queue));
      m_currentConfiguration.setDefaults(queueT);
   }
}


void ServerConfigurationDialog::on_authentication_currentIndexChanged(QString const& auth)
{
   if (m_dialog.queueSystem->currentText() == 
      ServerConfiguration::toString(ServerConfiguration::Web)) {
      bool pw(auth == Network::ToString(Network::Password));
      m_dialog.userName->setEnabled(pw);
      m_dialog.userNameLabel->setEnabled(pw);
   }
}





// This should be refactored to use the ServerRegistry::loadFromFile() code.
void ServerConfigurationDialog::on_loadButton_clicked(bool)
{
   QString filePath(QDir::homePath()); 
   filePath = QFileDialog::getOpenFileName(this, tr("Open Server Configuration"),
      filePath, tr("Configuration Files (*.cfg)"));

   if (filePath.isEmpty()) return;
   
/* -------------------------------------------------------- *\
   QFile file(filePath);
   if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      QLOG_ERROR() << "File does not exist";
      return;
   }

   QTextStream in(&file);
   QueueResourcesList qrl;
   qrl.fromPbsQueueInfoString(in.readAll());
   qDebug() << "Server configuration read in from file:";
   qDebug() << qrl.toQVariantList();

   return;
\* -------------------------------------------------------- */

   try {
      Parser::ParseFile parser(filePath);
      parser.start();
      parser.wait();

      QStringList errors(parser.errors());
      if (!errors.isEmpty()) {
         QMsgBox::warning(this, "IQmol", errors.join("\n"));
      }

      Data::Bank& bank(parser.data());
      QList<Data::YamlNode*> yaml(bank.findData<Data::YamlNode>());
      if (yaml.first()) {
         yaml.first()->dump();
         m_currentConfiguration = ServerConfiguration(*(yaml.first()));
         copyFrom(m_currentConfiguration);
      }

   } catch (YAML::Exception& err) {
      QString msg(QString::fromStdString(err.what()));
      QMsgBox::warning(this, "IQmol", msg);
   }
}
       

void ServerConfigurationDialog::on_exportButton_clicked(bool)
{
   if (!verify()) return;

   copyTo(m_currentConfiguration);

   QString filePath(QDir::homePath()); 
   filePath += "/iqmol_server.cfg";

   filePath = QFileDialog::getSaveFileName(this, tr("Save File"), filePath, 
       tr("Configuration Files (*.cfg)"));

   if (filePath.isEmpty()) return;
   Data::YamlNode node(m_currentConfiguration.toYamlNode());
   if (!node.saveToFile(filePath)) {
      QMsgBox::warning(this, "IQmol", "Failed to export server configuration");
   }
}

} } // end namespace IQmol::Process
