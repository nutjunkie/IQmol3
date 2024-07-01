#include "GroParser.h"
#include "TextStream.h"
#include "Util/QsLog.h"

#include "Data/Atom.h"
#include "Data/Group.h"
//#include "Data/PdbData.h"
#include "Data/ProteinChain.h"
#include "Data/Geometry.h"
#include "Data/Solvent.h"
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


bool Gro::parseChain(TextStream& textStream){
  bool ok(true);
  TextStream* topology;
  QFile topolfile(m_prefix + m_topologies.at(m_topolCount));
  if (topolfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    topology = new TextStream(&topolfile);
    topology->seek(";   nr       type  resnr residue  atom   cgnr     charge       mass  typeB    chargeB      massB");

  }
  m_currentChain = m_aToZ[m_chainNumber];
  QLOG_DEBUG() << "new chain "<< m_aToZ[m_chainNumber];
  QLOG_DEBUG() << "new topol "<< m_prefix + m_topologies.at(m_topolCount);
  m_chain = new Data::ProteinChain(QString(m_aToZ[m_chainNumber]));
  m_chains.insert(m_currentChain, m_chain);
  QString line,key;

 while (!textStream.atEnd()) {   
  line = textStream.nextLineNonTrimmed();
  key  = line;



   QString label(line.mid(11, 4).trimmed());  // eg. CHA
    QString res(line.mid(5, 3).trimmed());    // eg. HIS
    //QString id (line.mid(21, 1));              // eg. A
    QString grp(line.mid(1, 4).trimmed());    // eg. 143
    QString sym(label[0]);    // eg. C
    char strucType(0);  
    //QLOG_DEBUG() << "read line  "<< label << "  "<< res << " " << grp << " "<< sym;
  //Convert to picameters
    double x = 10*line.mid(20, 8).toFloat(&ok);
    double y = 10*line.mid(28, 8).toFloat(&ok);  
    double z = 10*line.mid(36, 8).toFloat(&ok); 
    qglviewer::Vec v(x,y,z);

    m_grpNumber = grp.toInt();
    if(m_secStrucIndex ==32){
      m_secStrucIndex =31;
    }

    if(m_currentGroup.toInt() > grp.toInt()){
      m_currentGroup = '0';
      QLOG_DEBUG() << "end chain loop";
      topolfile.close();
      textStream.skipBack();

      return true;
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
      m_chain = m_chains[m_currentChain];
    }
  //while(!(currentGroup.toInt() > grp.toInt())){
    if (!(m_aToZ[m_chainNumber]==m_secStrucChain[m_secStrucIndex])){
      strucType = 0;
    } else if (m_grpNumber < m_secStrucResStart[m_secStrucIndex]){
      strucType = 0;
    }else if (m_grpNumber == m_secStrucResStop[m_secStrucIndex])
    {
      strucType = m_secStrucType[m_secStrucIndex];
      if (m_secStrucIndex < 32){
      ++m_secStrucIndex;
      }
  

    }else
    {
      strucType = m_secStrucType[m_secStrucIndex];
    }
  if(grp!= m_currentGroup){
      topology->skipLine();
      m_currentGroup = grp;
      m_group = new Data::Group(grp.trimmed() + " " +res);
      m_chain->append(m_group);
    }
    m_topolLine = topology->nextLineNonTrimmed();
    m_atomCharge = m_topolLine.mid(49, 8).toFloat(&ok); 
    Data::Atom* atom(new Data::Atom(sym, label));
    m_group->addAtom(atom, v,m_atomCharge); 

    if (label == "CA"){
      m_CA = atom;
      Math::Vec3 vtemp {x,y,z};
      m_vCA = vtemp;
    }
   if (label == "O"){
      m_O = atom;
      Math::Vec3 vtemp {x,y,z};
      m_vO = vtemp;
      m_chain->appendAlphaCarbon(m_vCA);
      m_chain->appendPeptideOxygen(m_vO);

      if(strucType == 0 ){
        m_chain->appendSecondaryStructure(Data::SecondaryStructure::Coil);
      }
      if(strucType == 1){
        m_chain->appendSecondaryStructure(Data::SecondaryStructure::Helix);
      }
      if(strucType == 2){
        m_chain->appendSecondaryStructure(Data::SecondaryStructure::Sheet);
      }
      //chain->addResidue(vCA, vO, strucType);
    }
    
  
  
   // ++linenumber;
   }
    

  
  return true;
  }


bool Gro::parse(TextStream& textStream){
  bool ok(true);
  std::string str = m_filePath.toStdString();
  size_t index =str.find_last_of("/\\");
  QLOG_DEBUG() << "loading structure file";
  m_prefix = QString::fromStdString(str.substr(0,(index+1)));
  QString path = m_prefix + "SecStruc.dat";
  QString topolpath = m_prefix + "topol.top";
  QLOG_DEBUG() << path;
  QFile file(path);
  QString line, key;

  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
  TextStream secStrucTextStream(&file);
  QLOG_DEBUG() << "loading succesfull";
  //Data::ProteinChain* chain(0);
  m_chain = 0;
  m_solvent = 0;
  m_group = 0;
  m_CA = 0;
  m_O =0;

  //TextStream* topology;


  m_topolCount =0;
  
  m_topologies = getTopologyFiles(topolpath);
  while (!secStrucTextStream.atEnd()) {
    m_secStrucLine = secStrucTextStream.nextLineNonTrimmed();
    m_secStrucKey = m_secStrucLine;
    m_secStrucType.push_back(m_secStrucLine.mid(0,1).trimmed().toInt());
    m_secStrucChain.push_back(m_secStrucLine.mid(4,1).trimmed());
    m_secStrucResStart.push_back(m_secStrucLine.mid(8,4).trimmed().toInt());
    m_secStrucResStop.push_back(m_secStrucLine.mid(15,4).trimmed().toInt());

    QLOG_DEBUG() << m_secStrucLine.mid(0,1).trimmed()+ " " + m_secStrucLine.mid(4,1).trimmed() + " " + m_secStrucLine.mid(8,4).trimmed() + " " + m_secStrucLine.mid(15,4).trimmed();


   }
   for(int i =0; i<m_secStrucResStop.size(); i++){
    QLOG_DEBUG() << QString::number(m_secStrucResStop[i]);
   }
   m_secStrucIndex= 0;
   QString s;
  m_aToZ ="ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  m_chainNumber=0;
  m_grpNumber=0;

 

   while (!textStream.atEnd()) {


      
      line = textStream.nextLineNonTrimmed();
      key  = line;
    if(textStream.lineNumber() >= 2 && std::regex_search(line.toStdString(), std::regex("[A-Za-z]"))) {

    if(!parseChain(textStream)){
      QLOG_DEBUG() << "parseChain false";
      continue;
    }
    m_topolCount++;
    m_chainNumber++;
    QLOG_DEBUG() << "topolCount" << m_topolCount;
    }
    QLOG_DEBUG() << "streaming through " << line;

   }

  
 



    
  QLOG_DEBUG() << "reached end of parsing";
  for (auto chain : m_chains.values()) m_dataBank.append(chain);
  QLOG_DEBUG() << "reached end of chains";
  for (auto geom: m_geometries.values()) m_dataBank.append(geom);
   QLOG_DEBUG() << "reached end of geom";
  if (m_solvent){

  m_dataBank.append(m_solvent);

  return ok;
      }
  return ok;
  }
  return ok;
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
