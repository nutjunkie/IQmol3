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

#include "Job.h"
#include "QsLog.h"
#include <QVariant>
#include <QDate>


namespace IQmol {
namespace Process {



Job::Job(JobInfo const& jobInfo) 
{
   set("JulianDay",QDate::currentDate().toJulianDay());
   copy(jobInfo); 
   //m_jobName    = get<QString>("BaseName");
   //m_serverName = get<QString>("ServerName");
}


Job::~Job()
{
  // Let the server know we don't exist any more.
  deleted(this);
}


QVariant Job::toQVariant() const
{
   return QVariant(toQVariantList());
#if 0
   QVariantMap map;

   map.insert("JobName",      m_jobName);  
   map.insert("ServerName",   m_serverName);  
   map.insert("Message",      m_message);  
   map.insert("JobId",        m_jobId);  
   map.insert("SubmitTime",   m_submitTime);  
   map.insert("RunTime",      runTime());  
//   map.insert("Status",       (int)m_status);  
   map.insert("JulianDay",    m_julianDay);
   QLOG_DEBUG() << "normal part works";
if (m_jobInfo)   map.insert("jobInfo", m_jobInfo->toQVariantList());
   QLOG_DEBUG() << "in save Joblist to preferences";
   return QVariant(map);
#endif
}


bool Job::fromQVariant(QVariant const& qvar)
{
   QVariantList list(qvar.toList());
   return fromQVariantList(list);

#if 0
   if (!map.contains("JobName")) return false;
   m_jobName = map.value("JobName").toString(); 
   if (m_jobName.isEmpty()) return false;

   if (!map.contains("ServerName")) return false;
   m_serverName = map.value("ServerName").toString(); 
   if (m_serverName.isEmpty()) return false;

   if (!map.contains("Status")) return false;
//   m_status = static_cast<JobInfo::Status>(map.value("Status").toInt()); 

   if (map.contains("Message")) {
      m_message = map.value("Message").toString(); 
   }

   if (map.contains("SubmitTime")) {
      m_submitTime = map.value("SubmitTime").toString(); 
   }

   if (map.contains("JobId")) {
      m_jobId = map.value("JobId").toString(); 
   }

   if (map.contains("RunTime")) {
      resetTimer(map.value("RunTime").toUInt()); 
   }

   if (map.contains("JulianDay")) {
      m_julianDay = map.value("JulianDay").toUInt(); 
   }

   if (map.contains("JobInfo")) {
      if (!m_jobInfo->fromQVariantList(map.value("JobInfo").toList())) {
         QLOG_WARN() << "Faild to convert JobInfo from serialized Job";
      }
   }

   return true;

#endif
}


void Job::resetTimer(unsigned const seconds)
{ 
   m_timer.reset(seconds); 
}


unsigned Job::runTime() const 
{ 
   return m_timer.time(); 
}


QString Job::substituteMacros(QString const& input) const
{
   QString output(input);
   output.replace("${JOB_ID}",   get<QString>("JobId"));
   output.replace("${JOB_DIR}",  get<QString>("RemoteWorkingDirectory"));
   output.replace("${JOB_NAME}", get<QString>("BaseName"));

   output.replace("${QUEUE}",    get<QString>("QueueName"));
   output.replace("${WALLTIME}", get<QString>("wWllTime"));
   output.replace("${MEMORY}",   QString::number(get<qint64>("Memory")));
   output.replace("${JOBFS}",    QString::number(get<qint64>("Scratch")));
   output.replace("${SCRATCH}",  QString::number(get<qint64>("Scratch")));
   output.replace("${NCPUS}",    QString::number(get<qint64>("Ncpus")));

   // Check all the macros have been expanded
   if (output.contains("${")) {
      QLOG_WARN() << "Unmatched macros found in string:";
      QLOG_WARN() << input;
   }

   return output;
}


void Job::parseQueryOutput(QString const&)
{
   qDebug() << "Need to parse query string";
}


void Job::copyProgress(double fraction)
{
   int pc(100*fraction);
   m_copyProgress = "Copying (" + QString::number(pc) + "%)";
   updated();
}


void Job::setStatus(JobInfo::Status const status, QString const& message)
{
   if (status == jobStatus()) {
      if (!message.isEmpty()) m_message = message;
      updated();
      return;
   }
   setJobStatus(status);

   switch (status) {
      case JobInfo::NotRunning:
         m_message = "Job has not yet started";
         break;

      case JobInfo::Queued:
         m_message = "Job ID: " + jobId();
         m_submitTime = QTime::currentTime().toString("h:mm:ss");
         break;

      case JobInfo::Running:
         m_timer.start();
         break;

      case JobInfo::Suspended:
         m_timer.stop();
         break;

      case JobInfo::Killed:
         m_timer.stop();
         break;

      case JobInfo::Error:
         m_timer.stop();
         error();
         break;

      case JobInfo::Finished:
         m_timer.stop();
         finished();
         break;

      case JobInfo::Copying:
         m_copyProgress = "Copying";
         break;

      case JobInfo::Unknown:
         m_timer.stop();
         break;
   }

   if (!message.isEmpty()) m_message = message;
   qDebug() << "Setting Job status to" << JobInfo::toString(status) << m_message;
   updated();
}


bool Job::localFilesExist() const 
{ 
   return get<bool>("LocalFilesExist");
}

} } // end namespaces IQmol::Process
