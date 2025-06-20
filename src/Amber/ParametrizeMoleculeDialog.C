/*******************************************************************************
         
  Copyright (C) 2023 Andrew Gilbert
      
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

#include "Layer/MoleculeLayer.h"
#include "Layer/SystemLayer.h"
#include "Util/QMsgBox.h"
#include "Util/Preferences.h"
#include "Amber/ConfigDialog.h"
#include "Amber/ParametrizeMoleculeDialog.h"
#include "WaitingSpinner.h"

#include <QDir>

namespace IQmol {
namespace Amber { 

ParametrizeMoleculeDialog::ParametrizeMoleculeDialog(QWidget* parent, 
   Layer::Molecule* molecule) : QDialog(parent), m_molecule(molecule)
{
   m_dialog.setupUi(this);
   m_dialog.chargeSpin->setValue(m_molecule->totalCharge());
   m_dialog.multiplicitySpin->setValue(m_molecule->multiplicity());

   // set window title
   QString name(m_molecule->text().split(' ').first());
   setWindowTitle(tr("Parametrize ") + name);

   // set up the run button
   QPushButton* runButton = m_dialog.buttonBox->button(QDialogButtonBox::Apply);
   runButton->setText(tr("Run"));
   QPushButton* stopButton = m_dialog.buttonBox->addButton("Stop", QDialogButtonBox::ActionRole);
   stopButton->setEnabled(false);

   WaitingSpinner* spinner = new WaitingSpinner(this, false, false);
   spinner->setRoundness(70.0);
   spinner->setMinimumTrailOpacity(15.0);
   spinner->setTrailFadePercentage(70.0);
   spinner->setNumberOfLines(12);
   spinner->setLineLength(5);
   spinner->setLineWidth(2);
   spinner->setInnerRadius(3);
   spinner->setRevolutionsPerSecond(1);
   m_dialog.horizontalLayout->addWidget(spinner);
   m_dialog.horizontalLayout->setAlignment(spinner, Qt::AlignLeft);

   connect(runButton, &QPushButton::clicked, this, &ParametrizeMoleculeDialog::run);
   connect(runButton, &QPushButton::clicked, runButton, [runButton]() { runButton->setEnabled(false); });
   connect(runButton, &QPushButton::clicked, stopButton, [stopButton]() { stopButton->setEnabled(true); });
   connect(runButton, &QPushButton::clicked, spinner, &WaitingSpinner::start);

   connect(stopButton, &QPushButton::clicked, this, &ParametrizeMoleculeDialog::killed);
   connect(stopButton, &QPushButton::clicked, runButton, [runButton]() { runButton->setEnabled(true); });
   connect(stopButton, &QPushButton::clicked, stopButton, [stopButton]() { stopButton->setEnabled(false); });

   connect(this, &ParametrizeMoleculeDialog::finished, runButton, [runButton]() { runButton->setEnabled(true); });
   connect(this, &ParametrizeMoleculeDialog::finished, stopButton, [stopButton]() { stopButton->setEnabled(false); });
   connect(this, &ParametrizeMoleculeDialog::finished, spinner, &WaitingSpinner::stop);
}

void ParametrizeMoleculeDialog::on_chargeSpin_valueChanged(int value)
{
   m_molecule->chargeAvailable(value);
   m_dialog.multiplicitySpin->setValue(m_molecule->multiplicity());
}

void ParametrizeMoleculeDialog::on_multiplicitySpin_valueChanged(int value)
{
   m_molecule->multiplicityAvailable(value);
   m_dialog.chargeSpin->setValue(m_molecule->totalCharge());
}

void ParametrizeMoleculeDialog::run()
{
   // gather data
   forceField = m_dialog.forceField->currentText();

   // check if the Amber directory is set
   QString AmberDirectory = Preferences::AmberDirectory();
   if (AmberDirectory.isEmpty()) {
      QMessageBox::warning(this, tr("Amber directory not set"),
         tr("The Amber directory is not set. Please set it in the Edit Amber Config dialog."));
      Amber::ConfigDialog dialog(this);
      dialog.exec();
      if (dialog.result() == QDialog::Accepted) {
         Preferences::AmberDirectory(dialog.getDirectory());
      }
      return;
   }

   runAntechamber();
}

void ParametrizeMoleculeDialog::runAntechamber()
{
   // Call antechamer to parametrize the molecule
   QProcess *antechamber = new QProcess(this);
   antechamber->setProcessChannelMode(QProcess::MergedChannels);

   connect(antechamber, &QProcess::readyReadStandardOutput, [antechamber,this]() {
        auto output=antechamber->readAllStandardOutput();
        this->m_dialog.logTextBrowser->append(output.trimmed() + "\n");
    });

   connect(antechamber, SIGNAL(finished(int,QProcess::ExitStatus)),
      this, SLOT(antechamberFinished(int,QProcess::ExitStatus)));
   connect(this, SIGNAL(killed()), antechamber, SLOT(kill()));

   // Find antechamber executable
   QString AmberDirectory = Preferences::AmberDirectory();
   QString program = QDir(AmberDirectory).filePath("bin/antechamber");
   qDebug () << "Antechamber executable location: " << program;

   // Set working directory
   Layer::System* system(m_molecule->findLayers<Layer::System>(Layer::Parents).first());
   antechamber->setWorkingDirectory(system->getFile().absolutePath());
   qDebug() << "Antechamber working directory: " << system->getFile().absolutePath();

   // Prepare arguments and write the input mol2 file
   QString name(m_molecule->text().split(' ').first());
   m_molecule->writeToFile(QDir(system->getFile().absolutePath()).filePath(name + "_iqmol" + ".mol2"));
   qDebug() << "charge" << m_molecule->totalCharge() << "multiplicity" << m_molecule->multiplicity() << forceField << name;

   // Build arguments
   QStringList arguments;
   arguments << "-i" << name + "_iqmol" + ".mol2"
             << "-fi" << "mol2"
             << "-o" << name + ".mol2"
             << "-fo" << "mol2"
             << "-nc" << QString::number(m_molecule->totalCharge())
             << "-m" << QString::number(m_molecule->multiplicity())
             << "-s" << "2"
             << "-pf" << "yes"
             << "-dr" << "no"
             << "-rn" << name
             << "-at" << forceField
             << "-c" << "bcc";
   qDebug() << "Arguments: " << arguments;

   antechamber->start(program, arguments);
}

void ParametrizeMoleculeDialog::antechamberFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
   QProcess* antechamber(qobject_cast<QProcess*>(sender()));
   qDebug() << "Antechamber finished with exit code: " << exitCode;
   antechamber->deleteLater();

   if (exitStatus == QProcess::NormalExit && exitCode == 0) {
      qDebug() << "Antechamber finished";
      m_dialog.logTextBrowser->append("Antechamber finished normally.\n");
      emit m_molecule->parameterFileAvailable(m_molecule->text().split(' ').first() + ".mol2");
      runParmchk2();
   } else {
      qDebug() << "Antechamber crashed";
      m_dialog.logTextBrowser->append("Antechamber crashed.\n");
      finished();
   }
}

void ParametrizeMoleculeDialog::runParmchk2()
{
   // Call parmchk2 to check the parameters
   QProcess *parmchk2 = new QProcess(this);
   parmchk2->setProcessChannelMode(QProcess::MergedChannels);

   connect(parmchk2, &QProcess::readyReadStandardOutput, [parmchk2,this]() {
        auto output=parmchk2->readAllStandardOutput();
        this->m_dialog.logTextBrowser->append(output.trimmed() + "\n");
    });

   connect(parmchk2, SIGNAL(finished(int,QProcess::ExitStatus)),
      this, SLOT(parmchk2Finished(int,QProcess::ExitStatus)));
   connect(this, SIGNAL(killed()), parmchk2, SLOT(kill()));

   // Find parmchk2 executable
   QString AmberDirectory = Preferences::AmberDirectory();
   QString program = QDir(AmberDirectory).filePath("bin/parmchk2");
   qDebug() << "Parmchk2 executable location: " << program;

   // Set working directory
   Layer::System* system(m_molecule->findLayers<Layer::System>(Layer::Parents).first());
   parmchk2->setWorkingDirectory(system->getFile().absolutePath());
   qDebug() << "Parmchk2 working directory: " << system->getFile().absolutePath();

   // Prepare arguments
   QString name(m_molecule->text().split(' ').first());

   // Build arguments
   QStringList arguments;
   arguments << "-i" << name + ".mol2"
            << "-f" << "mol2"
            << "-o" << name + ".frcmod"
            << "-s" << "2"
            << "-a" << "N"
            << "-w" << "Y";
   qDebug() << "Arguments: " << arguments;

   parmchk2->start(program, arguments);
}

void ParametrizeMoleculeDialog::parmchk2Finished(int exitCode, QProcess::ExitStatus exitStatus)
{
   QProcess* process(qobject_cast<QProcess*>(sender()));
   qDebug() << "Parmchk2 finished with exit code: " << exitCode;
   process->deleteLater();

   if (exitStatus == QProcess::NormalExit && exitCode == 0) {
      qDebug() << "Parmchk2 finished";
      m_dialog.logTextBrowser->append("Parmchk2 finished normally.\n");
      emit m_molecule->parameterFileAvailable(m_molecule->text().split(' ').first() + ".frcmod");
   } else {
      qDebug() << "Parmchk2 crashed";
      m_dialog.logTextBrowser->append("Parmchk2 crashed.\n");
   }

   finished();
}

} } // end namespace IQmol::Amber
