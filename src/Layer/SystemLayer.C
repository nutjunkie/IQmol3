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

   connect(newAction("Box System"), SIGNAL(triggered()),
      this, SLOT(boxSystem()));
}


void System::appendData(Data::Bank& bank)
{  
   Factory& factory(Factory::instance());

   // This is the QM molecule
   Molecule* molecule(0);

   for (auto iter = bank.begin(); iter != bank.end(); ++iter) {
       Layer::List list(factory.toLayers(**iter));

       qDebug() << "Processing Layer:" << Data::Type::toString((*iter)->typeID());

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
             molecule = new Molecule();
             Data::Geometry* geom = dynamic_cast<Data::Geometry*>(*iter);
             molecule->setText("Molecule");
             if (geom)  molecule->setText(geom->name());
             molecule->appendData(list);
             appendLayer(molecule);
          }; break;
             
          default:
             if (!molecule) {
                 molecule = new Molecule();
                 molecule->setText("Molecule");
                 appendLayer(molecule);
             }
             molecule->appendData(list);
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
       qDebug() << "Appending System data" << text;
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
qDebug() << "Appending octree layer";
    connect(m_octree, SIGNAL(updated()), this, SIGNAL(updated()));
    appendLayer(m_octree);
    updated();
qDebug() << "Octree layer appended";
}

} } // end namespace IQmol::Layer
