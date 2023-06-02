#pragma once
//
//  Created by Gokhan SELAMET on 28/09/2016.
//  Copyright © 2016 Gokhan SELAMET. All rights reserved.
//


#include <stdio.h>
#include <string.h>
#include <math.h>
#include "Math/v3.h"
#include "Data/PdbData.h"

#define COMMIT_CPDB COMMITSHA1
#define COIL 0
#define HELIX 1
#define STRAND 2

using namespace IQmol;

namespace cpdb {

/*
typedef struct pdb pdb;
typedef struct chain chain;
typedef struct residue residue;
typedef struct atom atom;
typedef struct v3 v3;
*/

Data::pdb* initPDB(void);
int parsePDB (char const* pdbFilePath, Data::pdb *P , char *options);
/*
 by default, parsePDB function do not parse 'Hydrogene' atoms and 'Alternate Location' atoms.
 To parse these atoms you can pass `options` variable with these characters
 	h: for parsing hydrogen atoms
 	l: for parsing alternate locations
 	a: is equal to h and l both on
*/
int writePDB (const char *filename, const Data::pdb *P);
int writeFilePDB (FILE *F, const Data::pdb *P);
int printPDB (Data::pdb *P);
void freePDB (Data::pdb *P);

void appendChaintoPdb (Data::pdb *P, Data::chain newChain);
void appendResiduetoChain (Data::chain *C, Data::residue newResidue);
void appendAtomtoResidue (Data::residue *R, Data::atom newAtom);

Data::atom* getAtom (const Data::residue &resA, const char *atomType);


inline Data::atom* getAtom (Data::residue const& resA, char const* atomType) 
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
} // end namespace cpdb
