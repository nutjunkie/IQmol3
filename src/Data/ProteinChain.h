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
#include "Math/Vec.h"


namespace IQmol {
namespace Data {

   // Data class representing a single chain of a protein
   class ProteinChain : public MacroMolecule 
   {
      public:
         ProteinChain(QString const& label) : MacroMolecule(label) { }

         Type::ID typeID() const { return Type::ProteinChain; }

         int    nres() const { return m_secondaryStructure.size(); }
         double const* cao() const { return &m_caoPositions[0]; }
         char const*  ss() const { return &m_secondaryStructure[0]; }

         inline void addResidue(Math::Vec3 const& posCA, Math::Vec3 const& posO,
            char const secondaryStructure)
         {
             m_caoPositions.push_back(posCA[0]);
             m_caoPositions.push_back(posCA[1]);
             m_caoPositions.push_back(posCA[2]);
             m_caoPositions.push_back(posO[0]);
             m_caoPositions.push_back(posO[1]);
             m_caoPositions.push_back(posO[2]);

             m_secondaryStructure.push_back(secondaryStructure); 
         }

      private:
         // Data for generataing the cartoon representation
         unsigned           m_nResidues;
         std::vector<double> m_caoPositions;
         std::vector<char>  m_secondaryStructure;
   };

} } // end namespace IQmol::Data
