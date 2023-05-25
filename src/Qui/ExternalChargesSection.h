#ifndef QUI_EXTERNALCHARGESSECTION_H
#define QUI_EXTERNALCHARGESSECTION_H

/*!
 *  \class ExternalChargesSection 
 *
 *  \brief A KeywordSection class representing a $external_charges block
 *   
 *  \author Andrew Gilbert
 *  \date February 2008
 */

#include "KeywordSection.h"


namespace Qui {


class ExternalChargesSection : public KeywordSection {
   public:
      ExternalChargesSection(QString const& contents = "", bool visible = true) 
       : KeywordSection("external_charges", visible, true, contents) {
         processContents();
      }

      void read(QString const& input);

      ExternalChargesSection* clone() const;

   protected:
      QString previewContents() const;

   private:
      QString m_truncatedData;

      void processContents();
};


} // end namespace Qui
#endif
