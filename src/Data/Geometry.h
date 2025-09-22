#pragma once
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

#include "QmAtom.h"
#include <QtDebug>
#include <vector>
#include "QGLViewer/vec.h"


namespace IQmol {
namespace Data {

   /// Data class representing molecule with a particular geometry.  
   class Geometry : public Base {

      public:
		 // Copy constructor only copies the charge, multiplicity, atomic
		 // numbers and positions.  Atom and Geometry properties are not copied.
         Geometry();
         Geometry(Geometry const&);

         Geometry(QList<unsigned> const& atomicNumbers, 
            QList<double> const& coordinates);

         Geometry(std::vector<unsigned> const& atomicNumbers, 
           std::vector<double> const& coordinates);

         void set(QList<unsigned> const& atomicNumbers, QList<double> const& coordinates);

         Type::ID typeID() const { return Type::Geometry; }
         
         bool isValid() const;
         unsigned nAtoms() const { return (unsigned)m_atoms.size(); }
         QString atomicSymbol(unsigned const i) const;
         QString atomicLabel(unsigned const i) const;
         unsigned atomicNumber(unsigned const i) const;
         qglviewer::Vec position(unsigned const i) const;

         QList<qglviewer::Vec> const& coordinates() const { return m_coordinates; }
         void setCoordinates(QList<qglviewer::Vec> const&);

         /// Use to convert between Bohr and Angstrom
         void scaleCoordinates(double const);

         QString coordinatesAsString() const;

         /// Two geometries are considered the same if their AtomLists contain
         /// the same atoms in the same order.  Everything else could be
         /// different.  This allows, for example, the structures of a geometry
         /// optimization to be considered the same.
         bool sameAtoms(Geometry const&) const;
         bool sameAtoms(QList<unsigned> const& symbols) const;
         bool sameAtoms(QStringList const& symbols) const;

         void append(unsigned const z, qglviewer::Vec const& position, 
            QString const& label = QString());
         void append(QString const& symbol, qglviewer::Vec const& position, 
            QString const& label = QString());
         void append(QList<unsigned> const& z, QList<qglviewer::Vec> const& positions);

         void setCharge(int const charge);
         void setMultiplicity(unsigned const multiplicity);

         void setChargeAndMultiplicity(int const charge, unsigned const multiplicity);
         int  charge() const { return m_charge; }
         unsigned multiplicity() const { return m_multiplicity; }
         void computeGasteigerCharges();

         template <class P>
         bool setAtomicProperty(QList<double> values) 
         {
            if (values.size() != m_atoms.size()) return false; 
            QmAtomList::iterator iter;
            unsigned i(0);
            for (iter = m_atoms.begin(); iter != m_atoms.end(); ++iter, ++i) {
                P& p((*iter)->getProperty<P>());
                p.setValue(values[i]);
            }
            return true;
         }

         template <class P>
         P& getAtomicProperty(unsigned i) 
         {
            return m_atoms[i]->getProperty<P>();
         }

         template <class P>
         QStringList getLabels() 
         {
            QStringList labels;
            QList<QmAtom*>::iterator iter;
            for (iter = m_atoms.begin(); iter != m_atoms.end(); ++iter) {
                labels.append((*iter)->getLabel<P>());
            }
            return labels;
         }

         template <class P>
         P& getProperty() 
         {
            P* p(0);
            Bank::iterator iter;
            for (iter = m_properties.begin(); iter != m_properties.end(); ++iter) {
                if ( (p = dynamic_cast<P*>(*iter)) ) break; 
            }
            if (p == 0) {
               p = new P;
               m_properties.append(p);
            }
            return *p;
         }

         template <class P>
         bool hasProperty() const {
            P* p(0);
            Bank::const_iterator iter;
            for (iter = m_properties.begin(); iter != m_properties.end(); ++iter) {
                if ( (p = dynamic_cast<P*>(*iter)) ) return true;
            }
			// Now check for an atomic property, we cheat a bit by only looking
			// at the first atom.
            if (m_atoms.isEmpty()) return false;
            return m_atoms.first()->hasProperty<P>();
         }

         void appendProperty(Data::Base* data) {
            m_properties.append(data);
         }

         void dump() const;

         // These are currently only used in the Parser::Archive, but should be
         // set more generally.
         void setNAlpha(unsigned nAlpha) { m_nAlpha = nAlpha; }
         void setNBeta(unsigned nBeta) { m_nBeta = nBeta; }

         unsigned getNAlpha() const { return m_nAlpha; }
         unsigned getNBeta()  const { return m_nBeta; }

         QString const& name() const { return m_name; }
         void name(QString const& name) { m_name = name; }

      private:
         unsigned totalNuclearCharge() const;

         QmAtomList m_atoms;
         QList<qglviewer::Vec> m_coordinates;
         int m_charge;
         unsigned m_multiplicity;
         unsigned m_nAlpha;
         unsigned m_nBeta;
         Bank m_properties;
         QString m_name;
   };

} } // end namespace IQmol::Data
