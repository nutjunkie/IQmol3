#include "GroParser.h"
#include "TextStream.h"
#include "Util/QsLog.h"

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

bool Gro::parse(TextStream& textStream){
    QLOG_DEBUG() << "trying to parse using gro parser";

    bool ok(true);
   
   Data::ProteinChain* chain(0);
   Data::Group* group(0);

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
bool Gro::parseCartoon(TextStream& textStream){}

int Gro::parseGRO (char const* groFilePath, Data::Gro* data , char *options) 
{}

bool Gro::parseATOM(QString const& line, Data::Group& group)
{bool ok(true);
    QLOG_DEBUG() << "trying to parse x";
    QLOG_DEBUG() <<line.mid(20, 8);
   double x = line.mid(20, 8).toFloat(&ok);
   if (!ok) return false;
    QLOG_DEBUG() << "trying to parse y";
   double y = line.mid(28, 8).toFloat(&ok);
   if (!ok) return false;

   double z = line.mid(36, 8).toFloat(&ok);
   if (!ok) return false;
QLOG_DEBUG() << "trying to parse z";
   QString label(line.mid(11, 4).trimmed());
   QLOG_DEBUG() << label;
   QString sym(label[0]);
   QLOG_DEBUG() << sym;
   Data::Atom* atom(new Data::Atom(sym, label));
   group.addAtom(atom, qglviewer::Vec(x,y,z));

   return ok;}
bool Gro::parseCOMPND(QString const& line)
{}
} } // end namespace IQmol::Parser