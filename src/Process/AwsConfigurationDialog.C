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

#include "AwsConfigurationDialog.h"
#include "ServerConfiguration.h"
#include "QMsgBox.h"
#include "QsLog.h"


namespace IQmol {
namespace Process {

AwsConfigurationDialog::AwsConfigurationDialog(ServerConfiguration& config, QWidget* parent) 
  : QDialog(parent), m_config(config)
{
   m_dialog.setupUi(this);

   connect(m_dialog.buttonBox, SIGNAL(accepted()), this, SLOT(finished()));
   connect(m_dialog.buttonBox, SIGNAL(rejected()), this, SLOT(close()));

   m_regionValidator.setRegularExpression(QRegularExpression("^(us|eu|ap|sa|ca|me|af)-[a-z]+-\\d+$"));
   m_userPoolValidator.setRegularExpression(QRegularExpression("^[a-z]{2}-[a-z]+-\\d+_[A-Za-z0-9]+$"));
   m_appClientValidator.setRegularExpression(QRegularExpression("^[a-z0-9]{26}$"));
   m_gatewayValidator.setRegularExpression(QRegularExpression("^[a-z0-9]{10}$"));

   m_dialog.region->setValidator(&m_regionValidator);
   m_dialog.userPool->setValidator(&m_userPoolValidator);
   m_dialog.appClient->setValidator(&m_appClientValidator);
   m_dialog.gateway->setValidator(&m_gatewayValidator);

   copyFrom(m_config);
}


void AwsConfigurationDialog::finished()
{
   if (!verify()) return;
   copyTo(m_config);
   accept();
}


bool AwsConfigurationDialog::verify()
{
   bool ok(true);

   QString region(m_dialog.region->text());
   QString poolId(m_dialog.userPool->text());

   if (!poolId.startsWith(region)) {
      QMsgBox::warning(this, "IQmol", "Cognito Pool ID should start with the AWS region");
      ok = false;
   }

   return ok;
}

void AwsConfigurationDialog::copyFrom(ServerConfiguration const& config)
{
   m_dialog.region->   setText(config.value(ServerConfiguration::AwsRegion));
   m_dialog.userPool-> setText(config.value(ServerConfiguration::CognitoUserPool));
   m_dialog.appClient->setText(config.value(ServerConfiguration::CognitoAppClient));
   m_dialog.gateway->  setText(config.value(ServerConfiguration::ApiGateway));
}

void AwsConfigurationDialog::copyTo(ServerConfiguration& config)
{
   config.setValue(ServerConfiguration::AwsRegion,        m_dialog.region->text());
   config.setValue(ServerConfiguration::CognitoUserPool,  m_dialog.userPool->text());
   config.setValue(ServerConfiguration::CognitoAppClient, m_dialog.appClient->text());
   config.setValue(ServerConfiguration::ApiGateway,       m_dialog.gateway->text());
}


} } // end namespace IQmol::Process
