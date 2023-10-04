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

#include "PdbData.h"
#include <QDebug>


namespace IQmol {
namespace Data {


void Pdb::addChain(int const size)
{
   m_nResPerChain.push_back(size);
}


void Pdb::addResidue(Math::Vec3 const& posCA, Math::Vec3 const& posO, 
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


void Pdb::appendChain(Data::Pdb::Chain newChain) 
{
    m_chainList.push_back(newChain);    
}


void Pdb::appendResiduetoChain(Data::Pdb::Chain *C, Data::Pdb::Residue newResidue) 
{
   C->residues.push_back(newResidue);
   updateResiduePointers(C);
}


void Pdb::appendAtomtoResidue (Data::Pdb::Residue *R, Data::Pdb::Atom newAtom) 
{
   R->atoms.push_back(newAtom);
   updateAtomPointers(R);
}


void Pdb::updateResiduePointers(Data::Pdb::Chain *C) 
{
    Data::Pdb::Residue* p(0);
    Data::Pdb::Residue* n(0);
    Data::Pdb::Atom* a(0);

    for (int i=0; i<C->residues.size(); i++) {
        n = & C->residues[i];
        n->prev = p;
        if (p) p->next = n;
        p = n;
        for (int j=0; j < n->atoms.size(); j++) {
            a = & n->atoms[j];
            a->res = n;
        }
    }
    if (n) n->next = NULL;
}


void Pdb::updateAtomPointers (Data::Pdb::Residue *R) 
{
    int i;
    Data::Pdb::Atom *p = NULL;
    Data::Pdb::Atom *c = NULL;
    if (R->prev) p = & R->prev->atoms[R->prev->atoms.size()-1];

    for (i=0; i<R->atoms.size(); i++){
        c = & R->atoms[i];
        if (p) p->next = c;
        c->prev = p;
        p = c;
    }

    if (R->next) {
        c= & R->next->atoms[0];
        c->prev = p;
        p->next = c;
    }
    else  {
        if(c) c->next = NULL;
    }
}


Data::Pdb::Atom const* Pdb::getAtom(Data::Pdb::Residue const& resA, char const* atomType)
{
    Data::Pdb::Atom const* atomA;
    int i = 0;
    for (i=0; i<resA.atoms.size(); i++) {
        atomA = &resA.atoms[i];
        if ( ! strncmp(atomA->type, atomType, 5) )
            return atomA;
    }
    return NULL;
}


Data::Pdb::Chain* Pdb::getChain(char *c) 
{
   Chain* C(0);
   for (int i(0); i < nChains(); i++){
       C = &m_chainList[i];
       if (C->id == c[0]) break;
   }   
   return C;
}


void Pdb::fillSecondaryStructure(std::vector<Data::Pdb::SecondaryStructure> secStructs) {
    for(int s=0;s<secStructs.size();s++){
        char *c = secStructs[s].chain;
        int start = secStructs[s].start;
        int stop = secStructs[s].stop;

        Chain *C = getChain(c); 
        if(C != NULL){
            for(int r=0;r < C->residues.size();r++){
                Data::Pdb::Residue *R = &C->residues[r];
                if(R->id >= start && R->id <= stop){
                    R->ss = secStructs[s].type;
                }   
            }   
        }   
    }   
}


void Pdb::writeFile(FILE *F) const
{
    int chainId;
    for (chainId = 0 ; chainId < nChains(); chainId++) {
        Chain const* C = &m_chainList[chainId];
        Atom const* A = &C->residues[0].atoms[0];
        while (A != NULL) {
            fprintf (F, "ATOM  %5d %-4s%c%-3s %c%4d    %8.3f%8.3f%8.3f%6.2f%6.2f          %2s\n",
                     A->id, A->type,' ', A->res->type, C->id, A->res->id,
                     A->coor[0], A->coor[1], A->coor[2], A->occupancy, A->tfactor, A->element);
            A = A->next; 
        }       
    }       
}


void Pdb::write(const char *filename) const
{
    FILE *out;
    out = fopen(filename, "w");
    
    if (out == NULL) {
        perror("pdb file write error");
        exit(1);
    }   
    writeFile(out); 
    fclose(out);
}


void Pdb::dump() const
{
    writeFile(stdout);
}

} } // end namespace IQmol::Data
