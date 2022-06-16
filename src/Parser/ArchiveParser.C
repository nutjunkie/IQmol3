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

#include "ArchiveParser.h"
#include "Util/QsLog.h"
#include <libarchive/api/impl/qarchive.h>

#include <QFile>
#include <QDebug>


namespace IQmol {
namespace Parser {

bool Archive::parseFile(QString const& filePath)
{
   bool okay(true);

   try {
      libarchive::impl::QArchive archive(filePath.toStdString(), libstore::FileMode::ReadOnly);
      double scf_energy(0.0);
      archive.read(libarchive::impl::schema::methods::scf::iter::energy, scf_energy);
      qDebug() << "Read archive energy as " << scf_energy;
   } catch (std::exception& e) {
      m_errors.append(e.what());
      return false;
   }

   return okay;
}

} } // end namespace IQmol::Parser
