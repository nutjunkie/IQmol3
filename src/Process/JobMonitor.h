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

#include "ui_JobMonitor.h"
#include "Job.h"

#include <QTimer>


class QCloseEvent;
class QShowEvent;

namespace IQmol {
namespace Process {

   class Server;

   // The JobMonitor handles the submission and monitoring of external 
   // calculations such as Q-Chem jobs.  These jobs could be run on 
   // either the local or remote machine.

   class JobMonitor : public QMainWindow {

      Q_OBJECT

      public:
         static JobMonitor& instance();
         void   loadJobListFromPreferences();

      public Q_SLOTS:
         void submitJob(JobInfo*);

      Q_SIGNALS:
         /// This signal is emitted only when a job has finished successfully.
         void resultsAvailable(QString const& path, QString const& filter, qint64 molPtr);
         void jobAccepted();

         void postUpdateMessage(QString const&);

      protected:
         void closeEvent(QCloseEvent* event);
         void showEvent(QShowEvent* event);

      private Q_SLOTS:

         void jobSubmissionSuccessful(Job*);
         void jobSubmissionFailed(Job*);
         void on_clearListButton_clicked(bool);
         void on_processTable_cellDoubleClicked(int, int);

         /// Used to remove all jobs listed in the monitor.  This is triggered
         /// by a MainWindow menu action and may be useful there are rogue
         /// processes on the list which are causing problems and need to be
         /// removed.
         void removeAllJobs();

         /// This is really a pseudo-update to the entries in the monitor, it
         /// simply grabs the values that are cached in the Job objects.  The
         /// individual Servers decide how to handle the real updates, which 
         /// allows network requests to be minimized.  
         void updateTable();

         void reconnectServers();
         void jobUpdated();
         void jobFinished();

         // Context menu actions
         void contextMenu(QPoint const& position);
         void cancelCopy();
         void killJob();
         void removeJob();
         void queryJob();
         void copyResults();
         void viewOutput();
         void openResults();

      private:
         static QMap<Job*, QTableWidgetItem*> s_jobMap;
         static QList<Job*> s_deletedJobs;

         void initializeMenus();
         void saveJobListToPreferences() const;

         void appendToTable(Job*);
         void appendToTable(JobList&);
         void reloadJob(Job* job);
         void removeJob(Job*);
         void queryJob(Job* job);
         void copyResults(Job* job);
         void viewOutput(Job* job);
         void openResults(Job* job);

         bool getQueueResources(Server*, JobInfo*);
         Job* getSelectedJob(QTableWidgetItem* item = 0);

         bool getWorkingDirectory(Server*, JobInfo*);
         bool getRemoteWorkingDirectory(Server*, QString& suggestion);
         bool getLocalWorkingDirectory(QString& suggestion, bool allowSpace);
         bool renameFile(QString const& oldName, QString const& newName);

		 /// Clears the jobs from the monitor and their servers.  If
		 /// finishedOnly then only the processes that have status 
         void clearJobTable(bool const finishedOnly);

         // Singleton stuff
         JobMonitor(QWidget* parent = 0);
         explicit JobMonitor(JobMonitor const&); // prevent copying
         ~JobMonitor() { }
         static JobMonitor* s_instance;
         static void destroy();

         Ui::JobMonitor m_ui;
         QTimer m_updateTimer;
   };

} } // end namespace IQmol::Process
