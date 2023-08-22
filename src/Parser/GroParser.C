#include "GroParser.h"
#include "TextStream.h"
#include "Util/QsLog.h"

#include "Data/Atom.h"
#include "Data/Group.h"
#include "Data/PdbData.h"
#include "Data/ProteinChain.h"

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
   Data::Group* group(0);

   // loadTopologyFiles();

   QString line, key;
   QString currentChain;
   QString currentGroup;
   int linenumber(0);
   QString s;

   chain = new Data::ProteinChain(QString("Chain 1") );
    m_chains.insert(currentChain, chain);

   while (!textStream.atEnd()) {
      line = textStream.nextLineNonTrimmed();
      key  = line;
    if(linenumber >= 2 && line.length() >= 40 ) {
    s = line.mid(1, 4); // Res seq
        if (s != currentGroup) {
            currentGroup = s;
            group = new Data::Group(s.trimmed() + " " +line.mid(5, 3).trimmed());
            chain->append(group);
        }
       
    ok = ok && parseATOM(line, *group);
    if (!ok) goto error;
    }
    ++linenumber;
   }


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


int Gro::parseGRO (char const* groFilePath, Data::Gro* data , char *options) 
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
