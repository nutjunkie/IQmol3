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

#include "GroupLayer.h"
#include "AtomLayer.h"
#include "BondLayer.h"
#include "SystemLayer.h"
#include "Parser/ParseFile.h"
#include "LayerFactory.h"
#include "Math/Align.h"
#include "Util/QMsgBox.h"
#include "Util/QsLog.h"
#include "Viewer/UndoCommands.h"

#include "openbabel/mol.h"
#include "openbabel/atom.h"
#include "openbabel/bond.h"
#include "openbabel/obiter.h"
#include "openbabel/typer.h"

#include <QFileInfo>
#include <QtDebug>



using namespace qglviewer;
using namespace OpenBabel;

namespace IQmol {
namespace Layer {


Group::Group(PrimitiveList const& primitives, QString const& label) : Primitive(label)
{
   setFlags(Qt::ItemIsSelectable |  Qt::ItemIsEnabled | Qt::ItemIsEditable);
   addPrimitives(primitives);

   connect(newAction("Add Hydrogens"), SIGNAL(triggered()),
     this, SLOT(addHydrogens()));

   connect(newAction("Reperceive Bonds"), SIGNAL(triggered()),
     this, SLOT(reperceiveBonds()));
}



Group::~Group()
{
   AtomList::iterator atom;
   for (atom = m_atoms.begin(); atom != m_atoms.end(); ++atom) {
       delete (*atom);
   }

   BondList::iterator bond;
   for (bond = m_bonds.begin(); bond != m_bonds.end(); ++bond) {
       delete (*bond);
   }
}


void Group::dump() const
{
   qDebug() << "Dumping atom coordinates in Group:";
   AtomList::const_iterator atom;
   Vec v;
   for (atom = m_atoms.begin(); atom != m_atoms.end(); ++atom) {
       v = (*atom)->getPosition();
       qDebug() << "(x,y,z) = " << v.x << v.y << v.z;
   }
}


void Group::loadFromFile(QString const& filePath)
{
   QFileInfo info(filePath);
   QString name(info.completeBaseName());
   setText(name);

   setFlags(Qt::ItemIsSelectable |  Qt::ItemIsEnabled);

   Parser::ParseFile parser(filePath);
   parser.start();
   parser.wait();

   QStringList errors(parser.errors());
   if (!errors.isEmpty()) {
      QMsgBox::warning(0, "IQmol", errors.join("\n"));
   }

   IQmol::Data::Bank& bank(parser.data());
   Factory& factory(Factory::instance()); 
   List data(factory.toLayers(bank));
   List::iterator iter;

   for (iter = data.begin(); iter != data.end(); ++iter) {
       addAtoms((*iter)->findLayers<Atom>(Children));
       addBonds((*iter)->findLayers<Bond>(Children));
  }
}


void Group::addPrimitives(PrimitiveList const& primitives)
{
   AtomList atoms;
   BondList bonds;
   Atom* atom;
   Bond* bond;

   PrimitiveList::const_iterator primitive;
   for (primitive = primitives.begin(); primitive != primitives.end(); ++primitive) {
       if ((atom = dynamic_cast<Atom*>(*primitive))) {
          atoms.append(atom);
       }else if ((bond = dynamic_cast<Bond*>(*primitive))) {
          bonds.append(bond);
       }
   }

   addAtoms(atoms);
   addBonds(bonds);
   reperceiveBonds();
}


void Group::addAtoms(AtomList const& atoms)
{
   AtomList::const_iterator atom;
   for (atom = atoms.begin(); atom != atoms.end(); ++atom) {
       (*atom)->setReferenceFrame(&m_frame);
       (*atom)->setDrawMode(Primitive::Tubes);
       m_atoms.append(*atom);
   }
}


void Group::addBonds(BondList const& bonds)
{
   BondList::const_iterator bond;
   for (bond = bonds.begin(); bond != bonds.end(); ++bond) {
       (*bond)->setReferenceFrame(&m_frame);
       m_bonds.append(*bond);
   }
}


void Group::align(QList<Vec> const& current)
{
   AtomList atoms(getAtoms());
   int nAtoms(atoms.size());
   if (current.size() != nAtoms) {
      QMsgBox::warning(0, "IQmol", "Coordinate mismatch in Group alignment");
      return;
   }

   QList<double> weights;
   QList<Vec> reference;
   AtomList::iterator atom;
   for (atom = atoms.begin(); atom != atoms.end(); ++atom) {
       reference.append((*atom)->getTranslation());
       weights.append((*atom)->getMass());
   }

   Math::Align align(reference, current, weights);
   if (!align.computeAlignment()) {
      QMsgBox::warning(0, "IQmol", "Alignment failed in Group::align");
      return;
   }

   setRotation(align.rotation());
   setTranslation(align.translation());
}



PrimitiveList Group::ungroup()
{
   PrimitiveList primitives;

   AtomList::iterator atom;
   for (atom = m_atoms.begin(); atom != m_atoms.end(); ++atom) {
       Vec pos((*atom)->getPosition());
       (*atom)->setReferenceFrame(0);
       (*atom)->setPosition(pos);
       (*atom)->orphanLayer();
       primitives.append(*atom);
   }
   m_atoms.clear();

   BondList::iterator bond;
   for (bond = m_bonds.begin(); bond != m_bonds.end(); ++bond) {
       (*bond)->setReferenceFrame(0);
       (*bond)->orphanLayer();
       primitives.append(*bond);
   }

   m_bonds.clear();

   return primitives;
}


void Group::draw()
{
   // Only draw if selected
   if (!isSelected()) return;

   glPushMatrix();
   glMultMatrixd(m_frame.matrix());

   AtomList::iterator atom;
   for (atom = m_atoms.begin(); atom != m_atoms.end(); ++atom) {
       (*atom)->draw();    
   }

   BondList::iterator bond;
   for (bond = m_bonds.begin(); bond != m_bonds.end(); ++bond) {
       (*bond)->draw();    
   }

   glPopMatrix();
}


void Group::drawSelected()
{
   return;
   glPushMatrix();
   glMultMatrixd(m_frame.matrix());

   AtomList::iterator atom;
   for (atom = m_atoms.begin(); atom != m_atoms.end(); ++atom) {
       (*atom)->drawSelected();    
   }

   BondList::iterator bond;
   for (bond = m_bonds.begin(); bond != m_bonds.end(); ++bond) {
       (*bond)->drawSelected();    
   }

   glPopMatrix();
}


void Group::drawFast()
{
   glPushMatrix();
   glMultMatrixd(m_frame.matrix());

   AtomList::iterator atom;
   for (atom = m_atoms.begin(); atom != m_atoms.end(); ++atom) {
       (*atom)->drawFast();    
   }

   BondList::iterator bond;
   for (bond = m_bonds.begin(); bond != m_bonds.end(); ++bond) {
       (*bond)->drawFast();    
   }

   glPopMatrix();
}


void Group::select()
{
   setProperty(Selected);

   AtomList::iterator atom;
   for (atom = m_atoms.begin(); atom != m_atoms.end(); ++atom) {
       (*atom)->select();    
   }

   BondList::iterator bond;
   for (bond = m_bonds.begin(); bond != m_bonds.end(); ++bond) {
       (*bond)->select();    
   }
}


void Group::deselect()
{
   unsetProperty(Selected);

   AtomList::iterator atom;
   for (atom = m_atoms.begin(); atom != m_atoms.end(); ++atom) {
       (*atom)->deselect();    
   }

   BondList::iterator bond;
   for (bond = m_bonds.begin(); bond != m_bonds.end(); ++bond) {
       (*bond)->deselect();    
   }
}


void Group::setAtomScale(double const scale)
{
   AtomList::iterator atom;
   for (atom = m_atoms.begin(); atom != m_atoms.end(); ++atom) {
       (*atom)->setScale(scale);    
   }
}


void Group::setBondScale(double const scale)
{
   BondList::iterator bond;
   for (bond = m_bonds.begin(); bond != m_bonds.end(); ++bond) {
       (*bond)->setScale(scale);    
   }
}


void Group::setDrawMode(DrawMode const mode)
{
   AtomList::iterator atom;
   for (atom = m_atoms.begin(); atom != m_atoms.end(); ++atom) {
       (*atom)->setDrawMode(mode);    
   }

   BondList::iterator bond;
   for (bond = m_bonds.begin(); bond != m_bonds.end(); ++bond) {
       (*bond)->setDrawMode(mode);    
   }
}


Atom* Group::rootAtom() const
{
   Atom* atom(0);
   if (!m_atoms.isEmpty()) atom = m_atoms.first();
   return atom;
}


Atom* Group::createAtom(unsigned int const Z, qglviewer::Vec const& position)
{
   double atomScale(1.0);
   Atom* atom(new Atom(Z));
   atom->setDrawMode(Primitive::Tubes);
   atom->setScale(atomScale);
   atom->setSmallerHydrogens(false);
   atom->setPosition(position);
   return atom;
}


Bond* Group::createBond(Atom* begin, Atom* end, int const order)
{
   double bondScale(1.0);
   Bond* bond(new Bond(begin, end));
   bond->setOrder(order);
   bond->setDrawMode(Primitive::Tubes);
   bond->setScale(bondScale);
   return bond;
}


void Group::reperceiveBonds()
{
   AtomMap atomMap;

   OpenBabel::OBMol* obMol(new OpenBabel::OBMol());
   obMol->BeginModify();

   for (auto atomIter = m_atoms.begin(); atomIter != m_atoms.end(); ++atomIter) {
       OpenBabel::OBAtom* obAtom = obMol->NewAtom();
       atomMap.insert(obAtom, *atomIter);
       qglviewer::Vec pos = (*atomIter)->getPosition();
       int atomicNumber((*atomIter)->getAtomicNumber());
       obAtom->SetAtomicNum(atomicNumber);
       obAtom->SetVector(pos.x, pos.y, pos.z);
   }

   obMol->EndModify();
   obMol->ConnectTheDots();
   obMol->PerceiveBondOrders();

   for (auto bond : m_bonds) delete bond;
   m_bonds.clear();

   FOR_BONDS_OF_MOL(obBond, obMol) {
      Atom* begin = atomMap.value(obBond->GetBeginAtom());
      Atom* end   = atomMap.value(obBond->GetEndAtom());
      int   order = obBond->GetBondOrder();

      if (!begin || !end) {
         QString msg("Error encountered converting from OBMol object");
         QMsgBox::critical(0, "IQmol", msg);
         return;
      }

      Bond* bond(createBond(begin, end, order));
      bond->setReferenceFrame(&m_frame);
      bond->setDrawMode(Primitive::Tubes);
      m_bonds.append(bond);
   } 

   delete obMol;
}


void Group::addHydrogens()
{
   QLOG_DEBUG() << "Adding hydrogens to group";
   AtomMap atomMap;
   BondMap bondMap;
  
   OBMol* obMol(toOBMol(&atomMap, &bondMap));
   obMol->BeginModify();
   obMol->EndModify();
  
     // We now type the atoms and overwrite the valency/hybridization info
     // if it has been changed by the user.
    OBAtomTyper atomTyper;
  
    atomTyper.AssignHyb(*obMol);
    atomTyper.AssignTypes(*obMol);
  
    AtomMap::iterator iter;
    for (iter = atomMap.begin(); iter != atomMap.end(); ++iter) {
        int hybrid(iter.value()->getHybridization());
        if (hybrid > 0) iter.key()->SetHyb(hybrid);
     }
  
     obMol->AddHydrogens(false,false);
     obMol->BeginModify();
     obMol->EndModify();

     PrimitiveList primitives(fromOBMol(obMol, atomMap, bondMap));
     addPrimitives(primitives);
  
/*
     Command::AddHydrogens* cmd  = 
        new Command::AddHydrogens(this, fromOBMol(obMol, &atomMap, &bondMap));
     postCommand(cmd);
  
     delete obMol;
     m_modified = true;
     postMessage("");
*/
}


OBMol* Group::toOBMol(AtomMap* atomMap, BondMap* bondMap)
{  
   OBAtom* obAtom;
   OBBond* obBond;
   Vec     position;

   OBMol* obMol(new OBMol());
   atomMap->clear();
   bondMap->clear();
   AtomList atoms(findLayers<Atom>(Children));
   AtomList::iterator atomIter;

   obMol->BeginModify();
   obMol->SetHybridizationPerceived();

   for (atomIter = atoms.begin(); atomIter != atoms.end(); ++atomIter) {
       obAtom = obMol->NewAtom();
       atomMap->insert(obAtom, *atomIter);
       position = (*atomIter)->getPosition();
       obAtom->SetAtomicNum((*atomIter)->getAtomicNumber());
       obAtom->SetVector(position.x, position.y, position.z);
//       OBAtomAssignTypicalImplicitHydrogens(obAtom);
   }
    
   BondList bonds(findLayers<Bond>(Children));
   BondList::iterator bondIter;
   
   for (bondIter = bonds.begin(); bondIter != bonds.end(); ++bondIter) {
       unsigned hcount;
       unsigned order((*bondIter)->getOrder());
     
       obBond = obMol->NewBond();
       bondMap->insert(obBond, *bondIter);
       obBond->SetBondOrder(order);
  
       obAtom = atomMap->key((*bondIter)->beginAtom());
       hcount = obAtom->GetImplicitHCount();
       obAtom->SetImplicitHCount(order >= hcount ? 0 : hcount - order);
       obBond->SetBegin(obAtom);
       obAtom->AddBond(obBond);
   
       obAtom = atomMap->key((*bondIter)->endAtom());
       hcount = obAtom->GetImplicitHCount();
       obAtom->SetImplicitHCount(order >= hcount ? 0 : hcount - order);
       obBond->SetEnd(obAtom);
       obAtom->AddBond(obBond);
   }

   obMol->EndModify();

   return obMol;
}


PrimitiveList Group::fromOBMol(OBMol* obMol, AtomMap& atomMap, BondMap& bondMap)
{
   PrimitiveList addedPrimitives;
  
   Bond *bond;
   Atom *atom, *begin, *end;
     
   FOR_ATOMS_OF_MOL(obAtom, obMol) {
      Vec pos(obAtom->x(), obAtom->y(), obAtom->z());
      atom = atomMap.value(&*obAtom); 

      if (!atom) {

         atom = createAtom(obAtom->GetAtomicNum(), pos);
         addedPrimitives.append(atom);
         atomMap.insert(&*obAtom, atom);
      }  

      atom->setPosition(pos);
   }
  

   FOR_BONDS_OF_MOL(obBond, obMol) {
      bond  = bondMap.value(&*obBond);
      begin = atomMap.value(obBond->GetBeginAtom());
      end   = atomMap.value(obBond->GetEndAtom());
  
      if (!begin || !end) {
         QString msg("Error encountered converting from OBMol object");
         QMsgBox::critical(0, "IQmol", msg);
         return PrimitiveList();
      }  
  
      if (!bond) {
         bond  = createBond(begin, end, obBond->GetBondOrder());
         addedPrimitives.append(bond);
      }  
   }
  
   return addedPrimitives;
}

} } // end namespace IQmol::Layer
