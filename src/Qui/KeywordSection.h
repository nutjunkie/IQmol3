#pragma once
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

#include <QString>

namespace Qui {

// An base class representing containers for holding $section data.  
// in the simplest case this is just a string holding the data.

class KeywordSection {

   public:
      KeywordSection(QString const& name, bool visible = true, 
         bool visibleWhenEmpty = true, QString const& contents = QString())
       : m_visible(visible), m_name(name), m_contents(contents.trimmed()), 
         m_visibleWhenEmpty(visibleWhenEmpty) 
      { }
         
      QString name() const { return m_name; }

      void visible(bool visible) { m_visible = visible; }
      bool visible() const { return m_visible; }

      QString format(bool preview = false) const
      {
         if (!m_visible) return QString();
         QString s(preview ? previewContents() : formatContents());
         if (!s.isEmpty() || m_visibleWhenEmpty) {
            s = "$" + name() + "\n" + s + "$end\n\n";
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
          return new KeywordSection(m_name, m_visible, m_visibleWhenEmpty, m_contents);
      }

      static KeywordSection* Factory(QString const& type);

   protected:
	  // Allows for trunction of large contents in the preview window
      virtual QString previewContents() const
      {
         return formatContents();
      }

      bool    m_visible;
      bool    m_visibleWhenEmpty;
      QString m_name;
      QString m_contents;


   private:
      KeywordSection(KeywordSection const& that);
      KeywordSection const& operator=(KeywordSection const& that);
};

} // end namespace Qui
