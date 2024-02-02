#include "GroParser.h"
#include "TextStream.h"
#include "Util/QsLog.h"

#include "Data/Atom.h"
#include "Data/Group.h"
//#include "Data/PdbData.h"
#include "Data/ProteinChain.h"
#include "Data/Geometry.h"
#include "Data/Solvent.h"
#include "Math/Vec.h"
#include <regex>

#include <QDebug>
#include <QFile>
#include <stdlib.h>
#include <vector>


namespace IQmol {
namespace Parser {

std::vector<QString> getTopologyFiles(QString topolPath){
  QFile file(topolPath);
 
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
  TextStream topolTextStream(&file);
  const QString fileString = topolTextStream.readAll();
  QRegularExpression rx("\"(topol_Protein.*?)\"");
  QRegularExpressionMatchIterator i = rx.globalMatch(fileString);
  std::vector<QString> output;
  while (i.hasNext()) {
    QRegularExpressionMatch match = i.next();
    QString subtopol = match.captured(1);
    QLOG_DEBUG() << "newtopol";
    QLOG_DEBUG() << subtopol;
    output.push_back(subtopol);


  }



  return output;

  }
}


bool Gro::parse(TextStream& textStream){
  bool ok(true);
  std::string str = m_filePath.toStdString();
  size_t index =str.find_last_of("/\\");
  QLOG_DEBUG() << "loading structure file";
  QString prefix = QString::fromStdString(str.substr(0,(index+1)));
  QString path = prefix + "SecStruc.dat";
  QString topolpath = prefix + "topol.top";
  QLOG_DEBUG() << path;
  QFile file(path);

  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
  TextStream secStrucTextStream(&file);
  QLOG_DEBUG() << "loading succesfull";
  Data::ProteinChain* chain(0);
  Data::Solvent* solvent(0);
  Data::Group* group(0);
  Data::Atom* CA(0);
  Data::Atom* O(0);
  TextStream* topology;


  int topolcount(0);
  float atomcharge(0);
  std::vector<QString> topologies = getTopologyFiles(topolpath);


  QFile topolfile(prefix + topologies.at(topolcount) );



  if (topolfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    topology = new TextStream(&topolfile);
    topology->seek(";   nr       type  resnr residue  atom   cgnr     charge       mass  typeB    chargeB      massB");

  
  }else{
   error:
      QString msg("Error parsing toplogyfile ");
      msg +=  prefix + topologies.at(topolcount);
      m_errors.append(msg);
  }

  std::vector<QString> secStrucChain;
  std::vector<int>secStrucType,secStrucResStart,secStrucResStop;
  std::vector<float> chain_charges;
  Math::Vec3 vCA;
  Math::Vec3 vO;

   QString line, key, secStrucLine, secStrucKey,topolline;
   QString currentChain;
   QString currentGroup;
   int linenumber(0);
   int secStrucIndex(0);
   QString s;
  QString aToZ ="ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  int chainNumber(0);
  int grpNumber(0);

  chain = new Data::ProteinChain(QString("Chain ") + aToZ[chainNumber]);
  
  m_chains.insert(currentChain, chain);
   while (!secStrucTextStream.atEnd()) {
    secStrucLine = secStrucTextStream.nextLineNonTrimmed();
    secStrucKey = secStrucLine;
    secStrucType.push_back(secStrucLine.mid(0,1).trimmed().toInt());
    secStrucChain.push_back(secStrucLine.mid(4,1).trimmed());
    secStrucResStart.push_back(secStrucLine.mid(8,4).trimmed().toInt());
    secStrucResStop.push_back(secStrucLine.mid(15,4).trimmed().toInt());

    QLOG_DEBUG() << secStrucLine.mid(0,1).trimmed()+ " " + secStrucLine.mid(4,1).trimmed() + " " + secStrucLine.mid(8,4).trimmed() + " " + secStrucLine.mid(15,4).trimmed();


   }
   for(int i =0; i<secStrucResStop.size(); i++){
    QLOG_DEBUG() << QString::number(secStrucResStop[i]);
   }
   while (!textStream.atEnd()) {


      
      line = textStream.nextLineNonTrimmed();
      key  = line;
    if(linenumber >= 2 && std::regex_search(line.toStdString(), std::regex("[A-Za-z]"))) {

    QString label(line.mid(11, 4).trimmed());  // eg. CHA
    QString res(line.mid(5, 3).trimmed());    // eg. HIS
    //QString id (line.mid(21, 1));              // eg. A
    QString grp(line.mid(1, 4).trimmed());    // eg. 143
    QString sym(label[0]);    // eg. C
    char strucType(0);  

  //Convert to picameters
    double x = 10*line.mid(20, 8).toFloat(&ok);  if (!ok) goto error;
    double y = 10*line.mid(28, 8).toFloat(&ok);  if (!ok) goto error;
    double z = 10*line.mid(36, 8).toFloat(&ok);  if (!ok) goto error;

    qglviewer::Vec v(x,y,z);

    grpNumber = grp.toInt();
    //QLOG_DEBUG() << "chain number" <<chainNumber;
    //QLOG_DEBUG() << "secStrucIndex" <<secStrucIndex;
    if(secStrucIndex ==32){
      secStrucIndex =31;
    }

    if (!(aToZ[chainNumber]==secStrucChain[secStrucIndex])){
      strucType = 0;
    } else if (grpNumber < secStrucResStart[secStrucIndex]){
      strucType = 0;
    }else if (grpNumber == secStrucResStop[secStrucIndex])
    {
      strucType = secStrucType[secStrucIndex];
      if (secStrucIndex < 32){
      ++secStrucIndex;
      }
    }else
    {
      strucType = secStrucType[secStrucIndex];
    }
    
    
    //QLOG_DEBUG() << "current " << currentGroup <<" new " <<grp;
    if(currentGroup.toInt() > grp.toInt()){
      ++chainNumber;
      ++topolcount;
      topolfile.close();
      QFile topolfile(prefix + topologies.at(topolcount));
      currentChain = aToZ[chainNumber];
      QLOG_DEBUG() << "new chain "<< aToZ[chainNumber];
      QLOG_DEBUG() << "new topol "<< prefix + topologies.at(topolcount);
      chain = new Data::ProteinChain(QString("Chain ") + aToZ[chainNumber]);
      m_chains.insert(currentChain, chain);

      /* if (topolfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
          topology = new TextStream(&topolfile);
          topology->seek(";   nr       type  resnr residue  atom   cgnr     charge       mass  typeB    chargeB      massB");

        
        }else{
            QString msg("Error parsing toplogyfile ");
            msg +=  prefix + topologies.at(topolcount);
            msg +=  "\n" + line;
            m_errors.append(msg);
        }*/
    }else{
      chain = m_chains[currentChain];
    }

    if(grp!= currentGroup){
      topology->skipLine();
      currentGroup = grp;
      group = new Data::Group(grp.trimmed() + " " +res);
      chain->append(group);
    }
    //topolline = topology->nextLineNonTrimmed();
    //atomcharge = topolline.mid(49, 8).toFloat(&ok);  if (!ok) goto error;
    atomcharge =0;
    Data::Atom* atom(new Data::Atom(sym, label));
    group->addAtom(atom, v,atomcharge); 

    if (label == "CA"){
      CA = atom;
      Math::Vec3 vtemp {x,y,z};
      vCA = vtemp;
    }
   if (label == "O"){
      O = atom;
      Math::Vec3 vtemp {x,y,z};
      vO = vtemp;
      chain->appendAlphaCarbon(vCA);
      chain->appendPeptideOxygen(vO);

      if(strucType == 0 ){
        chain->appendSecondaryStructure(Data::SecondaryStructure::Coil);
      }
      if(strucType == 1){
        chain->appendSecondaryStructure(Data::SecondaryStructure::Helix);
      }
      if(strucType == 2){
        chain->appendSecondaryStructure(Data::SecondaryStructure::Sheet);
      }
      //chain->addResidue(vCA, vO, strucType);
    }
    
    if (!ok) goto error3;
    }
    ++linenumber;
   }

/*for (int chainId = 0; chainId < m_chains.size(); chainId++) {
    Data::chain *C = m_chains[chainId];
    
    QString id(C->id);
    Data::ProteinChain* proteinChain(m_chains[id]);

       for (int r = 0; r < C->residues.size(); r++) {
           Data::residue *R = &C->residues[r];
           Data::atom const* CA = Data::Pdb::getAtom(*R, (char *)"CA");
           Data::atom const* O  = Data::Pdb::getAtom(*R, (char *)"O");
           char ss = R->ss;

           if (CA == 0 || O == 0) {
               QString msg("CA/O not found in chain ");
               msg + C->id + " residue " + R->type + "_"  + R->id;
               m_errors.append(msg);
               return false;
           }   

           proteinChain->addResidue(CA->coor, O->coor, ss);
       }   
   }   
  */

  QLOG_DEBUG() << "reached end of parsing";
  for (auto chain : m_chains.values()) m_dataBank.append(chain);
  QLOG_DEBUG() << "reached end of chains";
 for (auto geom: m_geometries.values()) m_dataBank.append(geom);
  QLOG_DEBUG() << "reached end of geom";
  if (solvent){

  m_dataBank.append(solvent);

    return ok;
    error3:
      QString msg("Error parsing PDB file around line number ");
      msg +=  QString::number(textStream.lineNumber());
      msg +=  "\n" + line;
      m_errors.append(msg);
      return false;
      
}
}
}
bool Gro::parseATOM(QString const& line, Data::Group& group, float atomcharge)
{
   bool ok(true);
   double x = line.mid(20, 8).toFloat(&ok);
   if (!ok) return false;
   double y = line.mid(28, 8).toFloat(&ok);
   if (!ok) return false;

   double z = line.mid(36, 8).toFloat(&ok);
   if (!ok) return false;
   QString label(line.mid(11, 4).trimmed());
   QString sym(label[0]);
   Data::Atom* atom(new Data::Atom(sym, label));
   group.addAtom(atom, qglviewer::Vec(x,y,z),atomcharge);
  

   return ok;
}










} } // end namespace IQmol::Parser
