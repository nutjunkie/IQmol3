#include "GroParser.h"
#include "TextStream.h"
#include "Util/QsLog.h"

#include "Data/Atom.h"
#include "Data/Group.h"
#include "Data/PdbData.h"
#include "Data/ProteinChain.h"
#include "Data/Geometry.h"
#include "Data/Solvent.h"
#include <regex>

#include <QDebug>

#include <stdlib.h>
#include <vector>


namespace IQmol {
namespace Parser {


bool Gro::parse(TextStream& textStream)
{
    QLOG_DEBUG() << "trying to parse using gro parser";
    QLOG_DEBUG() << m_filePath;

    bool ok(true);
   
  Data::ProteinChain* chain(0);
  Data::Geometry* geometry(0);
  Data::Solvent* solvent(0);
  Data::Group* group(0);

   // loadTopologyFiles();

   QString line, key;
   QString currentChain;
   QString currentGroup;
   int linenumber(0);
   QString s;
  QString aToZ ="ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  int chainNumber = 0;
  chain = new Data::ProteinChain(QString("Chain ") + aToZ[chainNumber]);
  //chain = new Data::ProteinChain(QString("Chain ") + aToZ[chainNumber]);
  m_chains.insert(currentChain, chain);

   while (!textStream.atEnd()) {
      
      line = textStream.nextLineNonTrimmed();
      key  = line;
    if(linenumber >= 2 && std::regex_search(line.toStdString(), std::regex("[A-Za-z]"))) {

    QString label(line.mid(11, 4).trimmed());  // eg. CHA
    QString res(line.mid(5, 3).trimmed());    // eg. HIS
    //QString id (line.mid(21, 1));              // eg. A
    QString grp(line.mid(1, 4).trimmed());    // eg. 143
    QString sym(label[0]);    // eg. C
  

    double x = line.mid(20, 8).toFloat(&ok);  if (!ok) goto error;
    double y = line.mid(28, 8).toFloat(&ok);  if (!ok) goto error;
    double z = line.mid(36, 8).toFloat(&ok);  if (!ok) goto error;

    qglviewer::Vec v(x,y,z);
 
    //QLOG_DEBUG() << "current " << currentGroup <<" new " <<grp;
    if(currentGroup.toInt() > grp.toInt()){
      ++chainNumber;
      QLOG_DEBUG() << "new chain "<< aToZ[chainNumber];
      chain = new Data::ProteinChain(QString("Chain ") + aToZ[chainNumber]);
      m_chains.insert(currentChain, chain);
    }else{
      chain = m_chains[currentChain];
    }

    if(grp!= currentGroup){
      currentGroup = grp;
      group = new Data::Group(grp.trimmed() + " " +res);
      chain->append(group);
    }
    Data::Atom* atom(new Data::Atom(sym, label));
    group->addAtom(atom, v);
    
    if (!ok) goto error;
    }
    ++linenumber;
   }

  QLOG_DEBUG() << "reached end of parsing";
  for (auto chain : m_chains.values()) m_dataBank.append(chain);
  QLOG_DEBUG() << "reached end of chains";
 for (auto geom: m_geometries.values()) m_dataBank.append(geom);
  QLOG_DEBUG() << "reached end of geom";
  if (solvent) m_dataBank.append(solvent);

    return ok;
    error:
      QString msg("Error parsing PDB file around line number ");
      msg +=  QString::number(textStream.lineNumber());
      msg +=  "\n" + line;
      m_errors.append(msg);
      return false;
}    


bool Gro::parseCartoon(TextStream& textStream)
{
   return false;
}


int Gro::parseGRO(char const* groFilePath, Data::Gro* data , char *options) 
{
    return 0;
}


bool Gro::parseATOM(QString const& line, Data::Group& group)
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
   group.addAtom(atom, qglviewer::Vec(x,y,z));

   return ok;
}


bool Gro::parseCOMPND(QString const& line)
{
   return false;
}


void loadTopologyFiles()
{

}

} } // end namespace IQmol::Parser
