/*******************************************************************************

  Copyright (C) 2011-2055 Andrew Gilbert

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

#include "Viewer.h"
#include "QMsgBox.h"
#include "QsLog.h"
#include "Snapshot.h"
#include "SnapshotDialog.h"
#include "FileDialog.h"
#include "Preferences.h"
#include "gl2ps.h"

#include <QDebug>


using namespace qglviewer;


namespace IQmol {

Snapshot::Snapshot(Viewer* viewer, unsigned const flags) 
 : m_viewer(viewer), 
   m_flags(flags), 
   m_counter(0), 
   m_framerate(15),
   m_antialias(1),
   m_movieProcess(0)
{
   if (m_flags & Movie) m_flags |= AutoIncrement;
}


void Snapshot::startRecord()
{
   if (m_flags & Continuous) {
      connect(&m_recordTimer, SIGNAL(timeout()), this, SLOT(capture()));
      m_recordTimer.setInterval(1000.0/m_framerate); // msec
      m_recordTimer.start();
   }else {
      connect(m_viewer, SIGNAL(animationStep()), this, SLOT(capture()));
   }
}


void Snapshot::stopRecord()
{
   if (m_flags & Continuous) {
      m_recordTimer.stop();
      disconnect(&m_recordTimer, SIGNAL(timeout()), this, SLOT(capture()));
   }else {
      disconnect(m_viewer, SIGNAL(animationStep()), this, SLOT(capture()));
   }
}


// Returns false if the user cancels the action, true otherwise.
bool Snapshot::requestFileName()
{
   QFileInfo fileInfo(Preferences::LastFileAccessed());

   QString title;
   QStringList formats;

   if (m_flags & Movie) {
      title = "Save movie as";
      fileInfo.setFile(fileInfo.dir(), "movie.mp4");
#if Q_OS_WIN
      formats << "Audio Video Interleave (*.avi)"
              << "MPEG4 Movie (*.mp4)"
              << "QuickTime Movie (*.mov)" ;
#else
      formats << "MPEG4 Movie (*.mp4)"
              << "QuickTime Movie (*.mov)"
              << "Audio Video Interleave (*.avi)";
#endif
   }else if (m_flags & Vector) {
      title = "Save vector snapshot as";
      fileInfo.setFile(fileInfo.dir(), "snapshot.pov");
      formats << "PovRay (*.pov)";

   } else {
      title = "Save snapshot as";
      fileInfo.setFile(fileInfo.dir(), "snapshot.png");
      formats << "PNG (*.png)"
              << "JPEG (*.jpg)"
              << "Tagged Image File Format (*.tiff)"
              << "24bit RBG Bitmap (*.ppm)" 
              << "Windows Bitmap (*.bmp)";
   }

   QString filter = formats.first();

   QString fileName = FileDialog::getSaveFileName(m_viewer, title,
      fileInfo.filePath(), formats.join(";;"), &filter);

   if (fileName.isEmpty()) return false;

   fileInfo.setFile(fileName);
   m_fileBaseName  = fileInfo.path() + "/" + fileInfo.completeBaseName();
   m_fileExtension = fileInfo.suffix();

   if (m_flags & Movie) {
      SnapshotVideoDialog dialog(m_viewer);
      if (dialog.exec() == QDialog::Rejected) return false;

      m_videoExtension = m_fileExtension;
      m_fileExtension  = dialog.usePNG() ? "png" : "jpg";
      m_framerate = dialog.framerate();
      if (dialog.continuousRecording()) {
         m_flags |= Continuous;
      }else {
         m_flags &= ~Continuous;
      }
      m_size = dialog.size();

   }else if (m_flags & Vector) {
      // no need to bug the user further

   }else {
      SnapshotImageDialog dialog(m_viewer);
      if (dialog.exec() == QDialog::Rejected) return false;

      m_antialias = dialog.antialias() ? 8 : 1;
      m_size = dialog.size();
      m_dpi = dialog.dpi();
   }

   return true;
}


void Snapshot::capture()
{
   if (m_fileBaseName.isEmpty()) return;
   QString fileName(m_fileBaseName);

   if (m_flags & AutoIncrement) {
      if (m_counter > 99999) {
         QLOG_WARN() << "Too many movie frames requested, resetting counter";
         resetCounter();
      }
      if (m_counter < 10000) fileName += "0";
      if (m_counter <  1000) fileName += "0";
      if (m_counter <   100) fileName += "0";
      if (m_counter <    10) fileName += "0";
      fileName += QString::number(m_counter);
      ++m_counter;
   }

   fileName += "." + m_fileExtension;

   if (m_flags & Vector) {
      m_viewer->savePovRay(fileName);
   }else {
      m_viewer->saveImage(fileName, m_size, m_dpi, m_antialias);
      m_fileNames << fileName;
   }

   Preferences::LastFileAccessed(fileName);
}


void Snapshot::makeMovie()
{
   if (m_movieProcess) {
      QMsgBox::warning(0, "IQmol", "Movie making already in progress, please wait");
      return;
   }

   QDir dir(QApplication::applicationDirPath());
   QFileInfo ffmpeg = Preferences::FFmpegPath();
   if (!ffmpeg.exists()) {
      QMsgBox::warning(0, "IQmol", "ffmpeg executable not found");
      return;
   }

   QFile movie(m_fileBaseName + "." + m_videoExtension);
   if (movie.exists()) {
      if (!movie.remove()) {
         QMsgBox::warning(0, "IQmol", "Could not remove existing file " + movie.fileName());
         return;
      }
   }


   QFileInfo fileInfo(m_fileBaseName);
   QString files = fileInfo.fileName() + "%05d." + m_fileExtension;

   QStringList args;
   args << "-r" << QString::number(int(m_framerate)) << "-i" << files
        << "-vcodec" << "libx264" << "-y" << "-an" 
        << movie.fileName()
        << "-vf" << "\"scale=trunc(iw/2)*2:trunc(ih/2)*2\"";


   m_movieProcess = new QProcess;

   m_movieProcess->setWorkingDirectory(fileInfo.path());

   connect(m_movieProcess, SIGNAL(error(QProcess::ProcessError)), 
      this, SLOT(movieError(QProcess::ProcessError)));

   connect(m_movieProcess, SIGNAL(finished(int, QProcess::ExitStatus)), 
      this, SLOT(movieFinished(int, QProcess::ExitStatus)));

   qDebug() << "Start movie making";
   qDebug() << ffmpeg.filePath() << "with args" ;
   qDebug() << args;
   m_movieProcess->start(ffmpeg.filePath(), args);

   return;
}


void Snapshot::movieFinished(int, QProcess::ExitStatus exitStatus)
{
   QString fileName(m_fileBaseName + "." + m_videoExtension);
   
   QString msg;
   switch (exitStatus) {
      case QProcess::NormalExit:
         msg = "Movie written to:\n" + fileName + "\n\nRemove temporary image files?";
         Preferences::LastFileAccessed(fileName);
         break;
      case QProcess::CrashExit:
         msg = "Failed to make movie.  Remove image files?";
         break;
   }
   
   removeImageFiles(msg);

   if (m_movieProcess) delete m_movieProcess;
   m_movieProcess = 0;
   movieFinished();
}


void Snapshot::movieError(QProcess::ProcessError error)
{
   qDebug() << "movieError(QProcess::ProcessError error) called";
   QString msg("Failed to create movie:\n");
   switch (error) {

      case QProcess::FailedToStart: 
         msg += "Process failed to start";  
         break;

      case QProcess::Crashed:       
         msg += "Process crashed";          
         break;

      case QProcess::Timedout:      
         msg += "Process timed out";
         break;

      case QProcess::WriteError:
      case QProcess::ReadError:
         msg += "I/O error";          
         break;

      default:
         msg += "Unknown error";
   }

   QMsgBox::warning(0, "IQmol", msg);

   if (m_movieProcess) m_movieProcess->deleteLater();
   movieFinished();
}


void Snapshot::removeImageFiles(QString const& msg)
{
   if (QMsgBox::question(0, "IQmol", msg,
       QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
       for (int i = 0; i < m_fileNames.size(); ++i) {
           QFile file(m_fileNames[i]);
           if (file.exists()) file.remove();
       }
   }
   m_fileNames.clear();
}




/*  ----- Deprecate -----
void Snapshot::captureVector(QString const& fileName, int const format)
{
   char ext[8];
   strcpy(ext, gl2psGetFileExtension(format));

   int opt(GL2PS_USE_CURRENT_VIEWPORT |
           GL2PS_OCCLUSION_CULL       |
           GL2PS_TIGHT_BOUNDING_BOX   |
           GL2PS_NO_BLENDING);

   writefile(format, GL2PS_SIMPLE_SORT, opt, 0, fileName.toLatin1(), ext);
}


void Snapshot::writefile(int format, int sort, int options, int nbcol,
               const char *filename, const char *extension)
{
  FILE *fp;
  char file[256];
  int state = GL2PS_OVERFLOW, buffsize = 0;
  GLint viewport[4];

  strcpy(file, filename);
  strcat(file, ".");
  strcat(file, extension);

  viewport[0] = 0;
  viewport[1] = 0;
  viewport[2] = 0;
  viewport[3] = 0;

  fp = fopen(file, "wb");

  if(!fp){
    printf("Unable to open file %s for writing\n", file);
    exit(1);
  }

  printf("Saving image to file %s... ", file);
  fflush(stdout);

  while (state == GL2PS_OVERFLOW){
    buffsize += 1024*1024;
    gl2psBeginPage(file, "gl2psTest", viewport, format, sort, options,
                   GL_RGBA, 0, NULL, nbcol, nbcol, nbcol,
                   buffsize, fp, file);
    m_viewer->update();
    state = gl2psEndPage();
  }

  fclose(fp);

  printf("Done!\n");
  fflush(stdout);
}
*/

} // end namespace IQmol
