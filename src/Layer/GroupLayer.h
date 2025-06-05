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

#include "AtomLayer.h"
#include "BondLayer.h"
#include <QList>


namespace OpenBabel {
   class OBMol;
   class OBAtom;
   class OBBond;
}

namespace IQmol {

   namespace Layer {

      typedef QMap<OpenBabel::OBAtom*, Atom*>  AtomMap;
      typedef QMap<OpenBabel::OBBond*, Bond*>  BondMap;

      class Group : public Primitive {

         Q_OBJECT

         public:
            Group(PrimitiveList const& = PrimitiveList(), QString const& label = QString());

            ~Group();

            void loadFromFile(QString const& filePath);
            void align(QList<qglviewer::Vec> const& current);
            PrimitiveList ungroup();

            void draw();
            void drawFast();
            void drawSelected();

            void select();
            void deselect();

            void setAtomScale(double const scale);
            void setBondScale(double const scale);
            void setDrawMode(DrawMode const drawMode);
    
            AtomList getAtoms() const { return m_atoms; }
            BondList getBonds() const { return m_bonds; }

            // returns the first atom on the list, if any
            Atom* rootAtom() const;
            void dump() const;

         protected:
            void addAtoms(AtomList const&);
            void addBonds(BondList const&);
            void addPrimitives(PrimitiveList const&);

         private Q_SLOTS:
            void addHydrogens();
            void reperceiveBonds();

         private:
            OpenBabel::OBMol* toOBMol(AtomMap*, BondMap*);
            PrimitiveList fromOBMol(OpenBabel::OBMol*, AtomMap&, BondMap&);

            Bond* createBond(Atom* begin, Atom* end, int order);
            Atom* createAtom(unsigned int const Z, qglviewer::Vec const& position);

            AtomList m_atoms;
            BondList m_bonds;
      };

   } // end namespace Layer

typedef QList<Layer::Group*> GroupList;

} // end namespace IQmol::Layer 
