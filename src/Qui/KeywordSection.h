#ifndef QUI_KEYWORDSECTION_H
#define QUI_KEYWORDSECTION_H

/*!
 *  \class KeywordSection
 *
 *  \brief An abstract base class representing containers for holding $section
 *  data.  This base class primarily defines the I/O interface.
 *   
 *  \author Andrew Gilbert
 *  \date January 2008
 */

#include <QString>


namespace Qui {

class KeywordSection {

   public:
      KeywordSection(QString const& name, bool visible = true, 
         QString const& contents = QString()) 
       : m_visible(visible), m_name(name), m_contents(contents.trimmed()) { }

      QString name() const { return m_name; }

      void visible(bool visible) { m_visible = visible; }
      bool visible() const { return m_visible; }

      QString format(bool preview = false) const
      {
         QString s;
         if (m_visible) {
            s  = "$" + name() + "\n";
            s += preview ? previewContents() : formatContents();
            s += "$end\n\n";
         }
         return s;
      }

      virtual QString formatContents() const 
      { 
         return m_contents.isEmpty() ? m_contents : m_contents + "\n"; 
      }

      virtual void read(QString const& contents) 
      {
         m_contents = contents.trimmed(); 
         if (m_contents.isEmpty()) m_visible = false;
      }

      virtual KeywordSection* clone() const
      {
          return new KeywordSection(m_name, m_visible, m_contents);
      }

   protected:
	  // Allows for trunction of large contents
      virtual QString previewContents() const
      {
         return formatContents();
      }

      bool    m_visible;
      QString m_name;
      QString m_contents;


   private:
      KeywordSection(KeywordSection const& that);
      KeywordSection const& operator=(KeywordSection const& that);
};



// Non-member functions
//! A factory for generating KeywordSections
KeywordSection* KeywordSectionFactory(QString const& type);

} // end namespace Qui
#endif
