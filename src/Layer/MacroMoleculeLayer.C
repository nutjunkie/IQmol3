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

#include "AtomLayer.h"
#include "GroupLayer.h"
#include "MacroMoleculeLayer.h"

#include "Data/MacroMolecule.h"


using namespace qglviewer;

namespace IQmol {
namespace Layer {


MacroMolecule::MacroMolecule(Data::MacroMolecule const& macroMolecule, QObject* parent) : 
   Component(macroMolecule.label(), parent), m_tree(1), m_radius(0)
{
   QList<Data::Group*> const& groups(macroMolecule.groups());

   AtomList m_atoms(findLayers<Atom>(Children));

   for (auto group : groups) {
       Data::AtomList const& atoms(group->atoms());
       QList<qglviewer::Vec>const& coordinates(group->coordinates());

       PrimitiveList primitives;
       unsigned nAtoms(atoms.size());
       for (unsigned i(0); i < nAtoms; ++i) {
           Layer::Atom* atom(new Atom(atoms[i]->atomicNumber()));
           Vec pos(coordinates[i]);
           atom->setPosition(pos);
           m_radius = std::max(m_radius, pos.norm());
  
           primitives.append(atom);
       }

       Layer::Group* groupLayer = new Group(primitives, group->label());
       appendLayer(groupLayer);
   }
   
   //m_configurator.load();
   //setConfigurator(&m_configurator);
}


} } // end namespace IQmol::Layer
