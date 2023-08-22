#pragma once
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

#include <QtDebug>
#include "Atom.h"


namespace IQmol {
namespace Data {

   // Data class a group of primitives, e.g. residue, solvent molecule
   class Group : public Base {

      public:
         Group(QString const& label = QString()) : m_charge(0), m_label(label) { }

         Type::ID typeID() const { return Type::Group; }

         void addAtom(Data::Atom* atom, qglviewer::Vec const& position)
         {
            m_atoms.append(atom);
            m_coordinates.append(position);
         }

         AtomList const& atoms() const { return m_atoms; }

         QList<qglviewer::Vec> const& coordinates() const { return m_coordinates; }

         int charge() const { return m_charge; }
         QString label() const { return m_label; }

         void serialize(InputArchive& ar, unsigned const version = 0)  { }
         void serialize(OutputArchive& ar, unsigned const version = 0)  { }

         void dump() const 
         {  
             qDebug() << "Data::Group" << m_label;
             unsigned nAtoms(m_atoms.size());
             for (unsigned i(0); i < nAtoms; ++i) {
                 qDebug() << m_atoms[i]->getLabel() << "  " 
                          << m_coordinates[i].x << "  "
                          << m_coordinates[i].y << "  "
                          << m_coordinates[i].z;
             }
         }
         
      private:
         int          m_charge;
         QString      m_label;
         AtomList     m_atoms;
         QList<qglviewer::Vec> m_coordinates;
   };

} } // end namespace IQmol::Data
