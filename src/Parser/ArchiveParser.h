#ifndef IQMOL_PARSER_ARCHIVE_H
#define IQMOL_PARSER_ARCHIVE_H
/*******************************************************************************
       
  Copyright (C) 2022 Andrew Gilbert
           
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

#include "Parser.h"
#include <libarchive/api/impl/qarchive.h>

using namespace libarchive::impl;


namespace IQmol {

namespace Data {
   class Geometry;
}

namespace Parser {

   /// Parser for YAML files.
   class Archive : public Base {

      public:
         bool parseFile(QString const& filePath);
         // This should inherit the base-class definition.
         bool parse(TextStream&) { return false; } 

      private:
         Data::Geometry* readGeometry(schema::job::sp&);
         bool readShellData(schema::job::sp&);

         struct OrbitalData {
            int stateIndex;
            QString label;

            QStringList   labels;
            QList<double> alphaCoefficients;
            QList<double> betaCoefficients;
            QList<double> alphaEnergies;
            QList<double> betaEnergies;
         };

   };

} } // end namespace IQmol::Parser

#endif
