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

#include "Group.h"
#include "AminoAcid.h"


namespace IQmol {
namespace Data {

    enum class SecondaryStructure 
    {
       Coil, 
       Helix, 
       Sheet
    };

   // Data class a group of primitives, e.g. residue, solvent molecule
   class Residue : public Group {

      public:
         Residue(AminoAcid_t const type, unsigned const index) 
          : Group(QString::number(index) + " " + AminoAcid::toString(type)),
            m_aminoAcid(type), m_index(index)  { }

         Type::ID typeID() const { return Type::Residue; }

         AminoAcid_t type() const { return m_aminoAcid.type(); }
         unsigned  index() const { return m_index; }
         
      private:
         AminoAcid m_aminoAcid;
         unsigned  m_index;
   };

} } // end namespace IQmol::Data
