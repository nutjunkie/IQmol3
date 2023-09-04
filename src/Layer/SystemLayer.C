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
#include "MacroMoleculeLayer.h"
#include "LayerFactory.h"
#include "Viewer/UndoCommands.h"

#include <QtDebug>


namespace IQmol {
namespace Layer {


System::System(QString const& label, QObject* parent) : Base(label, parent)
{
   setFlags( Qt::ItemIsEnabled 
            | Qt::ItemIsEditable
            | Qt::ItemIsSelectable 
            | Qt::ItemIsDropEnabled  
            | Qt::ItemIsUserCheckable );
   setCheckState(Qt::Checked);
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


// This is not quite what we want when there is more than one components.
qglviewer::Vec System::center()
{
   qglviewer::Vec center;

   ComponentList list(findLayers<Component>(Children));
   for (auto component : list) {
       center += component->centerOfNuclearCharge();
   }

   if (list.size() > 0) center /= list.size();

   return center;
}
 

void System::translateToCenter(GLObjectList const& selection)
{  
   Command::MoveSystemObjects* cmd(new Command::MoveSystemObjects(this, "Translate"));
   
   // The ordering here is important!!
   Atom* atom;
   AtomList atomList;
   GLObjectList::const_iterator iter;
   for (iter = selection.begin(); iter != selection.end(); ++iter) {
       if ( (atom = qobject_cast<Atom*>(*iter)) ) atomList.append(atom);
   }   

   qglviewer::Vec c(center());
   qDebug() << "Translating to center for system" << c.x << c.y << c.z;
   
   switch (atomList.size()) {
      case 1:
         translate(-atomList[0]->getPosition());
         break;
      case 2:
         translate(-atomList[0]->getPosition());
         alignToAxis(atomList[1]->getPosition());
         break;
      case 3:
         translate(-atomList[0]->getPosition());
         alignToAxis(atomList[1]->getPosition());
         rotateIntoPlane(atomList[2]->getPosition());
         break;
      default:
         translate(-center());
         break;
   }   
   
   postCommand(cmd);
   softUpdate();
}


void System::translate(qglviewer::Vec const& displacement)
{
qDebug() << "Translating system to " 
         << displacement.x << " " << displacement.y << " " << displacement.z;
   ComponentList list(findLayers<Component>(Children));
   for (auto component : list) component->translate(displacement);
}


void System::rotate(qglviewer::Quaternion const& rotation)
{
   ComponentList list(findLayers<Component>(Children));
   for (auto component : list) component->rotate(rotation);
}


void System::alignToAxis(qglviewer::Vec const& point, qglviewer::Vec const& axis)
{
   ComponentList list(findLayers<Component>(Children));
   for (auto component : list) component->alignToAxis(point, axis);
}


void System::rotateIntoPlane(qglviewer::Vec const& pt, qglviewer::Vec const& axis,
   qglviewer::Vec const& normal)
{
   ComponentList list(findLayers<Component>(Children));
   for (auto component : list) component->rotateIntoPlane(pt, axis, normal);
}

} } // end namespace IQmol::Layer
