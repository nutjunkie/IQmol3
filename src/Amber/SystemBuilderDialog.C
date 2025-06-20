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

#include "Layer/SystemLayer.h"
#include "Layer/MoleculeLayer.h"
#include "Layer/AtomLayer.h"
#include "Util/QMsgBox.h"
#include "Util/Preferences.h"
#include "Amber/ConfigDialog.h"
#include "Amber/SystemBuilderDialog.h"

#include <QDir>
#include <QFileDialog>
#include <QProcess>

namespace IQmol {
namespace Amber { 


SystemBuilderDialog::SystemBuilderDialog(QWidget* parent,
   Layer::System* system) : QDialog(parent), m_system(system)
{
   m_dialog.setupUi(this);

   resetTleapInput();

   QPushButton* runButton = m_dialog.buttonBox->button(QDialogButtonBox::Apply);
   runButton->setText(tr("Run"));
   runButton->setDefault(true);
   QPushButton* stopButton = m_dialog.buttonBox->addButton("Stop", QDialogButtonBox::ActionRole);
   stopButton->setEnabled(false);
   QPushButton* resetButton = m_dialog.buttonBox->button(QDialogButtonBox::Reset);

   connect(m_dialog.editCheckBox, &QCheckBox::toggled, m_dialog.inputTextEdit, &QPlainTextEdit::setReadOnly);
   connect(m_dialog.editCheckBox, &QCheckBox::toggled, m_dialog.toolBox, &QWidget::setEnabled);
   connect(m_dialog.editCheckBox, &QCheckBox::toggled, [this](bool checked) {
      if (checked) {
         m_dialog.inputTextEdit->setReadOnly(false);
         m_dialog.toolBox->setEnabled(false);
      } else {
         int ret = QMessageBox::warning(this, tr("Warning"),
                               tr("The manual edits will be lost.\n"
                                  "Are you sure you want to continue?"),
                               QMessageBox::Ok | QMessageBox::Cancel,
                               QMessageBox::Cancel);
         if (ret == QMessageBox::Cancel) {
            m_dialog.editCheckBox->setChecked(true);
            return;
         }
         m_dialog.inputTextEdit->setReadOnly(true);
         m_dialog.toolBox->setEnabled(true);
         updateTleapInput();
      }
   });

   connect(runButton, &QPushButton::clicked, this, &SystemBuilderDialog::runTleap);
   connect(runButton, &QPushButton::clicked, runButton, [runButton]() { runButton->setEnabled(false); });
   connect(runButton, &QPushButton::clicked, stopButton, [stopButton]() { stopButton->setEnabled(true); });
   connect(runButton, &QPushButton::clicked, resetButton, [resetButton]() { resetButton->setEnabled(false); });
   
   connect(runButton, &QPushButton::clicked, m_dialog.tabWidget, [this]() {
      m_dialog.tabWidget->setCurrentIndex(1);
   });
   connect(runButton, &QPushButton::clicked, [this]() {
      m_dialog.outputTextEdit->clear();
   });

   connect(stopButton, &QPushButton::clicked, this, &SystemBuilderDialog::killed);
   connect(stopButton, &QPushButton::clicked, runButton, [runButton]() { runButton->setEnabled(true); });
   connect(stopButton, &QPushButton::clicked, stopButton, [stopButton]() { stopButton->setEnabled(false); });

   connect(resetButton, &QPushButton::clicked, this, &SystemBuilderDialog::resetTleapInput);
   connect(resetButton, &QPushButton::clicked, runButton, [runButton]() { runButton->setEnabled(true); });
   connect(resetButton, &QPushButton::clicked, m_dialog.tabWidget, [this]() {
      m_dialog.tabWidget->setCurrentIndex(0);
   });

   connect(this, &SystemBuilderDialog::finished, runButton, [runButton]() { runButton->setEnabled(true); });
   connect(this, &SystemBuilderDialog::finished, stopButton, [stopButton]() { stopButton->setEnabled(false); });
   connect(this, &SystemBuilderDialog::finished, resetButton, [resetButton]() { resetButton->setEnabled(true); });

   // Solvent
   connect(m_dialog.solventCommandComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTleapInput()));
   connect(m_dialog.solventBoxComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTleapInput()));
   connect(m_dialog.solventDistanceDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateTleapInput()));
   connect(m_dialog.soleventIsometricCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateTleapInput()));
   connect(m_dialog.solventClosenessDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateTleapInput()));

   // Ions
   connect(m_dialog.ionsCommandComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTleapInput()));
   connect(m_dialog.ion1ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTleapInput()));
   connect(m_dialog.ion2ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTleapInput()));
   connect(m_dialog.numIon1SpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateTleapInput()));
   connect(m_dialog.numIon2SpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateTleapInput()));
}

void SystemBuilderDialog::addMolecule(const Layer::Molecule* molecule)
{
   QString name(molecule->text().split(' ').first());
   if (m_dialog.parametersPage->findChild<QLabel *>(name + "molName")) {
      return;
   }

   QGroupBox *groupBox;
   QVBoxLayout *verticalLayout;
   QGridLayout *gridLayout;
   QLabel *molName;
   QPushButton *pushButton;
   QLabel *mol2;
   QLineEdit *mol2LineEdit;
   QToolButton *mol2ToolButton;
   QLabel *frcmod;
   QLineEdit *frcmodLineEdit;
   QToolButton *frcmodToolButton;

   groupBox = new QGroupBox(m_dialog.parametersPage);
   groupBox->setObjectName(name + QString::fromUtf8("groupBox"));
   verticalLayout = new QVBoxLayout(groupBox);
   verticalLayout->setObjectName(name + QString::fromUtf8("verticalLayout"));
   gridLayout = new QGridLayout();
   gridLayout->setObjectName(name + QString::fromUtf8("gridLayout"));

   molName = new QLabel(groupBox);
   molName->setObjectName(name + QString::fromUtf8("molName"));
   molName->setText(name);
   gridLayout->addWidget(molName, 0, 0, 1, 1);

   pushButton = new QPushButton(groupBox);
   pushButton->setObjectName(name + QString::fromUtf8("pushButton"));
   pushButton->setText(tr("Parametrize"));
   gridLayout->addWidget(pushButton, 0, 1, 1, 1);

   mol2 = new QLabel(groupBox);
   mol2->setObjectName(name + QString::fromUtf8("mol2"));
   mol2->setText(tr("mol2"));
   gridLayout->addWidget(mol2, 1, 0, 1, 1);

   mol2LineEdit = new QLineEdit(groupBox);
   mol2LineEdit->setObjectName(name + QString::fromUtf8("mol2LineEdit"));
   gridLayout->addWidget(mol2LineEdit, 1, 1, 1, 1);

   mol2ToolButton = new QToolButton(groupBox);
   mol2ToolButton->setObjectName(name + QString::fromUtf8("mol2ToolButton"));
   mol2ToolButton->setText(tr("..."));
   gridLayout->addWidget(mol2ToolButton, 1, 2, 1, 1);

   frcmod = new QLabel(groupBox);
   frcmod->setObjectName(name + QString::fromUtf8("frcmod"));
   frcmod->setText(tr("frcmod"));
   gridLayout->addWidget(frcmod, 2, 0, 1, 1);

   frcmodLineEdit = new QLineEdit(groupBox);
   frcmodLineEdit->setObjectName(name + QString::fromUtf8("frcmodLineEdit"));
   gridLayout->addWidget(frcmodLineEdit, 2, 1, 1, 1);

   frcmodToolButton = new QToolButton(groupBox);
   frcmodToolButton->setObjectName(name + QString::fromUtf8("frcmodToolButton"));
   frcmodToolButton->setText(tr("..."));
   gridLayout->addWidget(frcmodToolButton, 2, 2, 1, 1);

   verticalLayout->addLayout(gridLayout);

   m_dialog.parametersLayout->addWidget(groupBox);

   connect(pushButton, SIGNAL(clicked()), molecule, SLOT(parametrizeMoleculeDialog()));
   connect(molecule, SIGNAL(parameterFileAvailable(const QString&)), this, SLOT(findParameterFile(const QString&)));
   connect(mol2LineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(updateTleapInput()));
   connect(frcmodLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(updateTleapInput()));
   connect(mol2ToolButton, SIGNAL(clicked()), this, SLOT(browseParameterFile()));
   connect(frcmodToolButton, SIGNAL(clicked()), this, SLOT(browseParameterFile()));
}

void SystemBuilderDialog::findParameterFile(const QString& fileName)
{
   if (!QDir(m_system->getFile().absolutePath()).exists(fileName)) {
      return;
   }

   QFileInfo fileInfo(fileName);
   QString name = fileInfo.baseName();
   QString suffix = fileInfo.suffix();

   qDebug() << "Found Parameter File: " << name << " " << suffix;

   if (suffix == "mol2") {
      QLineEdit *lineEdit = m_dialog.parametersPage->findChild<QLineEdit *>(name + "mol2LineEdit");
      lineEdit->setText(fileName);
   } else if (suffix == "frcmod") {
      QLineEdit *lineEdit = m_dialog.parametersPage->findChild<QLineEdit *>(name + "frcmodLineEdit");
      lineEdit->setText(fileName);
   }
}

void SystemBuilderDialog::browseParameterFile()
{
   QToolButton *button = qobject_cast<QToolButton *>(sender());
   QString nameSuffix = button->objectName().split("ToolButton").first();
   QString suffix = nameSuffix.endsWith("mol2") ? "mol2" : "frcmod";
   QString name = nameSuffix.split(suffix).first();
   QString fileName = QFileDialog::getOpenFileName(this, tr("Open") + suffix + tr("file"), m_system->getFile().absolutePath(),
      tr("*.") + suffix);

   fileName = QDir(m_system->getFile().absolutePath()).relativeFilePath(fileName);

   if (!fileName.isEmpty()) {
      QLineEdit *lineEdit = m_dialog.parametersPage->findChild<QLineEdit *>(name + suffix + "LineEdit");
      lineEdit->setText(fileName);
   }
}

void SystemBuilderDialog::resetTleapInput()
{
   QString AmberDirectory = Preferences::AmberDirectory();
   if (AmberDirectory.isEmpty()) {
      QMessageBox::warning(this, tr("Amber directory not set"),
         tr("The Amber directory is not set. Please set it in the Edit Amber Config dialog."));
      Amber::ConfigDialog dialog(this);
      dialog.exec();
      if (dialog.result() == QDialog::Accepted) {
         Preferences::AmberDirectory(dialog.getDirectory());
      }
      AmberDirectory = Preferences::AmberDirectory();
   }

   QDir dir(AmberDirectory + "/dat/leap/cmd");
   QList<QString> defaultSourceFiles = {"leaprc.protein.ff14SB", "leaprc.water.tip3p", "leaprc.gaff2"};

   m_dialog.sourceSelectedList->clear();
   m_dialog.sourceAvailList->clear();
   for (const QString &filename : dir.entryList(QDir::Files))
      if (defaultSourceFiles.contains(filename)) {
         m_dialog.sourceSelectedList->addItem(filename);
      } else {
         m_dialog.sourceAvailList->addItem(filename);
      }

   MoleculeList molecules(m_system->findLayers<Layer::Molecule>(Layer::Children));
   for (MoleculeList::iterator it = molecules.begin(); it != molecules.end(); ++it) {
      AtomList atoms((*it)->findLayers<Layer::Atom>(Layer::Children));
      if (atoms.size() > 1) {
         QString name((*it)->text().split(' ').first());
         addMolecule(*it);
         findParameterFile(name + ".mol2");
         findParameterFile(name + ".frcmod");
      }
   }

   QList<QString> const positiveIonsList = {"Na+", "K+"};
   QList<QString> const negativeIonsList = {"Cl-"};

   for (const QString &ion : positiveIonsList) {
      m_dialog.ion1ComboBox->addItem(ion);
   }
   for (const QString &ion : negativeIonsList) {
      m_dialog.ion1ComboBox->addItem(ion);
   }

   updateTleapInput();
}

void SystemBuilderDialog::updateTleapInput()
{
   m_dialog.inputTextEdit->clear();
   for (int i = 0; i < m_dialog.sourceSelectedList->count(); i++) {
      m_dialog.inputTextEdit->appendPlainText("source " + m_dialog.sourceSelectedList->item(i)->text());
   }
   m_dialog.inputTextEdit->appendPlainText("");

   for (int i = 0; i < m_dialog.parametersLayout->count(); i++) {
      QGroupBox *groupBox = qobject_cast<QGroupBox *>(m_dialog.parametersLayout->itemAt(i)->widget());
      QString name = groupBox->objectName().split("groupBox").first();
      QLineEdit *mol2LineEdit = groupBox->findChild<QLineEdit *>(name + "mol2LineEdit");
      QLineEdit *frcmodLineEdit = groupBox->findChild<QLineEdit *>(name + "frcmodLineEdit");
      if (!mol2LineEdit->text().isEmpty()) {
         m_dialog.inputTextEdit->appendPlainText(name + " = loadmol2 " + mol2LineEdit->text());
      }
      if (!frcmodLineEdit->text().isEmpty()) {
         m_dialog.inputTextEdit->appendPlainText("loadamberparams " + frcmodLineEdit->text());
      }
      if (!mol2LineEdit->text().isEmpty()) {
         m_dialog.inputTextEdit->appendPlainText("check " + name + "\n");
      }
   }

   m_dialog.inputTextEdit->appendPlainText("mol = loadpdb " + m_system->text() + "_iqmol.pdb\n");

   QString solventCommand(m_dialog.solventCommandComboBox->currentText());
   if (!solventCommand.isEmpty()) {
      QString solventBox(m_dialog.solventBoxComboBox->currentText());
      QString distance(m_dialog.solventDistanceDoubleSpinBox->text());
      QString isometric(m_dialog.soleventIsometricCheckBox->isChecked() ? " iso" : "");
      QString closeness(m_dialog.solventClosenessDoubleSpinBox->text());
      m_dialog.inputTextEdit->appendPlainText(solventCommand + " mol " + solventBox + " " + distance + isometric + " " + closeness + "\n");
   }

   QString ionsCommand(m_dialog.ionsCommandComboBox->currentText());
   if (!ionsCommand.isEmpty()) {
      QString ionsInput;
      QString ion1(m_dialog.ion1ComboBox->currentText());
      QString ion2(m_dialog.ion2ComboBox->currentText());
      int numIon1(m_dialog.numIon1SpinBox->value());
      int numIon2(m_dialog.numIon2SpinBox->value());

      ionsInput = ionsCommand + " mol";
      if (!ion1.isEmpty()) {
         ionsInput += " " + ion1 + " " + QString::number(numIon1);
      }
      if (!ion2.isEmpty() && (numIon1 != 0)) {
         ionsInput += " " + ion2 + " " + QString::number(numIon2);
      }
      m_dialog.inputTextEdit->appendPlainText(ionsInput + "\n");
   }

   m_dialog.inputTextEdit->appendPlainText("savepdb mol " + m_system->text() + "_tleap.pdb");
   m_dialog.inputTextEdit->appendPlainText("saveamberparm mol " + m_system->text() + ".parm7 " + m_system->text() + ".rst7");
}

void SystemBuilderDialog::runTleap()
{
   QProcess *tleap = new QProcess(this);
   tleap->setProcessChannelMode(QProcess::MergedChannels);

   connect(tleap, &QProcess::readyReadStandardOutput, [tleap,this]() {
        auto output=tleap->readAllStandardOutput();
        this->m_dialog.outputTextEdit->appendPlainText(output.trimmed() + "\n");
    });

   connect(tleap, SIGNAL(finished(int,QProcess::ExitStatus)),
      this, SLOT(tleapFinished(int,QProcess::ExitStatus)));
   connect(this, SIGNAL(killed()), tleap, SLOT(kill()));

   // Find tleap executable
   QString AmberDirectory = Preferences::AmberDirectory();
   QString program = QDir(AmberDirectory).filePath("bin/tleap");
   qDebug () << "tleap executable location: " << program;

   // Set working directory
   tleap->setWorkingDirectory(m_system->getFile().absolutePath());
   qDebug() << "tleap working directory: " << m_system->getFile().absolutePath();

   // Write the input PDB file
   m_system->exportPdb();

   // Write input for tleap
   QFile file(QDir(m_system->getFile().absolutePath()).filePath("tleap.in"));
   if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
      return;
   QTextStream out(&file);
   QString input = m_dialog.inputTextEdit->toPlainText() + "\nquit\n";
   out << input.toUtf8().constData();
   file.close();

   // Build arguments
   QStringList arguments;
   arguments << "-f" << "tleap.in";
   qDebug() << "Arguments: " << arguments;

   tleap->start(program, arguments);
}

void SystemBuilderDialog::tleapFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
   QProcess* tleap(qobject_cast<QProcess*>(sender()));
   qDebug() << "tleap finished with exit code: " << exitCode;
   tleap->deleteLater();

   if (exitStatus == QProcess::NormalExit && exitCode == 0) {
      qDebug() << "tleap finished";
      m_dialog.outputTextEdit->appendPlainText("tleap finished normally.\n");
   } else {
      qDebug() << "tleap crashed";
      m_dialog.outputTextEdit->appendPlainText("tleap crashed.\n");
   }
   finished();
}

void SystemBuilderDialog::on_sourceAddButton_clicked()
{
   QListWidgetItem *item = m_dialog.sourceAvailList->takeItem(m_dialog.sourceAvailList->currentRow());
   m_dialog.sourceSelectedList->addItem(item);
   updateTleapInput();
}

void SystemBuilderDialog::on_sourceRemoveButton_clicked()
{
   QListWidgetItem *item = m_dialog.sourceSelectedList->takeItem(m_dialog.sourceSelectedList->currentRow());
   m_dialog.sourceAvailList->addItem(item);
   updateTleapInput();
}

void SystemBuilderDialog::on_ion1ComboBox_currentIndexChanged(int index)
{
   QList<QString> const positiveIonsList = {"Na+", "K+"};
   QList<QString> const negativeIonsList = {"Cl-"};

   if (positiveIonsList.contains(m_dialog.ion1ComboBox->itemText(index))) {
      m_dialog.ion2ComboBox->clear();
      m_dialog.ion2ComboBox->addItem("");
      for (const QString &ion : negativeIonsList) {
         m_dialog.ion2ComboBox->addItem(ion);
      }
   } else {
      m_dialog.ion2ComboBox->clear();
      m_dialog.ion2ComboBox->addItem("");
      for (const QString &ion : positiveIonsList) {
         m_dialog.ion2ComboBox->addItem(ion);
      }
   }
}

} } // end namespace IQmol::Amber
