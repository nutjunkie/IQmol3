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

#include <QMap>
#include <set>
#include <QString>
#include "KeywordSection.h"


namespace Qui {

typedef QMap<QString, QString> StringMap;


// A KeywordSection made up of keys and values, such as $rem and $solvent.
class KeyValueSection : public KeywordSection 
{
   public:
      KeyValueSection(QString const& name, bool visible = true, bool visibleWhenEmpty = true) 
       : KeywordSection(name, visible, visibleWhenEmpty) { }

      void read(QString const& input);

      void setOption(QString const& key, QString const& value);

      QString getOption(QString const& key) const {
         return m_options.count(key) > 0 ? m_options[key] : QString();
      }

      StringMap const& getOptions() const {
         return m_options;
      }

      void printOption(QString const& key, bool print);

      bool printOption(QString const& key) const {
         return m_toPrint.count(key) > 0;
      }

      KeyValueSection* clone() const;

      static void addAdHoc(QString const& rem, QString const& v1, QString const& v2);

   protected:
      QString formatContents() const;

   private:
      /// An ad-hoc map which converts values in the QUI to values used in
	  /// QChem and vice versa.  The values are stored as a QMap with keys
	  /// of the form rem::quiValue => qchemValue and also 
      /// rem::qchemValue => quiValue.  This allows a given value to be 
      /// mapped to different things depending on which option the value is 
      /// associated with.   This function is called in
      /// InputDialog::initializeControl() to avoid additional database access.
      static StringMap s_adHoc;

      static void fixAdHoc(QString const& name, QString& value);
       
	  /// Contains a list of the option values that have been explicitly set
	  /// (i.e. altered from the default.
	  StringMap m_options;

	  //! Contains a list of those options that should be printed to the input
	  //! file.  This act as a filter on the contents of m_options.
      std::set<QString> m_toPrint;
};

} // end namespace Qui
