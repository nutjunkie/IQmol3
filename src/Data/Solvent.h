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

#include "MacroMolecule.h"


namespace IQmol {
namespace Data {

   // Data class representing a collection of solvent molecules
   class Solvent : public MacroMolecule {

      public:
         Solvent(QString const& label = "Solvent") : 
           MacroMolecule(label), m_solventRadius(1.385) { }

         Type::ID typeID() const { return Type::Solvent; }

         inline void addSolvent(qglviewer::Vec const& v)
         {
            m_solventCenters.push_back(v);
         }

         double radius() const { return m_solventRadius; }
         size_t nCenters() const { return m_solventCenters.size(); }
         qglviewer::Vec const& operator[](size_t const i) const { return m_solventCenters[i]; }

         std::vector<qglviewer::Vec> centers() const { return m_solventCenters; }

      private:
         double m_solventRadius;
         std::vector<qglviewer::Vec> m_solventCenters;
   };

} } // end namespace IQmol::Data
