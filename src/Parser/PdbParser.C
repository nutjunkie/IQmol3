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
   
  Based off the CPDB code by:
  Gokhan SELAMET on 28/09/2016.
  Copyright Â© 2016 Gokhan SELAMET. All rights reserved.

********************************************************************************/

#include "PdbParser.h"
#include "TextStream.h"

#include "Math/v3.h"
#include "Data/Atom.h"
#include "Data/Group.h"
#include "Data/PdbData.h"
#include "Data/ProteinChain.h"

#include <QDebug>

#include <stdlib.h>
#include <vector>



namespace IQmol {
namespace Parser {


int extractStr(char *dest, const char *src, int begin, int end) {
    int length;
    while (src[begin] == ' ') begin++;
    while (src[end] == ' ') end--;
    length = end - begin + 1;
    strncpy(dest, src + begin, length);
    dest[length] = 0;
    return length;
}

void getCoordinates (const char *line, v3 *out) {
    char coorx[9] = {0};
    char coory[9] = {0};
    char coorz[9] = {0};
    strncpy (coorx, &line[30], 8);
    strncpy (coory, &line[38], 8);
    strncpy (coorz, &line[46], 8);
    out->x = atof (coorx);
    out->y = atof (coory);
    out->z = atof (coorz);
}

void getAtomType (const char *line, char *out) {
    extractStr(out, line, 12, 16);
}

void getResType (const char *line, char *out) {
    extractStr(out, line, 17, 19);
}

void getAtomElement (const char *line, char *out) {
    extractStr(out, line, 76, 78);
}

void getAtomId (const char *line, int *atomId) {
    char _temp[6]={0};
    extractStr(_temp, line, 6, 10);
    *atomId = atoi(_temp);
}

void getResidueId (const char *line, int *residueId) {
    char _temp[5]={0};
    extractStr(_temp, line, 22, 25);
    *residueId = atoi(_temp);
}

void getAlternativeLoc(const char *line, char *altLoc) {
    *altLoc = line[16];
}

void getChainId(const char *line, char *chainId) {
    *chainId = line[21];
}

void getOccupancy (const char *line, float *occupancy) {
    char _temp[7]={0};
    extractStr(_temp, line, 54, 59);
    *occupancy = atof(_temp);
}

void getTempFactor (const char *line, float *tempFactor){
    char _temp[7]={0};
    extractStr(_temp, line, 60, 65);
    *tempFactor = atof(_temp);
}

void getHelix(const char *line, int *start, int *stop, char *chain){
    if(strlen(line) >= 37){
        extractStr(chain, line, 19, 20);
        char _temp[5]={0};
        extractStr(_temp, line, 21, 24);
        *start = atoi(_temp);
        extractStr(_temp, line, 33, 36);
        *stop = atoi(_temp);
    }
}

void getSheet(const char *line, int *start, int *stop, char *chain){
    if(strlen(line) >= 38){
        extractStr(chain, line, 21, 22);
        char _temp[5]={0};
        extractStr(_temp, line, 23, 26);
        *start = atoi(_temp);
        extractStr(_temp, line, 34, 37);
        *stop = atoi(_temp);
    }
}


bool Pdb::parse(TextStream& textStream)
{
   bool ok(true);
   
   QMap<QString, Data::ProteinChain*> chains;

   Data::ProteinChain* chain(0);
   Data::Group* group(0);

   QString line, key;
   QString currentChain;
   QString currentGroup;

   while (!textStream.atEnd()) {
      line = textStream.nextLine();
      key  = line;

      key.resize(6);
      key = key.trimmed().toUpper();

      if (key == "ATOM" || key == "HETATM") {
         QString s;

         // Chain Id
         s = line.mid(21, 1);
         if (s != currentChain) {
            currentChain = s;
            if (chains.contains(currentChain)) {
               chain = chains[currentChain];
            }else {
               chain = new Data::ProteinChain(QString("Chain ") + currentChain);
               chains.insert(currentChain, chain);
            }
         }

         s = line.mid(22, 4); // Res seq
         if (s != currentGroup) {
            currentGroup = s;
            group = new Data::Group(s.trimmed() + " " +line.mid(17, 3).trimmed());
            chain->append(group);
         }

         ok = ok && parseATOM(line, *group);
         if (!ok) goto error;

      }else if (key == "COMPND") {
         parseCOMPND(line);
      } 
      
   }

   if (!chains.isEmpty()) {
      QList<Data::ProteinChain*> list(chains.values());
      for (auto chain : chains.values()) m_dataBank.append(chain);
   } 
   
   // This must go last as it rewinds the TextStream!
   ok = parseCartoon(textStream);

   return ok;

   error:
      QString msg("Error parsing PDB file around line number ");
      msg +=  QString::number(textStream.lineNumber());
      msg +=  "\n" + line;
      m_errors.append(msg);
      return false;
}


bool Pdb::parseCOMPND(QString const& line)
{
  if (line.contains("MOLECULE")) {
     QString s(line);
     s.remove(0,20); 
     m_label += s;
  }   
  return true;
}


/* http://www.wwpdb.org/documentation/file-format-content/format33/sect9.html#ATOM

COLUMNS        DATA  TYPE    FIELD        DEFINITION
-------------------------------------------------------------------------------------
 1 -  6        Record name   "ATOM  "
 7 - 11        Integer       serial       Atom  serial number.
13 - 16        Atom          name         Atom name.
17 - 17        Character     altLoc       Alternate location indicator.
18 - 20        Residue name  resName      Residue name.
22 - 22        Character     chainID      Chain identifier.
23 - 26        Integer       resSeq       Residue sequence number.
27 - 30        AChar         iCode        Code for insertion of residues.
31 - 38        Real(8.3)     x            Orthogonal coordinates for X in Angstroms.
39 - 46        Real(8.3)     y            Orthogonal coordinates for Y in Angstroms.
47 - 54        Real(8.3)     z            Orthogonal coordinates for Z in Angstroms.
55 - 60        Real(6.2)     occupancy    Occupancy.
61 - 66        Real(6.2)     tempFactor   Temperature  factor.
77 - 78        LString(2)    element      Element symbol, right-justified.
79 - 80        LString(2)    charge       Charge  on the atom.
*/


bool Pdb::parseATOM(QString const& line, Data::Group& group)
{
   bool ok(true);

   QString key(line.mid(0,5));
   QString index(line.mid(6,10));

   double x = line.mid(30, 8).toFloat(&ok);
   if (!ok) return false;

   double y = line.mid(38, 8).toFloat(&ok);
   if (!ok) return false;

   double z = line.mid(46, 8).toFloat(&ok);
   if (!ok) return false;

   QString label(line.mid(12, 4).trimmed());
   QString sym(line.mid(76, 2).trimmed());
   
   Data::Atom* atom(new Data::Atom(sym, label));
   group.addAtom(atom, qglviewer::Vec(x,y,z));

   return ok;
}


/*
 by default, parsePDB function do not parse 'Hydrogene' atoms and 'Alternate Location' atoms.
 To parse these atoms you can pass `options` variable with these characters
    h: for parsing hydrogen atoms
    l: for parsing alternate locations
    a: is equal to h and l both on
*/
int Pdb::parsePDB (char const* pdbFilePath, Data::Pdb* data , char *options) 
{
    Data::pdb *P(data->ptr());
    Data::chain *C;
    char altLocFlag = 1; // Default: skip alternate location atoms on
    char hydrogenFlag = 0; // Default: skip hydrogene atoms off
    int errorcode = 0;
    C = (Data::chain *)calloc(1, sizeof (Data::chain) );
    char line[128];
    FILE* pdbFile;
    char resType[5], atomType[5], atomElement[3], chainId, altLoc ;
    int resId, atomId, length;
    float tempFactor, occupancy;
    int resIdx = 0;
    int atomIdx = 0;
    v3 coor;
    int helixStart = 0;
    int helixStop = 0;
    char helixChain[2];
    int sheetStart = 0;
    int sheetStop = 0;
    char sheetChain[2];
    std::vector<Data::SS> secStructs;

    Data::residue *currentResidue = NULL;
    Data::chain *currentChain = NULL;
    Data::atom *currentAtom = NULL;
    
    // Setting Parsing Options
    int chindex = 0;
    char ch = options[chindex++];

    while (ch) {
        switch( ch ) {
            case 'h' :
                hydrogenFlag = 0;
                break;
            case 'l' :
                altLocFlag = 0;
                break;
            case 'a':
                hydrogenFlag = 0;
                altLocFlag = 0;
                break;
            default:
                fprintf(stderr, "invalid option: %c \n", ch );
                errorcode ++;
        }
        ch=options[chindex++];
    }

    pdbFile = fopen(pdbFilePath, "r");
    length = 0;
    
    if (pdbFile == NULL) {
        perror("pdb file cannnot be read");
        exit (2);
    }
    while (fgets(line, sizeof(line), pdbFile)) {
        if (!strncmp(line, "ATOM  ", 6)) {

            getCoordinates(line, &coor);
            getAtomType(line, atomType);
            getResType(line, resType);
            getAtomElement(line, atomElement);
            getAtomId(line, &atomId);

            getResidueId(line, &resId);
            getChainId(line, &chainId);
            getAlternativeLoc(line, &altLoc);
            getTempFactor(line, &tempFactor);
            getOccupancy(line, &occupancy);
            if ( hydrogenFlag && !strncmp(atomElement, "H", 3) ) continue;
            if ( altLocFlag && !(altLoc == 'A' || altLoc == ' ' ) ) continue;
            
            // Chain Related
            if (currentChain == NULL || currentChain->id != chainId) {
                Data::chain *myChain = (Data::chain *)calloc(1, sizeof(Data::chain));
                myChain->id = chainId;
                myChain->residues = NULL;
                myChain->__capacity = 0;

                data->appendChain(*myChain);
                currentChain = &(P->chains[P->size - 1]);
                currentAtom = NULL;
                currentResidue = NULL;
            }
            
            // Residue Related
            if (currentResidue == NULL || currentResidue->id != resId) {
                Data::residue *myRes = (Data::residue *)calloc(1, sizeof(Data::residue));
                myRes->id = resId;
                myRes->idx = resIdx++;
                myRes->atoms = NULL;
                myRes->size = 0;
                myRes->__capacity = 0;
                myRes->next = NULL;
                myRes->prev = currentResidue;
                myRes->ss = COIL;

                data->appendResiduetoChain(currentChain, *myRes);

                currentResidue = &(currentChain->residues[currentChain->size-1]);

                strncpy(currentResidue->type, resType, 5);
                if (currentResidue->prev) currentResidue->prev->next=currentResidue;
            }
            
            //Atom Related
            if (currentAtom == NULL || currentAtom->id != atomId) {
                Data::atom myAtom;
                myAtom.id = atomId;
                myAtom.idx = atomIdx++;
                myAtom.coor = coor;
                myAtom.tfactor = tempFactor;
                myAtom.occupancy = occupancy;
                myAtom.next = NULL;
                myAtom.prev = currentAtom;
                myAtom.res = currentResidue;
                data->appendAtomtoResidue(currentResidue, myAtom);
                currentAtom = &(currentResidue->atoms[currentResidue->size-1]);
                strncpy(currentAtom->type, atomType, 5);
                strncpy(currentAtom->element, atomElement, 3);
                if (currentAtom->prev) currentAtom->prev->next = currentAtom;
            }
            
        }
        if (!strncmp(line, "HELIX ", 6)) {
            getHelix(line, &helixStart, &helixStop, helixChain);
            Data::SS curSS; 
            curSS.start = helixStart; 
            curSS.stop = helixStop; 
            strncpy(curSS.chain, helixChain, 2);
            curSS.type = HELIX;
            secStructs.push_back(curSS);
        }
        if (!strncmp(line, "SHEET ", 6)) {
            getSheet(line, &sheetStart, &sheetStop, sheetChain);
            Data::SS curSS; 
            curSS.start = sheetStart; 
            curSS.stop = sheetStop; 
            strncpy(curSS.chain, sheetChain, 2);
            curSS.type = STRAND;
            secStructs.push_back(curSS);
        }
        else if (!strncmp(line, "ENDMDL", 6)  || !strncmp(line, "END", 3)) {
            break;
        }
    }
    fclose(pdbFile);

    data->fillSS(secStructs);
    return errorcode;
}



bool Pdb::parseCartoon(TextStream& textStream)
{
   textStream.rewind(); 
   qDebug() << "In the cartoon PDB parser";

   Data::Pdb* pdbData = new Data::Pdb;

   Data::pdb *P = pdbData->ptr();

   std::string str(m_filePath.toStdString());
   const char* fname(str.c_str());
   char options[0];

   parsePDB(fname, pdbData, options);

   for (int chainId = 0; chainId < P->size; chainId++) {
       Data::chain *C = &P->chains[chainId];
       pdbData->addChain(C->size);
       for (int r = 0; r < C->size; r++) {
           Data::residue *R = &C->residues[r];
           Data::atom *CA = Data::Pdb::getAtom(*R, (char *)"CA");
           Data::atom *O  = Data::Pdb::getAtom(*R, (char *)"O");
           char ss = R->ss;

           if (CA == 0 || O == 0) {
               QString msg("CA/O not found in chain ");
               msg + C->id + " residue " + R->type + "_"  + R->id;
               m_errors.append(msg);
               return false;
           }   

           pdbData->addResidue(CA->coor, O->coor, ss);
       }   
   }   

   m_dataBank.append(pdbData);

   return true;
}

} } // end namespace IQmol::Parser
