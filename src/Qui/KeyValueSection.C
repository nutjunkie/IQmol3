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

#include "QtVersionHacks.h"
#include "KeyValueSection.h"
#include "OptionDatabase.h"
#include "Option.h"
#include "QsLog.h"

#include <QtDebug>
#include <QRegularExpression>


namespace Qui {

StringMap KeyValueSection::s_adHoc;


void KeyValueSection::addAdHoc(QString const& rem, QString const& quiValue, 
   QString const& qchemValue) {
   // qchemValue (from the database) can be of the form qchem1|qchem2|...,
   // where all the valid QChem inputs are sparated by the | character.  For
   // the Qui->QChem mapping we have a one to one relationship, but for
   // QChem->Qui we have many to one.
   QString ucRem(rem.toUpper());
   QStringList validQChemValues(qchemValue.split("|"));

   s_adHoc[ucRem + "::" + quiValue] = validQChemValues[0];

   for (int i = 0; i < validQChemValues.count(); ++i) {
       // Even more ad Hoc.  This avoids MP2 getting transformed to MP2[V].
       // What really needs to be done is to have different translation units
       // depending on which direction the translation occurs.
       if (quiValue != "MP2[V]") {
          s_adHoc[ucRem + "::" + validQChemValues[i]] = quiValue;
       }   
   }   
}


void KeyValueSection::fixAdHoc(QString const& name, QString& value)
{
   QString key = name + "::" + value;
   if(s_adHoc.count(key)) value = s_adHoc[key];

   Option opt; 
   OptionDatabase& db(OptionDatabase::instance());
   bool inDatabase(db.get(name, opt));
   if (!inDatabase) return;

   if (opt.getType() == Option::Type_Logical) {
      if (value.toLower() == "true") {
         value = "1";
      }else if (value.toLower() == "false") {
         value = "0";
      }else if (value.toInt() != 0) {
         value = "1";
      }
   } 

   if (name == "QUI_GEOM_OPT_USER_TOPOLOGY") {
      value = (value.toInt() == 0) ? "generated" : "read";
   }
}


void KeyValueSection::printOption(QString const& key, bool print)
{
//qDebug() << "KVSection setting print" << key << print;

   // always print these options if the $block is active
   if ( key == "QUI_SOLVENT_DIELECTRIC" ||
        key == "QUI_PCM_THEORY"         ||
        key == "QUI_CHEMSOL_EFIELD"     ||
        key == "QUI_SMX_SOLVENT") {
        print  = true;
   }
        
   if (print) {
      m_toPrint.insert(key);
   }else {
      m_toPrint.erase(key);
   }
}


void KeyValueSection::setOption(QString const& key, QString const& value) 
{
   m_options.insert(key,value);
}


QString KeyValueSection::formatContents() const
{
   QString s;

   StringMap::const_iterator iter;
   QString key, value;
   
   for (iter = m_options.begin(); iter != m_options.end(); ++iter) {
       key   = iter.key();
       //qDebug() << "Dumping" << key << printOption(key);
       if (printOption(key) ) {
          value = iter.value();
          fixAdHoc(key, value);
          key.replace("QUI_SOLVENT_","");
          key.replace("QUI_PCM_","");
          key.replace("QUI_SMX_","");
          key.replace("QUI_CHEMSOL_","");
          key.replace("QUI_GEOM_OPT_","");
          s += "   " + key + "  " + value + "\n";
       }
   }

   return s;
}


void KeyValueSection::read(QString const& data) 
{
   QStringList lines( data.trimmed().split("\n", IQmolSkipEmptyParts) );
   QStringList tokens;
   QString     line;

   for (int i = 0; i < lines.count(); ++i) {
       tokens = lines[i].split(QRegularExpression("\\s+"), IQmolSkipEmptyParts);

       if (tokens.count() == 1) {
          QString key(tokens[0].toUpper());
          setOption(key, "");
          printOption(key, true);
       }else if (tokens.count() >= 2) {
          QString key(tokens[0].toUpper());
          QString value(tokens[1].toUpper());
          setOption(key, value);
          printOption(key, true);
       }else {
          QLOG_DEBUG() << "Invalid line in section" << m_name << ":" << line;
       }   
   }   
}


KeyValueSection* KeyValueSection::clone() const 
{
   KeyValueSection* kvs(new KeyValueSection(m_name, m_visible));
   kvs->m_options = m_options;
   kvs->m_toPrint = m_toPrint;
   return kvs;
}

} // end namespace Qui

