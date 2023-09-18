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

#include "DataList.h"

//#define COIL 0
//#define HELIX 1
//#define STRAND 2


namespace IQmol {
namespace Data {

   enum class AminoAcid_t 
   {
      ALA, ARG, ASN, ASP, CYS, GLN, GLU, GLY, HIS, ILE, 
      LEU, LYS, MET, PHE, PRO, SER, THR, TRP, TYR, VAL, 
      ASX, GLX, // ambiguous ASN/ASP and GLN/GLU
      XXX       // unknown 
   };

   class AminoAcid : public Base 
   {
      public:
         static QString const& toString(AminoAcid_t const);
         static QChar  toLetter(AminoAcid_t const);
         static AminoAcid_t toType(QString);

         AminoAcid(AminoAcid_t const type) : m_type(type) { }

         Type::ID typeID() const { return Type::AminoAcid; }

         QString const toString() const { return toString(m_type); }
         QChar   const toLetter() const { return toLetter(m_type); }
         AminoAcid_t type() const { return m_type; }

      protected:
         AminoAcid_t m_type;
   };


   class AminoAcidList : public List<AminoAcid> 
   {
      public:
         // Why is a fully qualified namespace required for Type with Qt >= 6.0
         IQmol::Data::Type::ID typeID() const { return IQmol::Data::Type::AminoAcidList; }
   };

} } // end namespace IQmol::Data
