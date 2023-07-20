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

Job::~Job()
{
  // Let the server know we don't exist any more.
  deleted(this);
}


QVariant Job::toQVariant() const
{
   return QVariant(toQVariantList());
}


bool Job::fromQVariant(QVariant const& qvar)
{
   QVariantList list(qvar.toList());
   bool ok(fromQVariantList(list));

   if (exists("RunTime")) resetTimer(get<unsigned>("RunTime"));

   return ok;
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
   output.replace("${WALLTIME}", get<QString>("WallTime"));
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
         set("SubmitTime", QTime::currentTime().toString("h:mm:ss"));
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

} } // end namespaces IQmol::Process
