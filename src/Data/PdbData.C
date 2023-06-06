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

Pdb::Pdb() : m_pdb(0)
{
   m_pdb = (pdb *)calloc(1, sizeof(pdb));
}

Pdb::~Pdb()
{
    Data::chain *C;
    Data::residue *R;
    for (int i(0); i < m_pdb->size; i++) {
        C = & m_pdb->chains[i];
        for (int j(0); j < C->size; j++) {
            R = & C->residues[j];
            free (R->atoms);
        }       
        free(C->residues);
    }       
    free(m_pdb->chains);
    free(m_pdb);
}


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


void Pdb::appendChain(Data::chain newChain) {
    if ( m_pdb->__capacity == 0) {
        m_pdb->chains = (Data::chain *)calloc (1, sizeof(Data::chain));
        m_pdb->__capacity++;
    }
    if (m_pdb->size == m_pdb->__capacity) {
        m_pdb->__capacity += INC_CAPACITY;
        Data::chain *newChain;
        newChain = (Data::chain *)realloc (m_pdb->chains, m_pdb->__capacity*sizeof(Data::chain));
        if (newChain == 0) {
            fprintf (stderr, "Allocation error \n");
            exit(1);
        }
        m_pdb->chains = newChain;
    }
    m_pdb->chains [m_pdb->size++] = newChain;
}


void Pdb::appendResiduetoChain(Data::chain *C, Data::residue newResidue) 
{
    if ( C->__capacity == 0)
        C->residues = (Data::residue *)calloc (1, sizeof(Data::residue));

    if ( C->size == C->__capacity ) {

        C->__capacity += INC_CAPACITY;
        Data::residue *newresidues;
        newresidues = (Data::residue *)realloc(C->residues, C->__capacity * sizeof(Data::residue));
        if (newresidues == 0) {
            fprintf (stderr, "Allocation error \n");
            exit(1);
        }
        C->residues = newresidues;
        memset(&C->residues[C->size], 0, (C->__capacity - C->size)*sizeof(Data::residue));
        C->residues [C->size++] = newResidue;
        updateResiduePointers(C);
    } else {

    C->residues [C->size++] = newResidue;
    }
}

void Pdb::appendAtomtoResidue (Data::residue *R, Data::atom newAtom) {
    if ( R->__capacity == 0)
        R->atoms = (Data::atom *)calloc (1, sizeof(Data::atom));
    if ( R->size == R->__capacity) {
        R->__capacity += INC_CAPACITY;
        Data::atom *newatoms;
        newatoms = (Data::atom *)realloc(R->atoms, R->__capacity * sizeof(Data::atom));
        if (newatoms == 0) {
            fprintf (stderr, "Allocation error \n");
            exit(1);
        }
        R->atoms = newatoms;
        memset(&R->atoms[R->size], 0, (R->__capacity - R->size)*sizeof(Data::atom));
        R->atoms[R->size++] = newAtom;
        updateAtomPointers(R);
    } else {
    R->atoms[R->size++] = newAtom;
    }
}


void Pdb::updateResiduePointers (Data::chain *C) {
    int i, j;
    Data::residue *p = NULL;
    Data::residue *n = NULL;
    Data::atom *a = NULL;
    for (i=0; i<C->size; i++) {
        n = & C->residues[i];
        n->prev = p;
        if (p) p->next = n;
        p = n;
        for (j=0; j<n->size; j++) {
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
    if (R->prev) p = & R->prev->atoms[R->prev->size-1];

    for (i=0; i<R->size; i++){
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


Data::atom* Pdb::getAtom(Data::residue const& resA, char const* atomType)
{
    Data::atom *atomA;
    int i = 0;
    for (i=0; i<resA.size; i++) {
        atomA = &resA.atoms[i];
        if ( ! strncmp(atomA->type, atomType, 5) )
            return atomA;
    }
    return NULL;
}


Data::chain* Pdb::getChain(char *c) 
{
   Data::chain* C(0);
   for (int i(0); i < m_pdb->size; i++){
       C = &m_pdb->chains[i];
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
            for(int r=0;r<C->size;r++){
                Data::residue *R = &C->residues[r];
                if(R->id >= start && R->id <= stop){
                    R->ss = secStructs[s].type;
                }   
            }   
        }   
    }   
}


void Pdb::writePDB (const char *filename, const Data::pdb *P) {
    FILE *out;
    out = fopen(filename, "w");
    
    if (out == NULL) {
        perror("pdb file write error");
        exit(1);
    }   
    writeFilePDB (out, P); 
    fclose(out);
}


void Pdb::writeFilePDB (FILE *F, const Data::pdb *P) {
    Data::chain *C = NULL;
    Data::atom *A = NULL;
    int chainId;
    for (chainId = 0 ; chainId < P->size; chainId++) {
        C = &P->chains[chainId];
        A = &C->residues[0].atoms[0];
        while (A != NULL) {
            fprintf (F, "ATOM  %5d %-4s%c%-3s %c%4d    %8.3f%8.3f%8.3f%6.2f%6.2f          %2s\n",
                     A->id, A->type,' ', A->res->type, C->id, A->res->id,
                     A->coor.x, A->coor.y, A->coor.z, A->occupancy, A->tfactor, A->element);
            A = A->next; 
        }       
    }       
}


void Pdb::printPDB(Data::pdb *P) {
    writeFilePDB(stdout, P);
}

} } // end namespace IQmol::Data
