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
#include "Math/Vec.h"


namespace IQmol {
namespace Data {

   class Pdb : public Base {

      public:
         struct Residue;

         struct Atom 
         {
            int id;
            int idx;
            char type[5];
            char element[3];
            Math::Vec3 coor;
            double tfactor, occupancy;
            Atom *next, *prev;
            Residue *res;
         };

         struct Residue 
         {
            int id;
            int idx;
            char type[5];
            std::vector<Atom> atoms;
            char ss;
            Residue *next, *prev;
         };

         struct Chain 
         {
            char id;
            std::vector<Residue> residues;
         };

         struct SecondaryStructure 
         {
            char type;
            int  start;
            int  stop;
            char chain[2];
         };

         typedef std::vector<Chain> ChainList;

      friend class boost::serialization::access;

      public:
         Type::ID typeID() const { return Type::Pdb; }

         Pdb() { }
         ~Pdb() { }

         ChainList& chainList() { return m_chainList; }

         int*   nres() { return &m_nResPerChain[0]; }
         double* cao() { return &m_caoPositions[0]; }
         int*   ss()  { return &m_secondaryStructure[0]; }

         unsigned nChains() const { return m_chainList.size(); }

         void dump() const;

         void addResidue(Math::Vec3 const& posO, Math::Vec3 const& posCA, 
            char const secondaryStructure);

         void addChain(int const size);

         void appendChain(Chain newChain);

         void appendResiduetoChain (Chain *C, Residue newResidue);

         void appendAtomtoResidue (Residue *R, Atom newAtom);

         static Atom const* getAtom (Residue const& resA, char const* atomType);

         void fillSecondaryStructure(std::vector<SecondaryStructure> secStructs);

         void serialize(InputArchive& ar, unsigned int const /*version*/) { }
         void serialize(OutputArchive& ar, unsigned int const /*version*/) { }

         void write(const char *filename) const;
         void writeFile(FILE *F) const;

      private:
         
         void updateAtomPointers(Residue *R);
         void updateResiduePointers(Chain *C);
         Chain* getChain(char *c);

         ChainList m_chainList;

         std::vector<int>    m_nResPerChain;
         std::vector<double> m_caoPositions;
         std::vector<int>   m_secondaryStructure;
   };

} } // end namespace IQmol::Data
