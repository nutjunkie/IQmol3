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
#include "MoleculeLayer.h"
#include "GroupLayer.h"
#include "LayerFactory.h"

#include "Data/Bank.h"
#include "Viewer/UndoCommands.h"

#include <QtDebug>
#include "Util/QsLog.h"


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
    QList<ProteinChain*> chains(findLayers<ProteinChain>());
    unsigned index(0);
    QString line;

    for (auto chain : chains) {
        QList<Group*> residues(chain->findLayers<Group>());
        QStringList tokens(chain->text().split(' '));
        if (tokens.size() != 2) {
           QLOG_ERROR() << "Invalid name for Protein chain:" << chain->text();
           return;
        }

        QString chainId(tokens[1]);
  
        for (auto residue : residues) {
            AtomList atoms(residue->getAtoms());
            QStringList tokens(residue->text().split(' '));
            if (tokens.size() != 2) {
               QLOG_ERROR() << "Invalid name for residue:" << residue->text();
               return;
            }

            QString resIndex(tokens[0]);
            QString res(tokens[1].toUpper());
            
            for (auto atom : atoms) {
               qglviewer::Vec pos(atom->getPosition());
               line = QString("%1%2%3%4%5%6%7%8%9%10%11%12%13%14%15%16%17%18")
                  .arg("ATOM"          ,-6)
                  .arg(index++         , 5)
                  .arg(' '             , 1)
                  .arg(atom->getLabel(), 4)
                  .arg(' '             , 1)
                  .arg(res             , 3)
                  .arg(' '             , 1)
                  .arg(chainId         , 1)
                  .arg(resIndex        , 4)
                  .arg(' '             , 4)
                  .arg(pos.x, 8, 'f'   , 3)
                  .arg(pos.y, 8, 'f'   , 3)
                  .arg(pos.z, 8, 'f'   , 3)
                  .arg(0.0,   6, 'f'   , 2)
                  .arg(0.0,   6, 'f'   , 2)
                  .arg(' '             ,10)
                  .arg(atom->getAtomicSymbol(), 2)
                  .arg(atom->getFormalCharge(), 2);

               qDebug() << line;
           }
        }
        
    }
/*       1         2         3         4         5         6         7
12345678901234567890123456789012345678901234567890123456789012345678901234567890
L.....L....L
HETATM 4530  CBA HEM D 148     -10.405 -12.971 -22.194  1.00 50.08           C
HETATM 4562 FE   HEM D 148      -9.504  -9.265 -17.387  1.00 15.46          FE
HETATM 4515  CHA HEM D 148      -9.813  -9.884 -20.599  0.00  0.00           C 0
*/

    QList<Molecule*> molecules(findLayers<Molecule>());

    for (auto mol : molecules) {
        //qDebug() << "Found molecule" << mol->text();
            AtomList atoms(mol->findLayers<Atom>());
        QStringList tokens(mol->text().split(' '));
        if (tokens.size() != 3) {
           QLOG_ERROR() << "Invalid name for Ligand:" << mol->text();
           return;
        }

        QString res(tokens[0]);
        QString resIndex(tokens[1]);
        QString chainId(tokens[2]);
        chainId.replace("(", "");
        chainId.replace(")", "");

        for (auto atom : atoms) {
            qglviewer::Vec pos(atom->getPosition());
            line = QString("%1%2%3%4%5%6%7%8%9%10%11%12%13%14%15%16%17%18")
               .arg("HETATM"        , 6)
               .arg(index++         , 5)
               .arg(' '             , 1)
               .arg(atom->getLabel(), 4)
               .arg(' '             , 1)
               .arg(res             , 3)
               .arg(' '             , 1)
               .arg(chainId         , 1)
               .arg(resIndex        , 4)
               .arg(' '             , 4)
               .arg(pos.x, 8, 'f'   , 3)
               .arg(pos.y, 8, 'f'   , 3)
               .arg(pos.z, 8, 'f'   , 3)
               .arg(0.0,   6, 'f'   , 2)
               .arg(0.0,   6, 'f'   , 2)
               .arg(' '             ,10)
               .arg(atom->getAtomicSymbol(), 2)
               .arg(atom->getFormalCharge(), 2);

            qDebug() << line;
        }
    }
}

} } // end namespace IQmol::Layer
