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

#include "QChemJobInfo.h"
#include "QsLog.h"


namespace IQmol {
namespace Process {

QString QChemJobInfo::fileExtension(QString const& file) const
{ 
   QString ext;
  
   if (file == "InputFileName")        { ext += ".inp";  }
   else if (file == "OutputFileName")  { ext += ".out";  }
   else if (file == "AuxFileName")     { ext += ".FChk"; }
   else if (file == "EspFileName")     { ext += ".esp";  }
   else if (file == "MoFileName")      { ext += ".mo";   }
   else if (file == "DensityFileName") { ext += ".hf";   }
   else if (file == "RunFileName")     { ext += ".run";  }
   else if (file == "BatchFileName")   { ext += ".bat";  }
   else if (file == "ErrorFileName")   { ext += ".err";  }
   else { QLOG_WARN() << "Invalid file extension requested" << file; }

   return ext;
}

} } // end namespace IQmol::Process

