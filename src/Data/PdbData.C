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


void Pdb::addResidue(v3 const& posCA, v3 const& posO, char const secondaryStructure)
{
   m_caoPositions.push_back(posCA.x);
   m_caoPositions.push_back(posCA.y);
   m_caoPositions.push_back(posCA.z);
   m_caoPositions.push_back(posO.x);
   m_caoPositions.push_back(posO.y);
   m_caoPositions.push_back(posO.z);

   m_secondaryStructure.push_back(secondaryStructure);
}


void Pdb::appendChain(Data::chain newChain) 
{
    m_pdb.chains.push_back(newChain);    
}


void Pdb::appendResiduetoChain(Data::chain *C, Data::residue newResidue) 
{
   C->residues.push_back(newResidue);
   updateResiduePointers(C);
}


void Pdb::appendAtomtoResidue (Data::residue *R, Data::atom newAtom) 
{
   R->atoms.push_back(newAtom);
   updateAtomPointers(R);
}


void Pdb::updateResiduePointers (Data::chain *C) 
{
    Data::residue* p(0);
    Data::residue* n(0);
    Data::atom* a(0);

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

void Pdb::updateAtomPointers (Data::residue *R) {
    int i;
    Data::atom *p = NULL;
    Data::atom *c = NULL;
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


Data::atom const* Pdb::getAtom(Data::residue const& resA, char const* atomType)
{
    Data::atom const* atomA;
    int i = 0;
    for (i=0; i<resA.atoms.size(); i++) {
        atomA = &resA.atoms[i];
        if ( ! strncmp(atomA->type, atomType, 5) )
            return atomA;
    }
    return NULL;
}


Data::chain* Pdb::getChain(char *c) 
{
   Data::chain* C(0);
   for (int i(0); i < nChains(); i++){
       C = &m_pdb.chains[i];
       if (C->id == c[0]) break;
   }   
   return C;
}


void Pdb::fillSS(std::vector<Data::SS> secStructs) {
    for(int s=0;s<secStructs.size();s++){
        char *c = secStructs[s].chain;
        int start = secStructs[s].start;
        int stop = secStructs[s].stop;

        Data::chain *C = getChain(c); 
        if(C != NULL){
            for(int r=0;r < C->residues.size();r++){
                Data::residue *R = &C->residues[r];
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
        Data::chain const* C = &m_pdb.chains[chainId];
        Data::atom const* A = &C->residues[0].atoms[0];
        while (A != NULL) {
            fprintf (F, "ATOM  %5d %-4s%c%-3s %c%4d    %8.3f%8.3f%8.3f%6.2f%6.2f          %2s\n",
                     A->id, A->type,' ', A->res->type, C->id, A->res->id,
                     A->coor.x, A->coor.y, A->coor.z, A->occupancy, A->tfactor, A->element);
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
