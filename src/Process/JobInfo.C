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

#include "JobInfo.h"
#include "QMapRange.h"
#include <QDate>


namespace IQmol {
namespace Process {


QString JobInfo::toString(Status const status) 
{
   QString s;
   switch (status) {
      case NotRunning:  s = "Not Running";  break;
      case Queued:      s = "Queued";       break;
      case Running:     s = "Running";      break;
      case Suspended:   s = "Suspended";    break;
      case Killed:      s = "Killed";       break;
      case Error:       s = "Error";        break;
      case Finished:    s = "Finished";     break;
      case Copying:     s = "Copying";      break;
      default:          s = "Unknown";      break;
   }   
   return s;
}


bool JobInfo::isActive(Status const status)
{
   bool active(true);

   switch (status) {
      case NotRunning:
      case Queued:
      case Running:
      case Suspended:
      case Unknown:
      case Copying:
         active = true;
         break;

      case Killed:
      case Error:
      case Finished:
         active = false;      
         break;
   }
   
   return active;
}



JobInfo::JobInfo() 
{
   m_jobStatus = NotRunning;
   set("JulianDay", QDate::currentDate().toJulianDay());
}



QVariantList JobInfo::toQVariantList() const
{
   QVariantList list;
   list << QVariant((int)m_jobStatus);

   // These are the fields that we do not need to serialize
   QStringList filterFields = { 
       "InputString", 
       "Coordinates", 
       "Constraints", 
       "CoordinatesFsm", 
       "ScanCoordinates",
       "OnsagerRadius",
       "EfpParameters",
       "EfpFragments",
       "ExternalCharges",
       "Charge",
       "Multiplicity",
       "NumElectrons", 
   };

   QVariantMap map(m_jobData);
   for (auto field : filterFields) {
       map.remove(field);
   }

   list << QVariant(map);

   QLOG_DEBUG() << "------ Serializing JobInfo ------";
   for (auto [key, value] : QMapRange<QVariantMap>(map)) {
       QLOG_DEBUG() << key << " -> " << value;
   }
   return list;
}


bool JobInfo::fromQVariantList(QVariantList const& list)
{
   bool ok(list.size() == 2);      
   if (!ok) return false;

   ok = list[0].canConvert<int>();  
   if (!ok) return false;
   m_jobStatus = static_cast<Status>(list[0].toInt());

   ok = list[1].canConvert<QVariantMap>();  
   if (!ok) return false;
   m_jobData = list[1].toMap();

   QLOG_DEBUG() << "------ Reading JobInfo ------";
   QLOG_DEBUG() << toString(m_jobStatus);
   QLOG_DEBUG() << m_jobData;

   return ok;
}


void JobInfo::copy(JobInfo const& that)
{
   m_jobStatus  = that.m_jobStatus;
   m_jobData    = that.m_jobData;
}


QString JobInfo::getRemoteFilePath(QString const& key) const
{
   QString path(get<QString>("RemoteWorkingDirectory"));
   if (!path.endsWith("/")) path += "/";
   path += get<QString>(key);
   return path;
}


QString JobInfo::getLocalFilePath(QString const& key) const
{
   QString path(get<QString>("LocalWorkingDirectory"));
   if (!path.endsWith("/")) path += "/";
   path += get<QString>("BaseName");
   return path;
}


void JobInfo::dump() const
{
   QLOG_DEBUG() << "--- JobInfo ---";
   for (auto i = m_jobData.begin(); i != m_jobData.end(); ++i) {
       QLOG_DEBUG() << i.key() << " = " << i.value();
   }
}

} } // end namespace IQmol::Process
