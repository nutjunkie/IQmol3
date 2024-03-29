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

#include "FileLayer.h"
#include <QFileInfo>
#include <QFile>


namespace IQmol {
namespace Layer {

File::File(Data::File const& file) : m_filePath(file.path()), m_configurator(*this)
{
   QFileInfo fileInfo(m_filePath);
   setText(fileInfo.fileName());
   setConfigurator(&m_configurator);
}


File::File(QString const& filePath) : m_filePath(filePath), m_configurator(*this)
{
   QFileInfo fileInfo(m_filePath);
   setText(fileInfo.fileName());
   setConfigurator(&m_configurator);
}


void File::tail(int const interval) 
{
   configure();
   m_configurator.tail(interval);
}


QString File::contents() const
{
   QString contents;
   QFile qfile;
   qfile.setFileName(m_filePath);

   if (qfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
      contents = QString(qfile.readAll());
      qfile.close();
   }

   return contents;
}

} } // end namespace IQmol::Layer
