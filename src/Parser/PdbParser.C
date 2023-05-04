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

#include "PdbParser.h"
#include "TextStream.h"

#include "Data/Geometry.h"
#include "Data/Atom.h"

#include <QFile>
#include <QFileInfo>
#include <QDebug>


namespace IQmol {
namespace Parser {

bool Pdb::parse(TextStream& textStream)
{
   bool ok(true);
   
   Data::Geometry* geometry(new Data::Geometry);

   QString line, key;

   while (!textStream.atEnd()) {
      line = textStream.nextLine();
      key  = line;

      key.resize(6);
      key = key.trimmed();

      if (key == "ATOM" || key == "HETATM") {
         ok = ok && parseATOM(line, *geometry);
         if (!ok) goto error;

      }else {
      } 
      
   }

   if (ok) {
      //geometryList = new Data::GeometryList;
      m_dataBank.append(geometry);
   }

   return ok;

   error:
      QString msg("Error parsing PDB file around line number ");
      msg +=  QString::number(textStream.lineNumber());
      msg +=  "\n" + line;
      m_errors.append(msg);
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
bool Pdb::parseATOM(QString const& line, Data::Geometry& geometry)
{
   QString key(line.mid(0,5));
   QString index(line.mid(6,10));

/*
   s = line.mid( 0, 6);  // Record name   "ATOM  "
   s = line.mid( 6, 5);  // Integer       serial       Atom  serial number.
   s = line.mid(12, 4);  // Atom          name         Atom name.

   s = line.mid(16, 1);  // Character     altLoc       Alternate location indicator.
   s = line.mid(17, 3);  // Residue name  resName      Residue name.
   s = line.mid(21, 1);  // Character     chainID      Chain identifier.
   s = line.mid(22, 4);  // Integer       resSeq       Residue sequence number.
   s = line.mid(26, 4);  // AChar         iCode        Code for insertion of residues.

   s = line.mid(30, 8);  // Real(8.3)     x            Orthogonal coordinates for X in Angstroms.
   s = line.mid(38, 8);  // Real(8.3)     y            Orthogonal coordinates for Y in Angstroms.
   s = line.mid(46, 8);  // Real(8.3)     z            Orthogonal coordinates for Z in Angstroms.
*/

   //x = line.mid(54, 6);  // Real(6.2)     occupancy    Occupancy.
   //x = line.mid(60, 6);  // Real(6.2)     tempFactor   Temperature  factor.

  // x = line.mid(76, 2);  // LString(2)    element      Element symbol, right-justified.

   //x = line.mid(78, 2);  // LString(2)    charge       Charge  on the atom.-

   
   bool ok(true);

   unsigned a;
   double x, y, z;
   QString sym(line.mid(76, 2));
   sym = sym.trimmed(); 
   qDebug() << " Converting" << sym;

   a = Data::Atom::atomicNumber(sym);    if (a == 0) return false;
   x = line.mid(30, 8).toDouble(&ok);  if (!ok) return false;
   y = line.mid(38, 8).toDouble(&ok);  if (!ok) return false;
   z = line.mid(46, 8).toDouble(&ok);  if (!ok) return false;
   
   geometry.append(a, qglviewer::Vec(x,y,z));

   return ok;

}

/*
12345678901234567890123456789012345678901234567890123456789012345678901234567890
HETATM  894  C1  EOH A2001      18.245  -4.085  12.277  0.60 10.06           C
ATOM    338  NH1 ARG A  17      -3.026 -10.418   8.551  1.00  2.94           N
*/

} } // end namespace IQmol::Parser
