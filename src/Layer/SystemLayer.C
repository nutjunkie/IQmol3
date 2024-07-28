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

#include "SystemLayer.h"
#include "ProteinChainLayer.h"
#include "SolventLayer.h"
#include "MoleculeLayer.h"
#include "GroupLayer.h"
#include "LayerFactory.h"
#include "FileLayer.h"

#include "Data/Bank.h"
#include "Viewer/UndoCommands.h"
#include "Util/QsLog.h"
#include "Util/QMsgBox.h"
#include "Util/Preferences.h"
#include "Process/JobInfo.h"

#include <QtDebug>
#include <QFileInfo>
#include <QDir>


namespace IQmol {
namespace Layer {


System::System(QString const& label, QObject* parent) : Component(label, parent), 
   m_octree(0)
{
   setFlags( Qt::ItemIsEnabled 
            | Qt::ItemIsEditable
            | Qt::ItemIsSelectable 
            | Qt::ItemIsDropEnabled  
            | Qt::ItemIsUserCheckable );
   setCheckState(Qt::Checked);

   m_surfaceList.setText("Ribbons");

   connect(newAction("Box System"), SIGNAL(triggered()), this, SLOT(boxSystem()));
   connect(newAction("Export PDB"), SIGNAL(triggered()), this, SLOT(exportPdb()));
   connect(newAction("Remove System"), SIGNAL(triggered()), this, SLOT(removeSystem()));
}


void System::appendData(Data::Bank& bank)
{  
   Factory& factory(Factory::instance());

   // This is the QM molecule
   Molecule* molecule(0);

   for (auto iter = bank.begin(); iter != bank.end(); ++iter) {
       Layer::List list(factory.toLayers(**iter));

       switch ((*iter)->typeID()) {
          case Data::Type::MacroMolecule: 
          case Data::Type::ProteinChain: 
          case Data::Type::Solvent: 
          case Data::Type::Pdb: 
          case Data::Type::FileList: 
             for (auto layer : list) {
                 connect(layer, SIGNAL(updated()), this, SIGNAL(updated()));
                 appendLayer(layer);
             }
             break;

          case Data::Type::Geometry: {
             molecule = new Molecule(this);
             Data::Geometry* geom = dynamic_cast<Data::Geometry*>(*iter);
             molecule->setText("Molecule");
             if (geom)  molecule->setText(geom->name());
             molecule->appendData(list);
             appendLayer(molecule);
             connectComponent(molecule);
             connect(molecule, SIGNAL(removeMolecule(Layer::Molecule*)),
                this, SIGNAL(removeMolecule(Layer::Molecule*)));
               
          }; break;
             
          default:
             if (!molecule) {
                 molecule = new Molecule(this);
                 molecule->setText("Molecule");
                 appendLayer(molecule);
             }
             molecule->appendData(list);
             connectComponent(molecule);
             connect(molecule, SIGNAL(removeMolecule(Layer::Molecule*)),
                this, SIGNAL(removeMolecule(Layer::Molecule*)));
       }
   }
}


void System::appendData(Layer::List& list)
{
   // !!! This needs fixing !!!
   // This is a bit cheesy, we rely on the QStandardItem text 
   // to determine the type of Layer.  
   QList<Base*> currentLayers(findLayers<Base>(Children));

   QStringList labels;
   labels << "Info";
   for (auto base = currentLayers.begin(); base != currentLayers.end(); ++base) {
       labels << (*base)->text();
   }

   QString text;
   for (auto iter = list.begin(); iter != list.end(); ++iter) {
       text = (*iter)->text();
   }
}


double System::radius()
{
   double r(0);

   ComponentList components(findLayers<Component>());
   for (auto& c: components) r = std::max(r, c->radius());

   return r;
}


void System::boxSystem()
{
    AtomList atoms = findLayers<Atom>(); 
    GroupList groups = findLayers<Group>(); 
    qDebug() << "Found" << atoms.size() << "atoms";
    qDebug() << "Found" << groups.size() << "groups";
    for (auto group : groups) atoms += group->getAtoms();
    qDebug() << "Found" << atoms.size() << "atoms";

    if (m_octree) {
       removeLayer(m_octree);
       delete m_octree;
    }
    m_octree = new Octree(atoms);

    connect(this, SIGNAL(selectionChanged()), m_octree, SLOT(selectionChanged()));
    connect(m_octree, SIGNAL(updated()), this, SIGNAL(updated()));

    connect(m_octree, SIGNAL(newMoleculeRequested(AtomList const&)), 
       this, SIGNAL(newMoleculeRequested(AtomList const&)));

    appendLayer(m_octree);
    updated();
}



void System::exportPdb()
{
    QString path(m_inputFile.path());
    path += QDir::separator() + m_inputFile.baseName();
    path += "_iqmol." + m_inputFile.completeSuffix();

    qDebug() << "Writing PDB format to " << path;
    QFile pdbFile(path);

    if (!pdbFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
       QMsgBox::critical(0, "IQmol",  "Failed to open PDB file for writing");
       return;
    }

    QTextStream os(&pdbFile);

    QList<ProteinChain*> chains(findLayers<ProteinChain>());
    unsigned index(1);
    QString line;
    QString res;
    QString resIndex;
    QString chainId;

    for (auto chain : chains) {
        QList<Group*> residues(chain->findLayers<Group>());
        QStringList tokens(chain->text().split(' '));
        if (tokens.size() != 2) {
           chainId = " ";
        }else {
           chainId = tokens[1];
        }
  
        for (auto residue : residues) {
            AtomList atoms(residue->getAtoms());
            QStringList tokens(residue->text().split(' '));
            if (tokens.size() != 2) {
               QLOG_ERROR() << "Invalid name for residue:" << residue->text();
               return;
            }

            resIndex = tokens[0];
            res = tokens[1].toUpper();
            
            for (auto atom : atoms) {
               qglviewer::Vec pos(atom->getPosition());

               QString label(atom->getLabel());
               QString symbol(atom->getAtomicSymbol());
               if (symbol.length() == 1) label.prepend(' ');
               while (label.length() < 4) label += " ";

//ATG: This should be the first 2 character are the atomic symbol, second 2 are a label
               line = QString("%1%2%3%4%5%6%7%8%9%10%11%12%13%14%15%16%17%18\n")
                  .arg("ATOM"        ,-6)
                  .arg(index++       , 5)
                  .arg(' '           , 1)
                  .arg(label         , 4)
                  .arg(' '           , 1)
                  .arg(res           , 3)
                  .arg(' '           , 1)
                  .arg(chainId       , 1)
                  .arg(resIndex      , 4)
                  .arg(' '           , 4)
                  .arg(pos.x, 8, 'f' , 3)
                  .arg(pos.y, 8, 'f' , 3)
                  .arg(pos.z, 8, 'f' , 3)
                  .arg(1.0,   6, 'f' , 2)
                  .arg(0.0,   6, 'f' , 2)
                  .arg(' '           ,10)
                  .arg(symbol, 2)
                  .arg(atom->getFormalCharge(), 2);

               os << line;
           }
        }

        line = QString("%1%2%3%4%5%6%7\n")
           .arg("TER", -6)
           .arg(index++         , 5)
           .arg(' '             , 6)
           .arg(res             , 3)
           .arg(' '             , 1)
           .arg(chainId         , 1)
           .arg(resIndex        , 4);
           
        os << line;
        
    }
/* 
- Parser
   - Fix the parser to only read in the first occupancy (10th column)
   - Add thermal temperature factor (11th column)
   - relabel the new hydrogens to something more meaningful

   - need a pdb ordering index as well as what is written in the pdb
ATOM   1695  CD  PRO A 215       7.230   9.064   2.229  1.00 53.73           C    
TER    1696      PRO A 215                               
HETATM 1697 MG    MG A 300      -5.179 -10.868 -13.937  1.00 31.08          MG   
HETATM 1698  K     K A 303      11.371  -3.739  -0.291  1.00 39.45           K    
HETATM 1699  N   SAM A 301      -9.436  -7.062 -12.623  1.00 30.55           N   
  eg. 3bwm has MG 300, then

check sorting of residue from parser, shouldn't be done
make ordering of residues consistent with pdb file, not index

- sort chain ids

- export charges from group layer to QChem input file
- Filter A/B column for alternatives
*/

    QList<Molecule*> molecules(findLayers<Molecule>());

    for (auto mol : molecules) {
        QStringList tokens(mol->text().split(' '));
        if (tokens.size() != 3) {
           QLOG_ERROR() << "Invalid name for Ligand:" << mol->text();
           return;
        }

        AtomList atoms(mol->findLayers<Atom>());

        QString res(tokens[0]);
        resIndex = tokens[1];
        chainId = tokens[2];
        chainId.replace("(", "");
        chainId.replace(")", "");

        for (auto atom : atoms) {
            QString label(atom->getLabel());
            QString symbol(atom->getAtomicSymbol());
            if (symbol.length() == 1) label.prepend(' ');
            while (label.length() < 4) label += " ";


            qglviewer::Vec pos(atom->getPosition());
            line = QString("%1%2%3%4%5%6%7%8%9%10%11%12%13%14%15%16%17%18\n")
               .arg("HETATM"      , 6)
               .arg(index++       , 5)
               .arg(' '           , 1)
               .arg(label         , 4)
               .arg(' '           , 1)
               .arg(res           , 3)
               .arg(' '           , 1)
               .arg(chainId       , 1)
               .arg(resIndex      , 4)
               .arg(' '           , 4)
               .arg(pos.x, 8, 'f' , 3)
               .arg(pos.y, 8, 'f' , 3)
               .arg(pos.z, 8, 'f' , 3)
               .arg(1.0,   6, 'f' , 2)
               .arg(0.0,   6, 'f' , 2)
               .arg(' '           ,10)
               .arg(symbol, 2)
               .arg(atom->getFormalCharge(), 2);

            os << line;
        }
    }

    QList<Solvent*> solvent(findLayers<Solvent>());

    int idx = resIndex.toInt();
    res = "HOH";

    for (auto sol : solvent) {

        std::vector<qglviewer::Vec> centers(sol->centers());

        for (auto pos : centers) {
            line = QString("%1%2%3%4%5%6%7%8%9%10%11%12%13%14%15%16%17%18\n")
               .arg("HETATM"      , 6)
               .arg(index++       , 5)
               .arg(" "           , 1)
               .arg(" O"          ,-4)
               .arg(" "           , 1)
               .arg(res           , 3)
               .arg(" "           , 1)
               .arg(chainId       , 1)
               .arg(idx           , 4)
               .arg(" "           , 4)
               .arg(pos.x, 8, 'f' , 3)
               .arg(pos.y, 8, 'f' , 3)
               .arg(pos.z, 8, 'f' , 3)
               .arg(1.0,   6, 'f' , 2)
               .arg(0.0,   6, 'f' , 2)
               .arg(" "           ,10)
               .arg("O"           , 2)
               .arg(0             , 2);

            os << line;
        }
    }

    os << "END\n";
    pdbFile.close();
}


Process::JobInfo System::qchemJobInfo()
{
   Process::JobInfo jobInfo;

   QList<Molecule*> molecules(findLayers<Molecule>());

   QString coordinates;
   QString externalCharges;
   int totalCharge(0);
   int numberOfElectrons(0);

   for (auto mol : molecules) {
       coordinates += mol->coordinatesAsString() + "\n";
       totalCharge += mol->totalCharge();
       numberOfElectrons += mol->totalCharge();
   }

   QList<ProteinChain*> chains(findLayers<ProteinChain>());

   for (auto chain : chains) {
       QList<Group*> residues(chain->findLayers<Group>());
       for (auto res : residues) {
            AtomList atoms(res->getAtoms());
            for (auto atom : atoms) {
                
                qglviewer::Vec pos(atom->getPosition());
                double q(atom->getCharge());
                externalCharges += QString("%1%2%3%4\n")
                  .arg(pos.x, 8, 'f' , 3)
                  .arg(pos.y, 8, 'f' , 3)
                  .arg(pos.z, 8, 'f' , 3)
                  .arg(q,     8, 'f' , 3);
            }
       }
   }

   jobInfo.set("Coordinates",   coordinates);
   jobInfo.set("Charge",        totalCharge);
   jobInfo.set("NumElectrons",  numberOfElectrons);
   jobInfo.set("Multiplicity",  1+(numberOfElectrons%2)); // hack, assume low spin
   jobInfo.set("LocalFilesExist", false);
   jobInfo.set("PromptOnOverwrite", true);
   jobInfo.set("ExternalCharges", externalCharges);

   QString name;

   if (m_inputFile.filePath().isEmpty()) {
      QFileInfo fileInfo(Preferences::LastFileAccessed());
      jobInfo.set("LocalWorkingDirectory", fileInfo.path());
      jobInfo.set("BaseName",text());
   }else {
      jobInfo.set("LocalWorkingDirectory", m_inputFile.path());
      jobInfo.set("BaseName", m_inputFile.completeBaseName());
      name =  + "/" + m_inputFile.completeBaseName() + ".inp";
   }

   //jobInfo.set("MoleculePointer", (qint64)this);
   jobInfo.set("SystemPointer", (qint64)this);

   // input file format
   FileList fileList(findLayers<File>(Children));
   FileList::iterator file;
   for (file = fileList.begin(); file != fileList.end(); ++file) {
       if ((*file)->fileName().endsWith(".inp")) {
          jobInfo.set("InputFileTemplate", (*file)->contents());
          break;
       }
   }

   return jobInfo;
}

} } // end namespace IQmol::Layer
