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

#include "UndoCommands.h"
#include "Layer/MoleculeLayer.h"
#include "Layer/ComponentLayer.h"
#include "Layer/SystemLayer.h"
#include "Layer/AtomLayer.h"
#include "Layer/BondLayer.h"
#include "Layer/GroupLayer.h"
#include "QsLog.h"
#include "Layer/ConstraintLayer.h"
#include "QVariantPtr.h"


using namespace qglviewer;

namespace IQmol {
namespace Command {


// --------------- AddHydrogens ---------------
AddHydrogens::AddHydrogens(
   Layer::Molecule* molecule, 
   Layer::PrimitiveList const& primitives)
 : QUndoCommand("Add hydrogens"), m_molecule(molecule), m_primitives(primitives)
{
   Layer::Atom *begin, *end, *atom;
   Layer::Bond *bond;
   Vec finalPosition;
   Vec bit(0.0, 0.0, 0.000001);  // avoids normalizing a null vector
   int index;

   AtomList hydrogens;
   BondList bonds;

   Layer::PrimitiveList::iterator iter;
   for (iter = m_primitives.begin(); iter != m_primitives.end(); ++iter) {
       if ( (bond = qobject_cast<Layer::Bond*>(*iter)) ) {
          bonds.append(bond);
       }else if  ( (atom = qobject_cast<Layer::Atom*>(*iter)) ) {
          hydrogens.append(atom);
       }
   }

   BondList::iterator bondIter;
   for (bondIter = bonds.begin(); bondIter != bonds.end(); ++bondIter) {
       begin = (*bondIter)->beginAtom();
       end   = (*bondIter)->endAtom();

       index = hydrogens.indexOf(begin);
       if (index >= 0) {
          finalPosition = hydrogens[index]->getPosition();
          hydrogens[index]->setPosition(end->getPosition() + bit);
          m_animatorList.append(new Animator::Move(hydrogens[index], finalPosition));
       }

       index = hydrogens.indexOf(end);
       if (index >= 0) {
          finalPosition = hydrogens[index]->getPosition();
          hydrogens[index]->setPosition(begin->getPosition() + bit);
          m_animatorList.append(new Animator::Move(hydrogens[index], finalPosition));
       }
   }
}


AddHydrogens::~AddHydrogens()
{
   if (m_molecule) m_molecule->popAnimators(m_animatorList); 
   AnimatorList::iterator iter;
   for (iter = m_animatorList.begin(); iter != m_animatorList.end(); ++iter) {
       delete (*iter);
   }
}


void AddHydrogens::redo()
{
   AnimatorList::iterator iter;
   for (iter = m_animatorList.begin(); iter != m_animatorList.end(); ++iter) {
       (*iter)->reset();
   }
   m_molecule->appendPrimitives(m_primitives);
   m_molecule->pushAnimators(m_animatorList); 
   m_molecule->updated();
}


void AddHydrogens::undo()
{
   m_molecule->takePrimitives(m_primitives);
   m_molecule->updated();
}



// --------------- EditPrimitives ---------------
EditPrimitives::~EditPrimitives()
{
   Layer::PrimitiveList::iterator iter;
   if (m_deleteRemoved) {
      for (iter = m_removed.begin(); iter != m_removed.end(); ++iter) delete *iter;
   }else {
      for (iter = m_added.begin(); iter != m_added.end(); ++iter) delete *iter;
   }
}


void EditPrimitives::redo()
{
   m_deleteRemoved = true;
   m_molecule->takePrimitives(m_removed);
   m_molecule->appendPrimitives(m_added);
   m_molecule->updated();
}


void EditPrimitives::undo()
{
   m_deleteRemoved = false;
   m_molecule->takePrimitives(m_added);
   m_molecule->appendPrimitives(m_removed);
   m_molecule->updated();
}



// --------------- MoveObjects ---------------
MoveObjects::MoveObjects(Layer::Component* component, QString const& text, bool const animate,
   bool const invalidateSymmetry) : QUndoCommand(text), m_component(component), 
   m_finalStateSaved(false), m_animate(animate), m_invalidateSymmetry(invalidateSymmetry)
{ 
   m_objectList = m_component->findLayers<Layer::GLObject>(Layer::Children);
   saveFrames(m_initialFrames);
}


MoveObjects::MoveObjects(GLObjectList const& objectList, QString const& text, 
   bool const animate, bool const invalidateSymmetry) : QUndoCommand(text), m_component(0),
   m_objectList(objectList), m_finalStateSaved(false), m_animate(animate), 
   m_invalidateSymmetry(invalidateSymmetry)
{ 
   // Need a Component handle for the Viewer update (yugh)
   ComponentList parents;
   int i(0);

   while (!m_component && i < m_objectList.size()) {
      parents = m_objectList[i]->findLayers<Layer::Component>(Layer::Parents);
      if (!parents.isEmpty()) m_component = parents.first();
      ++i;
   }

   if (!m_component) { QLOG_ERROR() << "MoveObjects constructor called with no Component"; }
   saveFrames(m_initialFrames);
}


MoveObjects::~MoveObjects()
{
   if (m_component) m_component->popAnimators(m_animatorList); 
   AnimatorList::iterator iter;
   for (iter = m_animatorList.begin(); iter != m_animatorList.end(); ++iter) {
       delete (*iter);
   }
}


void MoveObjects::redo() 
{
   if (!m_finalStateSaved) {
      saveFrames(m_finalFrames);
      m_finalStateSaved = true;
      if (m_animate) {   
         for (int i = 0; i < m_objectList.size(); ++i) {
             m_objectList[i]->setFrame(m_initialFrames[i]);
             m_animatorList.append(new Animator::Move(m_objectList[i],
                 m_finalFrames[i]));
         }
      }
   }

   if (m_animate && m_component) {
      AnimatorList::iterator iter;
      for (iter = m_animatorList.begin(); iter != m_animatorList.end(); ++iter) {
          (*iter)->reset();
      }
      m_component->pushAnimators(m_animatorList); 
      m_component->setReferenceFrame(m_finalFrames.last());
   }else {
      loadFrames(m_finalFrames);
   }

   if (m_component) {
      if (m_invalidateSymmetry) {
         auto molecule = dynamic_cast<Layer::Molecule*>(m_component);
         if (molecule) molecule->invalidateSymmetry();
         molecule->autoDetectSymmetry();
      }
      m_component->postMessage(m_msg);
      m_component->updated();
   }
}


void MoveObjects::undo() 
{
   loadFrames(m_initialFrames);
   if (m_component) {
      m_component->postMessage("");
      if (m_invalidateSymmetry) {
         auto molecule = dynamic_cast<Layer::Molecule*>(m_component);
         if (molecule) molecule->invalidateSymmetry();
         molecule->autoDetectSymmetry();
      }
      m_component->updated();
   }
}


void MoveObjects::saveFrames(QList<Frame>& frames)
{
   frames.clear();
   for (int i = 0; i < m_objectList.size(); ++i) {
       frames.append(m_objectList[i]->getFrame());
   }
   if (m_component) frames.append(m_component->getReferenceFrame());
}


void MoveObjects::loadFrames(QList<Frame> const& frames)
{
   for (int i = 0; i < m_objectList.size(); ++i) {
       m_objectList[i]->setFrame(frames[i]);
   }
   if (m_component) m_component->setReferenceFrame(frames.last());
}



// --------------- AddConstraint ---------------
AddConstraint::AddConstraint(Layer::Molecule* molecule, Layer::Constraint* constraint)
   : QUndoCommand("Add constraint"), m_deleteConstraint(false), m_molecule(molecule),
     m_constraint(constraint) 
{ 
   if (constraint->scanConstraint()) setText("Add scan coordinated");
}


AddConstraint::~AddConstraint()
{
   if (m_deleteConstraint && m_constraint) {
      delete m_constraint;
      m_constraint = 0;
   }
}


void AddConstraint::undo()
{
   m_molecule->removeConstraintLayer(m_constraint);
   m_deleteConstraint = true;
}


void AddConstraint::redo()
{
   m_molecule->addConstraintLayer(m_constraint);
   m_deleteConstraint = false;
}



// --------------- ChangeAtomType ---------------
ChangeAtomType::ChangeAtomType(Layer::Atom* atom) : QUndoCommand("Change atom type"),
   m_atom(atom), m_finalStateSaved(false), m_molecule(0)
{
   MoleculeList parents(atom->findLayers<Layer::Molecule>(Layer::Parents));
   //MoleculeList parents(atom->findParents<Layer::Molecule>());
   if (parents.size() != 1) {
      QLOG_ERROR() << "Could not determine parent Molecule";
   }else {
      m_molecule = parents.first();
   }
   m_oldAtomicNumber = m_atom->getAtomicNumber();
}


void ChangeAtomType::redo()
{
   if (m_finalStateSaved) {
      m_atom->setAtomicNumber(m_newAtomicNumber);
   }else {
      m_newAtomicNumber = m_atom->getAtomicNumber();
      m_finalStateSaved = true;
   }

   if (m_molecule) {
      m_molecule->updateInfo();
      m_molecule->softUpdate();
      m_molecule->reindexAtomsAndBonds();
      m_molecule->updated();
   }
}


void ChangeAtomType::undo()
{
   m_atom->setAtomicNumber(m_oldAtomicNumber);
   if (m_molecule) {
      m_molecule->updateInfo();
      m_molecule->softUpdate();
      m_molecule->reindexAtomsAndBonds();
      m_molecule->updated();
   }
}



// --------------- ChangeBondOrder ---------------
ChangeBondOrder::ChangeBondOrder(Layer::Bond* bond) : QUndoCommand("Change bond order"),
   m_bond(bond), m_finalStateSaved(false), m_molecule(0)
{
   // Need a Molecule handle for the Viewer update (yugh)
   MoleculeList parents(bond->findLayers<Layer::Molecule>(Layer::Parents));
   if (parents.size() != 1) {
      QLOG_ERROR() << "Could not determine parent molecule";
   }else {
      m_molecule = parents.first();
   }

   m_oldOrder = m_bond->getOrder();
}


void ChangeBondOrder::redo()
{
   if (m_finalStateSaved) {
      m_bond->setOrder(m_newOrder);
   }else {
      m_newOrder = m_bond->getOrder();
      m_finalStateSaved = true;
   }
   if (m_molecule) m_molecule->softUpdate();
}


void ChangeBondOrder::undo()
{
   m_bond->setOrder(m_oldOrder);
   if (m_molecule) m_molecule->softUpdate();
}


// --------------- AddComponent ---------------
AddComponent::AddComponent(Layer::Component* component, QStandardItem* parent) 
   : m_component(component), m_parent(parent), m_deleteComponent(false)
{ 
   if (!m_parent) {
      QLOG_WARN() << "No parent found for layer in AddComponent:" << component->text();
   }

   if (m_component->fileName().isEmpty()) {
      Layer::Molecule* molecule;
      if ( (molecule = qobject_cast<Layer::Molecule*>(component)) ) {
         setText("New molecule");
      }else {
         setText("New system");
      }
   }else {
      setText("Load file " + m_component->fileName());
   }
}


AddComponent::~AddComponent()
{
   if (m_deleteComponent) {
      QLOG_DEBUG() << "Deleting component" << m_component->text() << m_component;
      // The following causes a crash
      // delete m_component;  
   }
}


void AddComponent::redo()
{
   m_deleteComponent = false;
   QLOG_INFO() << "Adding " << m_component->text() << m_component 
               << "with parent" << m_parent;
   m_parent->appendRow(m_component);
   m_component->updated();
}


void AddComponent::undo()
{
   m_deleteComponent = true;
   QLOG_INFO() << "Removing " << m_component->text() << m_component;
   m_parent->takeRow(m_component->row());
   m_component->updated();
}


// --------------- RemoveComponent ---------------
RemoveComponent::RemoveComponent(Layer::Component* component, QStandardItem* parent)
   : m_component(component), m_parent(parent), m_deleteComponent(false)
{ 
   if (m_parent == 0) {
      QLOG_WARN() << "No parent found for layer in RemoveComponent:" << component->text();
   }

   if (m_component->fileName().isEmpty()) {
      Layer::Molecule* molecule;
      if ( (molecule = qobject_cast<Layer::Molecule*>(component)) ) {
         setText("Remove molecule");
      }else {
         setText("Remove system");
      }
   }else {
      setText("Remove " + m_component->fileName());
   }
}


RemoveComponent::~RemoveComponent()
{
   if (m_deleteComponent) {
      m_component->disconnect();
      // The following causes a crash
      //delete m_component;  
   }
}


void RemoveComponent::redo()
{
   m_deleteComponent = true;
   m_parent->takeRow(m_component->row());
   m_component->updated();
}


void RemoveComponent::undo()
{
   m_deleteComponent = false;
   QLOG_INFO() << "Adding component" << m_component->text() << m_component;
   m_parent->appendRow(m_component);
   m_component->updated();
}


// --------------- AppendData ---------------
void AppendData::redo()
{
   m_molecule->appendData(m_dataList);
   m_molecule->updated();
}

void AppendData::undo()
{
   // why undo this?
   //m_molecule->removeData(m_dataList);
   m_molecule->updated();
}



// --------------- RemoveData ---------------
void RemoveData::redo()
{
   qDebug() << "RemoveData::redo() cannot remove data";
   //m_molecule->removeData(m_dataList);
   m_molecule->updated();
}

void RemoveData::undo()
{
   m_molecule->appendData(m_dataList);
   m_molecule->updated();
}




/*
// --------------- GroupPrimitives ---------------
void GroupPrimitives::redo()
{
qDebug() << "Group Primitives 0";
   m_molecule->takePrimitives(m_primitiveList);
qDebug() << "Group Primitives 1";
   m_fragment << new Layer::Fragment(m_primitiveList);
qDebug() << "Group Primitives 2";
   m_molecule->appendPrimitives(m_fragment);
qDebug() << "Group Primitives 3 - finished";
}


void GroupPrimitives::undo()
{
qDebug() << "ungroup Primitives 0";
   Layer::Fragment* fragment = qobject_cast<Layer::Fragment*>(m_fragment.first());
qDebug() << "ungroup Primitives 1";
   m_molecule->takePrimitives(m_fragment);
qDebug() << "ungroup Primitives 2";
   m_molecule->appendPrimitives(fragment->ungroup());
qDebug() << "ungroup Primitives 3";
   //delete fragment;
   m_fragment.clear();
qDebug() << "ungroup Primitives end";
}


// --------------- UnroupPrimitives ---------------
void UngroupFragments::redo()
{
   m_molecule->takePrimitives(m_fragmentList);
   // At this point the Molecule has given us a list of only the top most
   // fragments
   FragmentList::iterator iter;
   QStandardItem* parent
   for (iter = m_fragmentList.begin(); iter != fragment.end(); ++iter) {
       parent parent((*iter)->parent());
       
   }

   m_molecule->appendPrimitives(fragment->ungroup());
   m_molecule->appendPrimitives(m_fragment);
   delete fragment;
   m_fragment.clear();
}


void UngroupFragments::undo()
{
   Layer::Fragment* fragment = qobject_cast<Layer::Fragment*>(m_fragment.first());
   m_molecule->takePrimitives(m_fragment);
   m_fragment << new Layer::Fragment(m_primitiveList);
}

*/

} } // end namespace IQmol::Command

