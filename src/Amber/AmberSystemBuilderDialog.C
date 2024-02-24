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

#include "AmberSystemBuilderDialog.h"
#include "Layer/SystemLayer.h"
#include "Layer/MoleculeLayer.h"
#include "Layer/AtomLayer.h"
#include "Util/QMsgBox.h"
#include "Util/Preferences.h"
#include "Amber/AmberConfigDialog.h"

#include <QDir>
#include <QFileDialog>

namespace IQmol {
namespace Amber { 


SystemBuilderDialog::SystemBuilderDialog(QWidget* parent,
   Layer::System* system) : QDialog(parent), m_system(system)
{
   m_dialog.setupUi(this);

   QString AmberDirectory = Preferences::AmberDirectory();
   if (AmberDirectory.isEmpty()) {
      QMessageBox::warning(this, tr("Amber directory not set"),
         tr("The Amber directory is not set. Please set it in the Edit Amber Config dialog."));
      Amber::AmberConfigDialog dialog(this);
      dialog.exec();
      if (dialog.result() == QDialog::Accepted) {
         Preferences::AmberDirectory(dialog.getDirectory());
      }
      AmberDirectory = Preferences::AmberDirectory();
   }

   QDir dir(AmberDirectory + "/dat/leap/cmd");
   QList<QString> defaultSourceFiles = {"leaprc.protein.ff14SB", "leaprc.water.tip3p", "leaprc.gaff2"};

   for (const QString &filename : dir.entryList(QDir::Files))
      if (defaultSourceFiles.contains(filename)) {
         m_dialog.sourceSelectedList->addItem(filename);
      } else {
         m_dialog.sourceAvailList->addItem(filename);
      }

   MoleculeList molecules(m_system->findLayers<Layer::Molecule>(Layer::Children));
   QList<QString> names;
   for (MoleculeList::iterator it = molecules.begin(); it != molecules.end(); ++it) {
      AtomList atoms((*it)->findLayers<Layer::Atom>(Layer::Children));
      if (atoms.size() > 1) {
         QString name((*it)->text().split(' ').first());
         if (!names.contains(name)) {
            names.append(name);
            addMolecule(*it);
            findParameterFile(name + ".mol2");
            findParameterFile(name + ".frcmod");
         }
      }
   }

   updateTleapInput();

   QPushButton* runButton = m_dialog.buttonBox->button(QDialogButtonBox::Apply);
   runButton->setText(tr("Run"));

   connect(runButton, &QPushButton::clicked, this, &SystemBuilderDialog::runTleap);
   connect(runButton, &QPushButton::clicked, m_dialog.tabWidget, [this]() {
      m_dialog.tabWidget->setCurrentIndex(1);
   });
}

void SystemBuilderDialog::addMolecule(const Layer::Molecule* molecule)
{
   QString name(molecule->text().split(' ').first());
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
   connect(mol2ToolButton, SIGNAL(clicked()), this, SLOT(browseParameterFile()));
   connect(frcmodToolButton, SIGNAL(clicked()), this, SLOT(browseParameterFile()));
}

void SystemBuilderDialog::findParameterFile(const QString& fileName)
{
   QFileInfo fileInfo(Preferences::LastFileAccessed());
   if (!QDir(fileInfo.absolutePath()).exists(fileName)) {
      return;
   }

   QFileInfo fi(fileName);
   QString name = fi.baseName();
   QString suffix = fi.suffix();

   if (suffix == "mol2") {
      QLineEdit *lineEdit = m_dialog.parametersPage->findChild<QLineEdit *>(name + "mol2LineEdit");
      lineEdit->setText(fileName);
   } else if (suffix == "frcmod") {
      QLineEdit *lineEdit = m_dialog.parametersPage->findChild<QLineEdit *>(name + "frcmodLineEdit");
      lineEdit->setText(fileName);
   }

   updateTleapInput();
}

void SystemBuilderDialog::browseParameterFile()
{
   QToolButton *button = qobject_cast<QToolButton *>(sender());
   QString nameSuffix = button->objectName().split("ToolButton").first();
   QString suffix = nameSuffix.endsWith("mol2") ? "mol2" : "frcmod";
   QString name = nameSuffix.split(suffix).first();
   QString fileName = QFileDialog::getOpenFileName(this, tr("Open") + suffix + tr("file"), Preferences::LastFileAccessed(),
      tr("*.") + suffix);

   QFileInfo fileInfo(Preferences::LastFileAccessed());
   fileName = QDir(fileInfo.absolutePath()).relativeFilePath(fileName);

   if (!fileName.isEmpty()) {
      QLineEdit *lineEdit = m_dialog.parametersPage->findChild<QLineEdit *>(name + suffix + "LineEdit");
      lineEdit->setText(fileName);
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

   m_dialog.inputTextEdit->appendPlainText("savepdb mol " + m_system->text() + ".pdb");
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

   // connect(tleap, SIGNAL(finished(int,QProcess::ExitStatus)),
      // this, SLOT(tleapFinished(int,QProcess::ExitStatus)));

   // Find tleap executable
   QString AmberDirectory = Preferences::AmberDirectory();
   QString program = QDir(AmberDirectory).filePath("bin/tleap");
   qDebug () << "Tleap executable location: " << program;

   // Set working directory
   QFileInfo fileInfo(Preferences::LastFileAccessed());
   tleap->setWorkingDirectory(fileInfo.absolutePath());
   qDebug() << "Tleap working directory: " << fileInfo.absolutePath();

   // Write the input PDB file
   QFile pdbFile(QDir(fileInfo.absolutePath()).filePath(m_system->text() + "_iqmol.pdb"));

   // Write input for tleap
   QFile file(QDir(fileInfo.absolutePath()).filePath("tleap.in"));
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

} } // end namespace IQmol::Amber
