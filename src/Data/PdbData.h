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

#include "Data.h"
#include "Math/v3.h"

#define COIL 0
#define HELIX 1
#define STRAND 2
#define INC_CAPACITY 15



namespace IQmol {
namespace Data {

   struct residue;

   struct atom {
      int id;
      int idx;
      char type[5];
      char element[3];
      v3 coor;
      float tfactor, occupancy;
      atom *next, *prev;
      residue *res;
   };

   struct residue {
      int id;
      int idx;
      char type[5];
      std::vector<atom> atoms;
      char ss;
      residue *next, *prev;
   };

   struct chain {
      char id;
      std::vector<residue> residues;
   };

   struct pdb {
      std::vector<chain> chains;
   };

   struct SS {
      char type;
      int  start;
      int  stop;
      char chain[2];
   };


   class Pdb : public Base {

      friend class boost::serialization::access;

      public:
         Type::ID typeID() const { return Type::Pdb; }

         Pdb() { }
         ~Pdb() { }

         pdb& ref() { return m_pdb; }

         int*   nres() { return &m_nResPerChain[0]; }
         float* cao() { return &m_caoPositions[0]; }
         char*  ss() { return &m_secondaryStructure[0]; }

         unsigned nChains() const { return m_pdb.chains.size(); }

         void dump() const;

         void addResidue(v3 const& posO, v3 const& posCA, char const secondaryStructure);
         void addChain(int const size);

         void appendChain(Data::chain newChain);
         void appendResiduetoChain (Data::chain *C, Data::residue newResidue);
         void appendAtomtoResidue (Data::residue *R, Data::atom newAtom);

         static Data::atom const* getAtom (Data::residue const& resA, char const* atomType);

         void fillSS(std::vector<Data::SS> secStructs);

         void serialize(InputArchive& ar, unsigned int const /*version*/) { }
         void serialize(OutputArchive& ar, unsigned int const /*version*/) { }

         void write(const char *filename) const;
         void writeFile(FILE *F) const;

      private:
         
         void updateAtomPointers(Data::residue *R);
         void updateResiduePointers(Data::chain *C);
         Data::chain* getChain(char *c);

         pdb m_pdb;

         std::vector<int>   m_nResPerChain;
         std::vector<float> m_caoPositions;
         std::vector<char>  m_secondaryStructure;
   };

} } // end namespace IQmol::Data
