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

#include <QVariant>
#include "QsLog.h"


namespace IQmol {
namespace Process {

   /// Data class that holds information about a Server job.  
   class JobInfo {

      public:
         enum Status { Unknown = 0,
                       NotRunning,
                       Queued, 
                       Running, 
                       Suspended, 
                       Killed,
                       Error, 
                       Finished, 
                       Copying 
                     };

         static QString toString(Status const);

         static bool isActive(Status const);

         JobInfo();

         JobInfo(JobInfo const& that) { copy(that); }

         JobInfo& operator=(JobInfo const& that) 
         {
            if (this != &that) copy(that);  return *this;
         }

         // Treat Status as a special case so we don't have to register the
         // enum with QVariant
         Status jobStatus() const { return m_jobStatus; }
         void setJobStatus(Status const jobStatus) { m_jobStatus = jobStatus; }

         bool exists(QString const& key) const { return m_jobData.contains(key); }

         template <typename T>
         void set(QString const key, T const& value) 
         {
            qDebug() << "Inserting job info:" << key << "->" << value;
            m_jobData.insert(key, QVariant(value));
         }

         template <typename T>
         T get(QString const key) const
         {
            T t;
            if (m_jobData.contains(key) && m_jobData[key].canConvert<T>()) {
               t = m_jobData[key].value<T>();
            }else {
               QLOG_WARN() << "Failed to find key in JobInfo object:" << key;
            }
            return t;
         }

         QString getRemoteFilePath(QString const& key) const;

         QString getLocalFilePath(QString const& key) const;

		 // Serialization functions, note that we filter fields such as
		 // InputFile as these are not required on reload.
         QVariantList toQVariantList() const;

         bool fromQVariantList(QVariantList const&);

         bool isActive() const { return isActive(m_jobStatus); }

         virtual void dump() const;


      protected:
         void copy(JobInfo const&);

      private:
         Status m_jobStatus;
         QVariantMap m_jobData;
   };

} } // end namespace IQmol::Process
