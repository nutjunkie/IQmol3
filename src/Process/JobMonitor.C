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

#include "JobMonitor.h"
#include "QueueResourcesList.h"
#include "QueueResourcesDialog.h"
#include "ServerRegistry.h"
#include "Server.h"
#include "Preferences.h"
#include "NetworkException.h"
#include "QMsgBox.h"
#include "QsLog.h"
#include "FileDialog.h"
#include "Layer/FileLayer.h"

#include <QCloseEvent>
#include <QInputDialog>
#include <QShowEvent>
#include <QHeaderView>
#include <QFileDialog>
#include <QDir>
#include <QSet>
#include <QDate>

#include <QDebug>


namespace IQmol {
namespace Process {

// These need to be better handled
void CleanUpQChem(Job*);
void FilterQChemFields(Job*);
void SetQChemFileNames(Job*);


JobMonitor* JobMonitor::s_instance = 0;

QMap<Job*, QTableWidgetItem*> JobMonitor::s_jobMap = QMap<Job*, QTableWidgetItem*>();

JobList JobMonitor::s_deletedJobs = QList<Job*>();
   

JobMonitor& JobMonitor::instance()
{
   if (s_instance == 0) {
      s_instance = new JobMonitor(0);
      atexit(JobMonitor::destroy);
   }
   return *s_instance;
}


void JobMonitor::destroy()
{
   s_instance->saveJobListToPreferences();
   JobList jobs(s_jobMap.keys());

   for (auto job : jobs) delete job;
   for (auto job : s_deletedJobs) delete job;
}


JobMonitor::JobMonitor(QWidget* parent) : QMainWindow(parent)
{
   m_ui.setupUi(this);

   setStatusBar(0);
   initializeMenus();

   QTableWidget* table(m_ui.processTable);
   table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
   table->horizontalHeader()->setStretchLastSection(true);
   table->verticalHeader()->setDefaultSectionSize(fontMetrics().lineSpacing() + 5);
   table->setSortingEnabled(false);
   table->setColumnWidth(0,140);
   table->setColumnWidth(1, 80);
   table->setColumnWidth(2,100);
   table->setColumnWidth(3,100);
   table->setColumnWidth(4,120);
      
   // Set up the context menu handler
   table->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(m_ui.processTable, SIGNAL(customContextMenuRequested(QPoint const&)),
      this, SLOT(contextMenu(QPoint const&)));

   // ...and the update timer;
   m_updateTimer.setInterval(1000);  // 1000 milliseconds
   connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(updateTable()));
}


void JobMonitor::initializeMenus() 
{
   QMenuBar* menubar(menuBar());
   QAction*  action;
   QMenu*    menu;

   menubar->clear();
   menu = menubar->addMenu(tr("File"));
      action = menu->addAction(tr("Reconnect Servers"));
      connect(action, SIGNAL(triggered()), this, SLOT(reconnectServers()));

      action = menu->addAction(tr("Remove All Processes"));
      connect(action, SIGNAL(triggered()), this, SLOT(removeAllJobs()));

      action = menu->addAction(tr("Close"));
      connect(action, SIGNAL(triggered()), this, SLOT(close()));
      action->setShortcut(QKeySequence::Close);
}


void JobMonitor::showEvent(QShowEvent* event)
{
   m_updateTimer.start();
   event->accept();
}


void JobMonitor::closeEvent(QCloseEvent* event)
{
   m_updateTimer.stop();
   event->accept();
}


void JobMonitor::saveJobListToPreferences() const
{
   JobList jobs(s_jobMap.keys());
   QVariantList list;

   for (auto const job : jobs) list.append(job->toQVariant());

   Preferences::JobMonitorList(list);
}


void JobMonitor::loadJobListFromPreferences()
{
   QVariantList list(Preferences::JobMonitorList());
   QLOG_INFO() << "Loading" << list.size() << "jobs from preferences file";
   if (list.isEmpty()) return;

   QDateTime cutoff = QDateTime::currentDateTime();
   cutoff = cutoff.addDays(-Preferences::DaysToRememberJobs());

   bool remoteJobsActive(false);
   JobList jobs;

   for (auto qvar : list) {
       Job* job(new Job(qvar));
       job->dump();
       
       QDateTime submit(QDateTime::fromSecsSinceEpoch(job->get<qint64>("SubmitTime")));

       if (submit < cutoff) {
          delete job; 
          continue;
       }

       jobs.append(job);

       if (job->isActive()) {
          job->setStatus(JobInfo::Unknown);
          Server* server = ServerRegistry::instance().find(job->serverName());
          if (server) {
             server->watchJob(job);
             if (server->isLocal()) {
                server->open();
                server->query(job);
             }else {
                remoteJobsActive = true;
             }
          }else {
             QLOG_WARN() << "Unable to locate server:" << job->serverName();
          }
       }
   }

   appendToTable(jobs);

   if (remoteJobsActive) {
      QString msg("IQmol found processes on remote servers that were still active "
         "in the last session. \n\nWould you like to reconnect to the server(s)?");
      if (QMsgBox::question(0, "IQmol", msg,
         QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) reconnectServers();
   }
}


Job* JobMonitor::getSelectedJob(QTableWidgetItem* item)
{
   QTableWidget* table(m_ui.processTable);
   
   if (item == 0) {
      QList<QTableWidgetItem*> items(table->selectedItems());
      if (items.isEmpty()) return 0;
      item = items.first();
   }

   item = table->item(item->row(), 0);
   JobList list(s_jobMap.keys(item));

   return list.isEmpty() ? 0 : list.first();  // and only
}


void JobMonitor::reconnectServers()
{
   QSet<QString> servers;
   for (auto job : s_jobMap.keys()) servers.insert(job->serverName());

   try {
      ServerRegistry::instance().closeAllConnections();
      ServerRegistry::instance().connectServers(servers.values());

   }catch (Exception& ex) {
      postUpdateMessage("");
      QMsgBox::warning(this, "IQmol", ex.what());
   }
}


// ---------- Submit ----------

void JobMonitor::submitJob(JobInfo* jobInfo)
{
   QString serverName(jobInfo->get<QString>("ServerName"));
   qDebug() << "writing to server : " << serverName;
   Server* server(ServerRegistry::instance().find(serverName));

   if (!server) {
      QString msg("Invalid server: ");
      msg += serverName;
      QMsgBox::warning(0, "IQmol", msg);
      return;
   }

   try {
      // stop the update timer while we are doing this
      BlockServerUpdates bs(server);
      postUpdateMessage("Connecting to server...");

      if (!server->open()) {
         QString msg("Failed to connnect to server ");
         msg += server->name();
         msg += ":\n" + server->message();
         QMsgBox::warning(0, "IQmol", msg);
         postUpdateMessage("");
         return;
      }

      if (!getWorkingDirectory(server, jobInfo)) {
         postUpdateMessage("");
         return;
      }

      if (server->needsResourceLimits()) {
         postUpdateMessage("Obtaining queue information...");
         if (!getQueueResources(server, jobInfo)) {
            postUpdateMessage("");
            return;
         }
      }

      // This should only really be done when the server is first opened.
      connect(server, SIGNAL(jobSubmissionFailed(Job*)),
         this, SLOT(jobSubmissionFailed(Job*)), Qt::UniqueConnection);

      connect(server, SIGNAL(jobSubmissionSuccessful(Job*)),
         this, SLOT(jobSubmissionSuccessful(Job*)), Qt::UniqueConnection);

      postUpdateMessage("Submitting job");
      qDebug() << "Submitting job";

      Job* job(new Job(*jobInfo));
      SetQChemFileNames(job);
      server->submit(job);
      qDebug() << "Job submitted";
      jobAccepted();  // Closes the QUI window

   }catch (Network::AuthenticationCancelled& ex) {
      server->closeConnection();
      qDebug() << "JobMonitor: authentication cancelled";

   }catch (Network::AuthenticationError& ex) {
      server->closeConnection();
      postUpdateMessage("");
      QMsgBox::warning(0, "IQmol", "Invalid username or password");

   }catch (Network::NetworkTimeout& ex) {
      server->closeConnection();
      postUpdateMessage("");
      QMsgBox::warning(0, "IQmol", "Network timeout");

   }catch (Exception& ex) {
      postUpdateMessage("");
      QMsgBox::warning(0, "IQmol", ex.what());

   }catch (...) {
      postUpdateMessage("");
      QMsgBox::warning(0, "IQmol", "Error encountered");
   }
}


bool JobMonitor::getWorkingDirectory(Server* server, JobInfo* jobInfo)
{
   QString dirPath;

   postUpdateMessage("Determining working directory...");

   if (server->isLocal()) {
      dirPath = Preferences::LastFileAccessed();
      QFileInfo info(dirPath);
      if (info.isFile()) dirPath = info.path();
#ifndef Q_OS_WIN32
      dirPath += "/" + jobInfo->get<QString>("BaseName");
#endif
      bool allowSpace(false);
      if (!getLocalWorkingDirectory(dirPath, allowSpace)) return false;
   }else {
      dirPath = jobInfo->get<QString>("BaseName");
      if (!getRemoteWorkingDirectory(server, dirPath)) return false;
   }

   QDir dir(dirPath);

   QString basename(dir.dirName());
   jobInfo->set("BaseName",basename);
  
   jobInfo->set("RemoteWorkingDirectory", dirPath);
   if (server->isLocal()) {
      jobInfo->set("LocalWorkingDirectory", dirPath);
   }

   return true;
}


bool JobMonitor::getRemoteWorkingDirectory(Server* server, QString& name)
{
   QString message;
   QString path;

   if (server->isWebBased()) {
      postUpdateMessage("Obtaining job name");
      message = "Job name:";
   }else {
      message = "Working directory on " + server->name() +":";
      path = server->configuration().value(ServerConfiguration::WorkingDirectory);
      path += "/";
   }

   QString pathName(path + name);
   bool exists(false);

   do {
      bool okPushed(false);
      name = QInputDialog::getText(0, "IQmol", message, QLineEdit::Normal, name, &okPushed);

      while (name.endsWith("/"))  { name.chop(1); }
      while (name.endsWith("\\")) { name.chop(1); }
      name = name.trimmed();

      if (!okPushed || name.isEmpty()) return false;

      pathName = path + name;

      // clean the path
      QDir dir(pathName);
      pathName = dir.path();

      exists = server->exists(pathName);
      QString msg("Directory " + name + " exists.  Overwrite?");
      if (exists && QMsgBox::question(0, "IQmol", msg) == 
         QMessageBox::Ok) {
         exists = false;
       }
   } while (exists);

   if (!server->makeDirectory(pathName)) {
      QString msg("Failed to create directory: ");
      msg += pathName;
      QMsgBox::warning(0, "IQmol", msg);
      return false;
   }
   
   name = pathName;
   return true;
}


bool JobMonitor::getLocalWorkingDirectory(QString& dirName, bool allowSpace)
{
   QDir dir(dirName);
   dir.setFilter(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);

   while (true) {
      QString dirName(dir.dirName());
      dir.cdUp();
      QString dirPath(dir.path());

      QFileDialog dialog(this , "Save As", dirPath);
      dialog.selectFile(dirName);

      // The following may need to be switched depending on which version of
      // the Qt libraries are being used.
      dialog.setOption(QFileDialog::ShowDirsOnly, true);
#ifdef Q_OS_MAC
      dialog.setOption(QFileDialog::DontUseNativeDialog, true);
#endif
      dialog.setFileMode(QFileDialog::Directory);
      // dialog.setFileMode(QFileDialog::DirectoryOnly);

      if (dialog.exec() == QDialog::Rejected) return false;

      QStringList list(dialog.selectedFiles());
      if (list.isEmpty()) return false; 

      dir.setPath(list.first());
      if (dir.path().isEmpty()) return false;

      if (dir.dirName().contains(" ") && !allowSpace) {
         // This situation should only arise when we are submitting a QChem
         // job locally, in which case this is effectively the job name.
         // We can't have a space in the name because QChem will barf.
         QMsgBox::warning(0, "IQmol", "Directory name cannot contain spaces");
      }else {
         if (dir.count() == 0) break;

         QString msg("Directory ");
         msg += dir.dirName() + " exists, overwrite?";

         if (QMsgBox::question(0, "IQmol", msg,
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            // We don't actually overwrite the directory, only the files within
            // the directory with the same name as the ones we create.
            break;
         }   
      }   
   }   

   dirName = dir.path();
   Preferences::LastFileAccessed(dirName);
   return true;
}


bool JobMonitor::getQueueResources(Server* server, JobInfo* jobInfo)
{
   ServerConfiguration& configuration(server->configuration());
   QVariantList qvar(configuration.queueResourcesList());
   QueueResourcesList list(qvar);

   if (list.isEmpty()) {
      QLOG_DEBUG() << "QueueResources List is empty, getting queue information";
      QString info(server->queueInfo());
      QLOG_TRACE() << info;

      ServerConfiguration::QueueSystemT queueSystem(configuration.queueSystem());

      switch (queueSystem) {  
         case ServerConfiguration::Basic:
            // nothing to parse
            break;
         case ServerConfiguration::PBS:
            list.fromPbsQueueInfoString(info);
            break;
         case ServerConfiguration::SGE:
             list.fromSgeQueueInfoString(info);
            break;
         case ServerConfiguration::SLURM:
            list.fromSlurmQueueInfoString(info);
            break;
         case ServerConfiguration::Web:
         case ServerConfiguration::AWS:
            // nothing to parse
            break;
      }
   }

   postUpdateMessage("Setting job resources");
   QueueResourcesDialog dialog(&list, QApplication::activeWindow());
   if (dialog.exec() == QDialog::Rejected) return false;
   configuration.setValue(ServerConfiguration::QueueResources,list.toQVariantList());
   ServerRegistry::save();

   jobInfo->set("QueueName", dialog.queue());
   jobInfo->set("WallTime",dialog.walltime());
   jobInfo->set("Memory", dialog.memory());
   jobInfo->set("Scratch", dialog.scratch());
   jobInfo->set("Ncpus", dialog.ncpus());

   return true;
}


void JobMonitor::jobSubmissionSuccessful(Job* job)
{
   appendToTable(job);
   FilterQChemFields(job);
   saveJobListToPreferences();
}


void JobMonitor::jobSubmissionFailed(Job* job)
{
   if (!job) return;
   QString msg("Job submission failed");
   if (!(job->message().isEmpty())) msg += ":\n" + job->message();
   QMsgBox::warning(0, "IQmol", msg);
   s_deletedJobs.append(job);
}


// ---------- Add/Remove Jobs ----------

bool timeOrder(Job const* a, Job const* b)
{
    return a->get<qint64>("SubmitTime") < b->get<qint64>("SubmitTime");
}


void JobMonitor::appendToTable(JobList& jobs)
{
   qSort(jobs.begin(), jobs.end(), timeOrder);
   for (auto job : jobs) appendToTable(job);
}


void JobMonitor::appendToTable(Job* job) 
{
   if (!job) return;

   QTableWidget* table(m_ui.processTable);
   int row(table->rowCount());
   table->setRowCount(row+1);

   int ncol(table->columnCount());
   Q_ASSERT(ncol == 5);
   for (int i = 0; i < ncol; ++i) {
       table->setItem(row, i, new QTableWidgetItem());
   }

   unsigned runTime(job->runTime());
   QString status(JobInfo::toString(job->jobStatus()));
   qint64 epoch(job->get<qint64>("SubmitTime"));
   QDateTime submit(QDateTime::fromSecsSinceEpoch(epoch));

   QString submitField;
   if (submit.date() == QDate::currentDate()) {
      submitField = submit.time().toString("h:mm:ss");  
   }else {
      submitField = submit.date().toString("d MMM");
   }

   table->item(row, 0)->setText(job->jobName());
   table->item(row, 1)->setText(job->serverName());
   table->item(row, 2)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
   table->item(row, 2)->setText(submitField);
   table->item(row, 3)->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
   if (runTime) table->item(row, 3)->setText(Util::Timer::formatTime(runTime));
   table->item(row, 4)->setText(status);
   table->item(row, 4)->setToolTip(job->message());

   s_jobMap.insert(job, table->item(row,0));

   connect(job, SIGNAL(updated()),  this, SLOT(jobUpdated()));
   connect(job, SIGNAL(finished()), this, SLOT(jobFinished()));
   // Gromacs branch had this deleted - why?
   saveJobListToPreferences();
}


void JobMonitor::removeJob()
{
   removeJob(getSelectedJob());
}


void JobMonitor::removeJob(Job* job)
{
   if (!job) return;

   QTableWidgetItem* item(s_jobMap.value(job));
   if (!item) return;

   m_ui.processTable->removeRow(item->row());
   s_jobMap.remove(job);
   s_deletedJobs.append(job);

   disconnect(job, SIGNAL(updated()),  this, SLOT(jobUpdated()));
   disconnect(job, SIGNAL(finished()), this, SLOT(jobFinished()));

   Server* server = ServerRegistry::instance().find(job->serverName());
   if (server) server->unwatchJob(job);
   saveJobListToPreferences();
}


void JobMonitor::on_clearListButton_clicked(bool)
{
   bool statusCheck(true);
   clearJobTable(statusCheck);
}


void JobMonitor::removeAllJobs()
{
   QString msg("Remove all jobs from the list?\n\n");
   msg += "This will stop monitoring of all jobs, but will not kill running processes";
   if (QMsgBox::question(this, "IQmol", msg) == QMessageBox::Cancel) return;

   bool statusCheck(false);
   clearJobTable(statusCheck);
}


void JobMonitor::clearJobTable(bool const finishedOnly)
{
   JobList list(s_jobMap.keys());
   JobList::iterator iter;

   for (iter = list.begin(); iter != list.end(); ++iter) {
       Job* job(*iter);
       if (job->isActive() && finishedOnly) {
          // Do nothing
       }else {
          removeJob(job);
       }
   }
}


// ---------- Job updatess ----------

void JobMonitor::updateTable()
{
   for (auto job : s_jobMap.keys()) reloadJob(job);
}


void JobMonitor::reloadJob(Job* job)
{
   if (!job) return;

   if (!s_jobMap.contains(job)) {
      QLOG_WARN() << "Update called on unknown Job";
      return;
   }

   QTableWidgetItem* item(s_jobMap.value(job));
   QTableWidget* table(m_ui.processTable);

   // Only update the runtime and status columns.  These come from the cached
   // results in the Job object, the Server is responsible for updating these
   // according to the updateInterval
   unsigned time(job->runTime());
   if (time) table->item(item->row(), 3)->setText(Util::Timer::formatTime(time));
   if (job->jobStatus() == JobInfo::Copying) {
      table->item(item->row(), 4)->setText(job->copyProgressString());
   }else {
      QString status(JobInfo::toString(job->jobStatus()));
      table->item(item->row(), 4)->setText(status);
   }
   table->item(item->row(), 4)->setToolTip(job->message());
}


void JobMonitor::jobUpdated()
{
   Job* job(qobject_cast<Job*>(sender()));
   reloadJob(job);
   saveJobListToPreferences();
}


void JobMonitor::jobFinished()
{
   Job* job = qobject_cast<Job*>(sender());
   if (!job || job->isActive()) return;

   if (job->get<bool>("LocalFilesExist")) {
      CleanUpQChem(job);
      reloadJob(job);

      if (job->jobStatus() == JobInfo::Error) {
         QString msg(job->jobName() + " failed:\n");
         msg += job->message();
         QMsgBox::warning(0, "IQmol", msg);
      }else {
         resultsAvailable(job->get<QString>("LocalWorkingDirectory"),
                          job->get<QString>("BaseName"),
                          job->get<qint64>("MoleculePointer"));
      }
     
   }else {
      QString msg("Job " + job->jobName() + " finished.\n");
      msg += "Copy results from server?";
      if (QMsgBox::question(0, "IQmol", msg, 
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
         copyResults(job);
      }
   }
}


// --------------- Context Menu Actions ---------------
void JobMonitor::contextMenu(QPoint const& pos)
{
   QTableWidget* table(m_ui.processTable);
   QTableWidgetItem* item(table->itemAt(pos));
   if (!item) return;
   Job* job(getSelectedJob(item));
   if (!job) return;

   QMenu *menu = new QMenu(this);
   QAction* kill;
   JobInfo::Status status(job->jobStatus());

   switch (status) {
      case JobInfo::Queued:   
         kill = menu->addAction("Delete Job From Queue", this, SLOT(killJob()));
         break;
      case JobInfo::Copying:   
         kill = menu->addAction("Cancel Copy", this, SLOT(cancelCopy()));
         break;
      default:
         kill = menu->addAction("Kill Job", this, SLOT(killJob()));
         break;
   }

   QAction* remove = menu->addAction(tr("Remove Job"),               this, SLOT(removeJob()));
   QAction* query  = menu->addAction(tr("Query Job"),                this, SLOT(queryJob()));
   QAction* view   = menu->addAction(tr("View Output File"),         this, SLOT(viewOutput()));
   QAction* open   = menu->addAction(tr("Open Results"),             this, SLOT(openResults()));
   QAction* copy   = menu->addAction(tr("Copy Results From Server"), this, SLOT(copyResults()));

   kill->setEnabled(false);
   query->setEnabled(false);
   remove->setEnabled(false);
   view->setEnabled(false);
   open->setEnabled(false);
   copy->setEnabled(false);

   switch (status) {
      case JobInfo::NotRunning:
         break;

      case JobInfo::Queued:
         kill->setEnabled(true);
         query->setEnabled(true);
         break;

      case JobInfo::Running:
         kill->setEnabled(true);
         query->setEnabled(true);
         break;

      case JobInfo::Suspended:
         kill->setEnabled(true);
         query->setEnabled(true);
         break;

      case JobInfo::Unknown:
         remove->setEnabled(true);
         query->setEnabled(true);
         break;

      case JobInfo::Killed:
         remove->setEnabled(true);
         break;

      case JobInfo::Error:
         remove->setEnabled(true);
         copy->setEnabled(true);
         break;

      case JobInfo::Finished:
         remove->setEnabled(true);
         copy->setEnabled(true);
         break;

      case JobInfo::Copying:
         kill->setEnabled(true);
         break;
   }

   Server* server = ServerRegistry::instance().find(job->serverName());
   if (server && server->isLocal()) {
      copy->setEnabled(false);
   }

   if (job->get<bool>("LocalFilesExist")) {
      view->setEnabled(true);
      if (status == JobInfo::Finished) open->setEnabled(true);
   }

   menu->exec(table->mapToGlobal(pos));
   delete menu;
}


void JobMonitor::on_processTable_cellDoubleClicked(int, int)
{
   Job* job(getSelectedJob());
   if (!job) return;
   bool localFilesExist(job->get<bool>("LocalFilesExist"));

   switch (job->jobStatus()) {
      case JobInfo::Error:
         if (localFilesExist) {
            viewOutput(job);
         }else {
            QString msg = "Job failed:\n";
            msg += job->message();
            QMsgBox::information(0, "IQmol", msg);
         }
         break;

      case JobInfo::Finished:
         if (localFilesExist) {
            openResults(job);
         }else {
            if (QMsgBox::question(0, "IQmol", "Copy results from server?",
               QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
               copyResults(job);
            }
          }
         break;

      default:
         queryJob(job);
         break;
   }
}


void JobMonitor::cancelCopy()
{
   Job* job(getSelectedJob());
   if (!job) return;

   JobInfo::Status status(job->jobStatus());
   if (status != JobInfo::Copying) {
      QLOG_DEBUG() << "Cancel copy called on non-copy job";
      return;
   }

   try {
      Server* server = ServerRegistry::instance().find(job->serverName());
      if (server) server->cancelCopy(job);
   } catch (Exception& ex) {
      QMsgBox::warning(0, "IQmol", ex.what());
   }
}


void JobMonitor::killJob()
{    
   Job* job(getSelectedJob());
   if (!job) return;
  
   JobInfo::Status status(job->jobStatus());

   QString msg;
   if (status == JobInfo::Queued) {
      msg = "Are you sure you want to remove the job ";
      msg += job->jobName() + " from the queue?";
   }else {
      msg = "Are you sure you want to terminate the job ";
      msg += job->jobName() + "?";
   }    

   if (QMsgBox::question(0, "IQmol", msg) == QMessageBox::Cancel) return;
    
   try {
      Server* server = ServerRegistry::instance().find(job->serverName());
      if (server) server->kill(job);
   } catch (Exception& ex) {
      QMsgBox::warning(0, "IQmol", ex.what());
   }
}


void JobMonitor::openResults()
{
  openResults(getSelectedJob());
}


void JobMonitor::openResults(Job* job)
{
   if (!job) return;
   resultsAvailable(job->get<QString>("LocalWorkingDirectory"),
                    job->get<QString>("BaseName"),
                    job->get<qint64>("MoleculePointer"));
}


void JobMonitor::viewOutput()
{    
  viewOutput(getSelectedJob());
}


void JobMonitor::viewOutput(Job* job)
{
   if (!job) return;

   QFileInfo output(job->getLocalFilePath("OutputFileName"));

   if (!output.exists()) {
      QMsgBox::warning(this,"IQmol", "Output file no longer exists");
      job->set("LocalFilesExist", false);
      return;
   }   

   Layer::File* file = new Layer::File(output.filePath());

   if (job->jobStatus() == JobInfo::Running) {
      file->tail();
   }else {
      file->configure();
   }   
}


void JobMonitor::copyResults()
{    
   copyResults(getSelectedJob());
}


void JobMonitor::copyResults(Job* job)
{    
   if (!job) return;

   try {
      QString dirPath(job->get<QString>("LocalWorkingDirectory"));
      QFileInfo info(dirPath);

      if (job->get<bool>("LocalFilesExist") && info.exists()) {
         QString msg("Results are in the directory:\n\n");
         msg += dirPath + "\n\nDownload results again?";
         if (QMsgBox::question(this, "IQmol", msg) == QMessageBox::Cancel) return;
      }

      // clean the path
      QDir dir(dirPath);
      dirPath = dir.path();
   
      bool allowSpace(true);
      if (!getLocalWorkingDirectory(dirPath, allowSpace)) return;

      job->set("LocalWorkingDirectory", dirPath);

      Server* server = ServerRegistry::instance().find(job->serverName());
      if (!server) throw Exception("Invalid server");
      server->copyResults(job);

   } catch (Exception& ex) {
      QMsgBox::warning(0, "IQmol", ex.what());
   }
}


void JobMonitor::queryJob()
{  
   queryJob(getSelectedJob());
}


void JobMonitor::queryJob(Job* job)
{    
   if (!job) return;

   try {
      Server* server = ServerRegistry::instance().find(job->serverName());
      if (!server) throw Exception("Invalid server");
      server->query(job);
   } catch (Exception& ex) {
      QMsgBox::warning(0, "IQmol", ex.what());
   }
}

} } // end namespace IQmol::Process
