#ifndef IQMOL_LAYER_REM_H
#define IQMOL_LAYER_REM_H
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

#include "Layer.h"
#include "Data/RemSectionData.h"


namespace IQmol {
namespace Layer {

   /// Simple Layer holding a set of REM options.
   class Rem : public Base {

      Q_OBJECT

      public:
         Rem(Data::RemSection const& remSection);
         QString formatSection() const { return m_remSection.format(); }

      private:
         Data::RemSection m_remSection;
   };

} }  // end namespace IQmol::Layer

#endif
