/*!
 *  \file MoleculeSection.C 
 *
 *  \brief Non-inline member functions of the MoleculeSection class, see
 *  MoleculeSection.h for details.
 *   
 *  \author Andrew Gilbert
 *  \date February 2008
 */

#include "QtVersionHacks.h"
#include "MoleculeSection.h"

#include <QRegularExpression>
#include <QStringList>
#include <QMessageBox>

#include <QtDebug>


namespace Qui {


//! Takes a the text from between the $molecule-$end tags and parses it.
void MoleculeSection::read(QString const& input) 
{
   QStringList lines( input.trimmed().split(QRegularExpression("\\n")) );
   bool okay(false);

   if (lines.count() > 0) {
      QString first(lines[0].replace(QChar(','),QChar(' ')));
      QStringList tokens(first.split(QRegularExpression("\\s+"), IQmolSkipEmptyParts));
      lines.removeFirst();

      if (tokens.count() == 1) {
         if (tokens[0].toLower() == "read") {
            setCoordinates("read");
            okay = true;
         }
      }else if (tokens.count() == 2) {
         // line 1 is charge + multiplicity
         // everything else is the molecule
         bool c,m;
         m_charge = tokens[0].toInt(&c);
         m_multiplicity = tokens[1].toInt(&m);
         okay = c && m;
         setCoordinates(lines.join("\n"));
      }
      
   }

   // TODO: This should really load a molecule object so that the coordinate
   // conversion can be done.
   if (!okay) {
      QString msg("Problem reading $molecule section: \n");
      msg += input;
      QMessageBox::warning(0, "Parse Error", msg);
   }
}


bool MoleculeSection::isReadCoordinates() const
{
    return (m_coordinates == "read");
}


QString MoleculeSection::myDump() const
{
   QString s;
   if (isReadCoordinates()) {
      s += "read\n";
   }else {
      s += QString::number(m_charge) + " ";
      s += QString::number(m_multiplicity) + "\n";
      if (m_isFsm) {
         s += "  " + m_coordinatesFsm + "\n";
      }else {
         if (m_coordinates != "") {
           // ZQ: temporary fix for fragment job input
           if (m_coordinates.contains(QRegularExpression("^--")))
             s += m_coordinates + "\n";
           else
             s += "  " + m_coordinates + "\n";
         }
      }
   }
   return s;
}


QString MoleculeSection::formatContents() const
{
   return myDump(); 
}



MoleculeSection* MoleculeSection::clone() const {
   return new MoleculeSection(m_coordinates, m_charge, m_multiplicity);
}


void MoleculeSection::setCoordinates(QString const& coordinates) { 
   m_coordinates = coordinates;
    qDebug() << "coordinates set" << m_coordinates;
   parseCoordinates();
   qDebug() << "parsed coordinates";
}


void MoleculeSection::setCoordinatesFsm(QString const& coordinates) { 
   m_coordinatesFsm = coordinates; 
}


// TODO: this should be smartened up a bit, for the time being all I want is
// the number of atoms.
void MoleculeSection::parseCoordinates() {
   m_coordinates = m_coordinates.trimmed();
   if (m_coordinates == "read" || m_coordinates == "") {
      m_numberOfAtoms = 0;
   }else {
      m_numberOfAtoms = m_coordinates.count(QRegularExpression("\\n")) + 1; 
   }
   return;
}


void MoleculeSection::setCharge(int charge) {
   m_charge = charge;
}


void MoleculeSection::setMultiplicity(int multiplicity) {
   m_multiplicity = multiplicity;
}

} // end namespace Qui
