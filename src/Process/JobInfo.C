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


// Allows delegation of message passing to Connection classes without creating
// a dependency on libProcess.a
JobInfo::Status JobInfo::fromString(QString const& s) 
{
   Status status(Unknown);

   if (s == "Not Running") { status = NotRunning;  } else
   if (s == "Queued")      { status = Queued;      } else
   if (s == "Running")     { status = Running;     } else
   if (s == "Suspended")   { status = Suspended;   } else
   if (s == "Killed")      { status = Killed;      } else
   if (s == "Error")       { status = Error;       } else
   if (s == "Finished")    { status = Finished;    } else
   if (s == "Copying")     { status = Copying;     }

   return status;
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


QVariantList JobInfo::toQVariantList() const
{
   QVariantList list;
   list << QVariant((int)m_jobStatus);
   list << QVariant(m_jobData);
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

/*
   for (auto [key, value] : QMapRange<QVariantMap>(m_jobData)) {
       QLOG_DEBUG() << key << " -> " << value;
   }
*/

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
   QLOG_WARN() << "Calling JobInfo::getLocalFilePath with key" << key;
   QString path(get<QString>("LocalWorkingDirectory"));
   if (!path.endsWith("/")) path += "/";
   path += get<QString>(key);
   return path;
}


void JobInfo::dump() const
{
   QLOG_DEBUG() << "------ JobInfo Contents ------";
   QLOG_DEBUG() << toString(m_jobStatus);
   for (auto i = m_jobData.begin(); i != m_jobData.end(); ++i) {
       QLOG_DEBUG() << i.key().leftJustified(24, QLatin1Char(' ')) << " = " << i.value();
   }
}

} } // end namespace IQmol::Process
