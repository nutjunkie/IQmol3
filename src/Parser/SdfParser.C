/*******************************************************************************
       
  Copyright (C) 2026 Andrew Gilbert
           
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

#include "SdfParser.h"
#include "TextStream.h"

#include "Data/Atom.h"
#include "Data/Energy.h"
#include "Data/GeometryList.h"

#include <QRegularExpression>


namespace IQmol {
namespace Parser {

bool Sdf::parse(TextStream& textStream)
{
   Data::GeometryList* geometryList(new Data::GeometryList(m_label));

   while (!textStream.atEnd()) {
      Data::Geometry* geometry(readNextGeometry(textStream));
      if (geometry) geometryList->append(geometry);
   }

   if (geometryList->isEmpty()) {
      m_errors.append("No coordinates found");
      delete geometryList;
   }else {
      m_dataBank.append(geometryList);
   }

   return m_errors.isEmpty();
}


Data::Geometry* Sdf::readNextGeometry(TextStream& textStream)
{
   if (textStream.atEnd()) return 0;

   QString title(textStream.nextLineNonTrimmed());
   if (title.trimmed().isEmpty() && textStream.atEnd()) return 0;

   if (textStream.atEnd()) return 0;
   textStream.nextLineNonTrimmed();
   if (textStream.atEnd()) return 0;
   textStream.nextLineNonTrimmed();
   if (textStream.atEnd()) return 0;

   int nAtoms(0), nBonds(0);
   QString countsLine(textStream.nextLineNonTrimmed());
   if (!readCounts(countsLine, nAtoms, nBonds)) {
      m_errors.append("Invalid SDF counts line around line " +
         QString::number(textStream.lineNumber()));
      return 0;
   }

   Data::Geometry* geometry(new Data::Geometry());

   for (int i = 0; i < nAtoms; ++i) {
      if (textStream.atEnd()) {
         delete geometry;
         m_errors.append("Unexpected end of file in SDF atom block");
         return 0;
      }

      unsigned atomicNumber(0);
      qglviewer::Vec position;
      QString atomLine(textStream.nextLineNonTrimmed());
      if (readAtom(atomLine, atomicNumber, position)) {
         geometry->append(atomicNumber, position);
      }else {
         delete geometry;
         m_errors.append("Invalid SDF atom line around line " +
            QString::number(textStream.lineNumber()));
         return 0;
      }
   }

   for (int i = 0; i < nBonds && !textStream.atEnd(); ++i) {
      textStream.nextLineNonTrimmed();
   }

   QString line;
   while (!textStream.atEnd()) {
      line = textStream.nextLineNonTrimmed();
      if (line.trimmed() == "M  END") break;
   }

   double energy(0.0);
   QString energyLabel;
   if (readEnergy(textStream, energy, energyLabel)) {
      Data::TotalEnergy& total(geometry->getProperty<Data::TotalEnergy>());
      total.setValue(energy, Data::Energy::Hartree);
      total.setLabel(energyLabel);
   }

   return geometry;
}


bool Sdf::readCounts(QString const& line, int& nAtoms, int& nBonds) const
{
   bool atomsOk(false), bondsOk(false);

   if (line.size() >= 6) {
      nAtoms = line.mid(0, 3).toInt(&atomsOk);
      nBonds = line.mid(3, 3).toInt(&bondsOk);
   }

   if (!atomsOk || !bondsOk) {
      QStringList tokens(TextStream::tokenize(line));
      if (tokens.size() >= 2) {
         nAtoms = tokens[0].toInt(&atomsOk);
         nBonds = tokens[1].toInt(&bondsOk);
      }
   }

   return atomsOk && bondsOk && nAtoms >= 0 && nBonds >= 0;
}


bool Sdf::readAtom(QString const& line, unsigned& atomicNumber, qglviewer::Vec& position) const
{
   bool xOk(false), yOk(false), zOk(false);
   QString symbol;

   if (line.size() >= 34) {
      position.x = line.mid( 0, 10).toDouble(&xOk);
      position.y = line.mid(10, 10).toDouble(&yOk);
      position.z = line.mid(20, 10).toDouble(&zOk);
      symbol = line.mid(31, 3).trimmed();
   }

   if (!xOk || !yOk || !zOk || symbol.isEmpty()) {
      QStringList tokens(TextStream::tokenize(line));
      if (tokens.size() >= 4) {
         position.x = tokens[0].toDouble(&xOk);
         position.y = tokens[1].toDouble(&yOk);
         position.z = tokens[2].toDouble(&zOk);
         symbol = tokens[3];
      }
   }

   atomicNumber = Data::Atom::atomicNumber(symbol);

   return xOk && yOk && zOk && atomicNumber > 0;
}


bool Sdf::readEnergy(TextStream& textStream, double& energy, QString& label)
{
   bool found(false);
   QString line;

   while (!textStream.atEnd()) {
      line = textStream.nextLineNonTrimmed();
      QString trimmed(line.trimmed());

      if (trimmed == "$$$$") break;
      if (!trimmed.startsWith(">")) continue;

      QString field(trimmed);
      int lt(field.indexOf('<'));
      int gt(field.lastIndexOf('>'));
      if (lt >= 0 && gt > lt) {
         field = field.mid(lt+1, gt-lt-1);
      }

      QString value;
      while (!textStream.atEnd()) {
         line = textStream.nextLineNonTrimmed();
         trimmed = line.trimmed();
         if (trimmed == "$$$$") return found;
         if (trimmed.isEmpty()) break;
         if (!value.isEmpty()) value += " ";
         value += trimmed;
      }

      if (!found && isEnergyField(field) && parseEnergy(value, energy)) {
         label = field;
         found = true;
      }
   }

   return found;
}


bool Sdf::parseEnergy(QString const& value, double& energy) const
{
   QRegularExpression number("[-+]?(?:\\d+\\.?\\d*|\\.\\d+)(?:[eE][-+]?\\d+)?");
   QRegularExpressionMatch match(number.match(value));
   if (!match.hasMatch()) return false;

   bool ok(false);
   double e(match.captured(0).toDouble(&ok));
   if (ok) energy = e;
   return ok;
}


bool Sdf::isEnergyField(QString const& field) const
{
   QString normalized(field.toLower());
   normalized.remove(QRegularExpression("[^a-z0-9]"));

   QStringList preferred;
   preferred << "energy" << "etot" << "total" << "totalenergy"
             << "finalenergy" << "scfenergy" << "qmenergy";

   return preferred.contains(normalized) || normalized.contains("energy");
}

} } // end namespace IQmol::Parser
