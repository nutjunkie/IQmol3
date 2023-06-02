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

#include "Data.h"
#include "Math/v3.h"


namespace IQmol {
namespace Data {

   struct residue;

   struct atom {
      int id;
      int idx;
      char type[5];
      char element[3];
      v3 coor;
      float tfactor, occupancy;
      atom *next, *prev;
      residue *res;
   };

   struct residue {
      int id;
      int idx;
      char type[5];
      atom *atoms;
      int size;
      int __capacity;
      char ss;
      residue *next, *prev;
   };

   struct chain {
      char id;
      residue *residues;
      int size;
      int __capacity;
   };

   struct pdb {
      chain *chains;
      int size;
      int __capacity;
      char name[32];
   };

   struct SS {
      char type;
      int start;
      int stop;
      char chain[2];
   };


   class Pdb : public Base {

      friend class boost::serialization::access;

      public:
         Type::ID typeID() const { return Type::Pdb; }

         void dump() const { }

         void serialize(InputArchive& ar, unsigned int const /*version*/) 
         {
         }

         void serialize(OutputArchive& ar, unsigned int const /*version*/) 
         {
         }

      private:
   };

} } // end namespace IQmol::Data
