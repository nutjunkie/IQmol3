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

  http://www.wwpdb.org/documentation/file-format-content/format33/sect9.html#ATOM

  COLUMNS        DATA  TYPE    FIELD        DEFINITION
  -------------------------------------------------------------------------------------
   1 -  6   6    Record name   "ATOM  "
   7 - 11   5    Integer       serial       Atom  serial number.
  12 - 12   1   space
  13 - 16   4    Atom          name         Atom name.
  17 - 17   1    Character     altLoc       Alternate location indicator.
  18 - 20   3    Residue name  resName      Residue name.
  21 - 21   1   space
  22 - 22   1    Character     chainID      Chain identifier.
  23 - 26   4    Integer       resSeq       Residue sequence number.
  27 - 30   4    AChar         iCode        Code for insertion of residues.
  31 - 38   8    Real(8.3)     x            Orthogonal coordinates for X in Angstroms.
  39 - 46   8    Real(8.3)     y            Orthogonal coordinates for Y in Angstroms.
  47 - 54   8    Real(8.3)     z            Orthogonal coordinates for Z in Angstroms.
  55 - 60   6    Real(6.2)     occupancy    Occupancy.
  61 - 66   6    Real(6.2)     tempFactor   Temperature  factor.
  67 - 76  10   space
  77 - 78   2    LString(2)    element      Element symbol, right-justified.
  79 - 80   2    LString(2)    charge       Charge  on the atom.


  COLUMNS        DATA  TYPE     FIELD         DEFINITION
  -----------------------------------------------------------------------------------
   1 -  6        Record name    "HELIX "
   8 - 10        Integer        serNum        Serial number of the helix. This starts
                                              at 1  and increases incrementally.
  12 - 14        LString(3)     helixID       Helix  identifier. In addition to a serial
                                              number, each helix is given an 
                                              alphanumeric character helix identifier.
  16 - 18        Residue name   initResName   Name of the initial residue.
  20             Character      initChainID   Chain identifier for the chain containing
                                              this  helix.
  22 - 25        Integer        initSeqNum    Sequence number of the initial residue.
  26             AChar          initICode     Insertion code of the initial residue.
  28 - 30        Residue  name  endResName    Name of the terminal residue of the helix.
  32             Character      endChainID    Chain identifier for the chain containing
                                              this  helix.
  34 - 37        Integer        endSeqNum     Sequence number of the terminal residue.
  38             AChar          endICode      Insertion code of the terminal residue.
  39 - 40        Integer        helixClass    Helix class (see below).
  41 - 70        String         comment       Comment about this helix.
  72 - 76        Integer        length        Length of this helix.


  COLUMNS       DATA  TYPE     FIELD          DEFINITION
  -------------------------------------------------------------------------------------
   1 -  6        Record name   "SHEET "
   8 - 10        Integer       strand         Strand  number which starts at 1 for each
                                              strand within a sheet and increases by one.
  12 - 14        LString(3)    sheetID        Sheet  identifier.
  15 - 16        Integer       numStrands     Number  of strands in sheet.
  18 - 20        Residue name  initResName    Residue  name of initial residue.
  22             Character     initChainID    Chain identifier of initial residue 
                                              in strand. 
  23 - 26        Integer       initSeqNum     Sequence number of initial residue
                                              in strand.
  27             AChar         initICode      Insertion code of initial residue
                                              in  strand.
  29 - 31        Residue name  endResName     Residue name of terminal residue.
  33             Character     endChainID     Chain identifier of terminal residue.
  34 - 37        Integer       endSeqNum      Sequence number of terminal residue.
  38             AChar         endICode       Insertion code of terminal residue.
  39 - 40        Integer       sense          Sense of strand with respect to previous
                                              strand in the sheet. 0 if first strand,
                                              1 if  parallel,and -1 if anti-parallel.
  42 - 45        Atom          curAtom        Registration.  Atom name in current strand.
  46 - 48        Residue name  curResName     Registration.  Residue name in current strand
  50             Character     curChainId     Registration. Chain identifier in
                                              current strand.
  51 - 54        Integer       curResSeq      Registration.  Residue sequence number
                                              in current strand.
  55             AChar         curICode       Registration. Insertion code in
                                              current strand.
  57 - 60        Atom          prevAtom       Registration.  Atom name in previous strand.
  61 - 63        Residue name  prevResName    Registration.  Residue name in
                                              previous strand.
  65             Character     prevChainId    Registration.  Chain identifier in
                                              previous  strand.
  66 - 69        Integer       prevResSeq     Registration. Residue sequence number
                                              in previous strand.
  70             AChar         prevICode      Registration.  Insertion code in
                                              previous strand.

********************************************************************************/

#include "PdbParser.h"

#include "TextStream.h"
#include "Data/Atom.h"
#include "Data/Geometry.h"
#include "Data/ProteinChain.h"
#include "Data/Solvent.h"
#include "Util/QsLog.h"
#include "Math/Vec.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QVector>


namespace IQmol {
namespace Parser {


bool Pdb::parse(TextStream& textStream)
{
   bool ok(true);

   Data::ProteinChain* chain(0);
   Data::Geometry* geometry(0);
   Data::Solvent* solvent(0);
   Data::Residue* residue(0);

   QString line, key;
   QString currentGeometry;
   QChar   currentChainId;

   unsigned currentResidueId(0);

   while (!textStream.atEnd()) {
      line = textStream.nextLine();
      key  = line.mid(0, 6).trimmed().toUpper();  // eg. CHA

      if (key == "COMPND") {
         if (line.contains("MOLECULE")) {
            QString s(line);
            s.remove(0,20); 
            m_label += s;
         } 

      }else if (key == "HELIX") {
         QString chainId(line.mid(19,1));
         
         SS ss;
         ss.chain = chainId[0];
         ss.start = line.mid(21,4).toInt(&ok);  if (!ok) goto error;
         ss.stop  = line.mid(33,4).toInt(&ok);  if (!ok) goto error;
         ss.type  = Data::SecondaryStructure::Helix;

         m_secondaryStructure.append(ss);

      }else if (key == "SHEET") {
         QString chainId(line.mid(21,1));

         SS ss;
         ss.chain = chainId[0];
         ss.start = line.mid(22,4).toInt(&ok);  if (!ok) goto error;
         ss.stop  = line.mid(33,4).toInt(&ok);  if (!ok) goto error;
         ss.type  = Data::SecondaryStructure::Sheet;
 
         m_secondaryStructure.append(ss);

      }else if (key == "ATOM" || key == "HETATM") {

         QString alternateLocation(line.mid(16, 1).trimmed());
         if (!alternateLocation.isEmpty() && alternateLocation != 'A') continue;

         QString label(      line.mid(12, 4).trimmed());    // eg. CA
         QString residueName(line.mid(17, 3).trimmed());    // eg. HIS
         QString chainId(    line.mid(21, 1));              // eg. A
         QString atomSymbol( line.mid(76, 2).trimmed());    // eg. C

         unsigned residueId = line.mid(22, 4).trimmed().toUInt(&ok);  if(!ok) goto error;

         double x = line.mid(30, 8).toFloat(&ok);  if (!ok) goto error;
         double y = line.mid(38, 8).toFloat(&ok);  if (!ok) goto error;
         double z = line.mid(46, 8).toFloat(&ok);  if (!ok) goto error;

         Math::Vec3 v {x,y,z};
         qglviewer::Vec qv {x,y,z};

         if (key == "ATOM") {

            if (chainId[0] != currentChainId) {
               currentChainId = chainId[0];
               if (m_chains.contains(currentChainId)) {
                  chain = m_chains[currentChainId];
               }else {
                  chain = new Data::ProteinChain(currentChainId);
                  m_chains.insert(currentChainId, chain);
                  m_chainOrder.append(currentChainId);
               }
            }

            if (residueId != currentResidueId) {
               currentResidueId = residueId;
               Data::AminoAcid_t type(Data::AminoAcid::toType(residueName));
               
               residue = new Data::Residue(type, residueId);
               chain->append(residue);
            }
   
            Data::Atom* atom(new Data::Atom(atomSymbol, label));
            residue->addAtom(atom, qv);

            if (label == "CA") {
               chain->appendAlphaCarbon(v);
            }else if (label == "O") {
               chain->appendPeptideOxygen(v);
            }

         }else if (residueName == "HOH") {  // HETATM
            if (!solvent) solvent = new Data::Solvent("Water");
            solvent->addSolvent(qv);

         }else {  // HETATM
            QString geom = QString::number(residueId) + " ("+chainId+")";

            if (geom != currentGeometry) {
               currentGeometry = geom;

               if (m_geometries.contains(currentGeometry)) {
                  geometry = m_geometries[currentGeometry];
               }else {
                  geometry = new Data::Geometry();
                  geometry->name(residueName + " " + geom);
                  m_geometries.insert(currentGeometry, geometry);
                  m_geometryOrder.append(currentGeometry);
               }
            }
            
            geometry->append(atomSymbol, qv, label);
         }         

      }else if (key == "ENDMDL" || key == "END") {
            break;
      } 
   }

   if (!setSecondaryStructure()) {
      m_errors.append("Failed to set secondary structure information");
      goto error;
   }

   if (!saveSecondaryStructure()) {
      m_errors.append("Failed to save secondary structure information");
      goto error;
   }

   //for (auto chain : m_chains.values()) m_dataBank.append(chain);
   for (auto c : m_chainOrder) m_dataBank.append(m_chains[c]);

   if (solvent) m_dataBank.append(solvent);
   // These are the non-protein HETATM systems
   // for (auto geom: m_geometries.values()) m_dataBank.append(geom);
   for (auto g : m_geometryOrder) m_dataBank.append(m_geometries[g]);
  
 
   return ok;

   error:
      if (solvent) delete solvent;
      for (auto chain : m_chains.values()) delete chain;

      QString msg("Error parsing PDB file around line number ");
      msg +=  QString::number(textStream.lineNumber());
      msg +=  "\n" + line;
      m_errors.append(msg);
      return false;
}


Data::SecondaryStructure 
   Pdb::getSecondaryStructure(QChar const& chain, int const index)
{
   Data::SecondaryStructure s(Data::SecondaryStructure::Coil);

   for (auto& ss : m_secondaryStructure) {
       if (ss.chain != chain) continue;
       if (index >= ss.start && index <= ss.stop) {
          s = ss.type;
          break;
       }
   }

   return s;
}


bool Pdb::setSecondaryStructure()
{
   bool ok(true);

qDebug() << "!!!Secondary structure order based on Map keys!!!";
   for (auto const& key : m_chains.keys()) {
       Data::ProteinChain* chain(m_chains[key]);
       QList<Data::Group*> groups(chain->groups());

       QVector<Data::SecondaryStructure> secondaryStructure;

       for (auto& group : groups) {
           Data::Residue* residue = dynamic_cast<Data::Residue*>(group); 
           if (residue) {
              int index(residue->index());
              Data::SecondaryStructure
                  ss(getSecondaryStructure(key, index));
              secondaryStructure.push_back(ss);
           }
       }

       chain->setSecondaryStructure(secondaryStructure); 
   }

   return ok;
}


bool Pdb::saveSecondaryStructure()
{
   QFileInfo fileInfo(m_filePath);
   fileInfo.setFile(fileInfo.dir(),"SecStruc.dat");

   QFile file(fileInfo.absoluteFilePath());
   if (!file.open(QIODevice::WriteOnly)) {
       m_errors.append("Failed to open secondary structure file for write");
       return false;
   } 

   QByteArray buffer;

   for (auto const& ss : m_secondaryStructure) {

       QString line = QString("%1  %2  %3  %4\n").arg(int(ss.type), 1)
                                                 .arg(ss.chain,1)
                                                 .arg(ss.start,4)
                                                 .arg(ss.stop, 4);
       buffer.append(line.toLatin1());
   }

   buffer.append(QString("end file").toLatin1());
   file.write(buffer);
   file.close();

   return true;
}

} } // end namespace IQmol::Parser
