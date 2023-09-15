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

#include "AminoAcid.h"
#include "Util/QsLog.h"


namespace IQmol {
namespace Data {

static const QStringList symbols = 
{
      "Ala", "Arg", "Asn", "Asp", "Cys", 
      "Gln", "Glu", "Gly", "His", "Ile",
      "Leu", "Lys", "Met", "Phe", "Pro",
      "Ser", "Thr", "Trp", "Tyr", "Val",
      "Asx", "Glx", "Xxx"
};


QString const& AminoAcid::toString(AminoAcid_t const type)
{
   unsigned idx = static_cast<unsigned>(type);
   return symbols[idx];
}


QChar AminoAcid::toLetter(AminoAcid_t const type)
{
   static const QString letters("ARNDCQEGHILKMFPSTWYVBZ");
   unsigned idx = static_cast<unsigned>(type);
   return letters[idx];
}


AminoAcid_t AminoAcid::toType(QString s)
{
   s = s.toLower();
   s[0] = s[0].toUpper();

   int idx(symbols.indexOf(s));
   if (idx < 0) {
      QLOG_WARN() << "Invalid Amino Acid encountered" << s;
      idx = symbols.size()-1;
   }

   return static_cast<AminoAcid_t>(idx);
}

} } // end namespace IQmol::Data
