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

#include "Data/Bank.h"
#include "Viewer/UndoCommands.h"
#include "Util/QsLog.h"
#include "Util/QMsgBox.h"

#include <QtDebug>
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
           QLOG_ERROR() << "Invalid name for Protein chain:" << chain->text();
           return;
        }

        chainId = tokens[1];
  
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
               if (label.size() < 4) label = " " + label;
               line = QString("%1%2%3%4%5%6%7%8%9%10%11%12%13%14%15%16%17%18\n")
                  .arg("ATOM"        ,-6)
                  .arg(index++       , 5)
                  .arg(' '           , 1)
                  .arg(label         ,-4)
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
                  .arg(atom->getAtomicSymbol(), 2)
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
   - Fix the parser to only read in the first occupancy 
   - Add thermal temperature factor
- relabel the new hydrogens to something more meaningfule
- add spinner https://github.com/snowwlex/QtWaitingSpinner
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
            if (label.size() < 4) label = " " + label;
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
               .arg(atom->getAtomicSymbol(), 2)
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

} } // end namespace IQmol::Layer
