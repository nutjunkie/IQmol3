#pragma once
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

#include "DataList.h"


namespace IQmol {
namespace Data {

   class File : public Base {

      public:
         File(QString const& filePath = QString()) : m_filePath(filePath) { }

         Type::ID typeID() const { return Type::File; }
         QString path() const { return m_filePath; }
         void dump() const;

      private:
         QString m_filePath;
   };


   class FileList : public List<File> { 
      public:
          Type::ID typeID() const { return Type::FileList; }
   };

} } // end namespace IQmol::Data
