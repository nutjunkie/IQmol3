#ifndef QUI_MOLECULESECTION_H
#define QUI_MOLECULESECTION_H

/*!
 *  \class MoleculeSection
 *
 *  \brief A KeywordSection class representing a $molecule section.  
 *   
 *  \author Andrew Gilbert
 *  \date January 2008
 */

#include "KeywordSection.h"


namespace Qui {


class MoleculeSection : public KeywordSection {
   public:
      MoleculeSection(QString const& coordinates = "read", int const charge = 0,
         int multiplicity = 1) : KeywordSection("molecule"), m_charge(charge), 
         m_multiplicity(multiplicity), m_coordinates(coordinates), m_isFsm(false)
      {
         parseCoordinates();
      }

      void read(QString const& input);
      MoleculeSection* clone() const;

      void setCharge(int charge);
      void setMultiplicity(int multiplicity);
      void setCoordinates(QString const& coordinates);
      void setCoordinatesFsm(QString const& coordinates);
      bool isReadCoordinates() const;

      void setFsm(bool tf) { m_isFsm = tf; }

      int getCharge() const { return m_charge; }
      int getMultiplicity() const { return m_multiplicity; }

      QString getCoordinates() { return m_coordinates; }
      int getNumberOfAtoms() { return m_numberOfAtoms; }


   protected:
      QString formatContents() const;


   private:
      int  m_charge;
      int  m_multiplicity;
      int  m_numberOfAtoms;
      QString m_coordinates;
      QString m_coordinatesFsm;
      bool m_isFsm;

      QString myDump() const;
      void parseCoordinates();
};


} // end namespace Qui
#endif
