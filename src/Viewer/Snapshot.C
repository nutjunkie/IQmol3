/*******************************************************************************
       
  Copyright (C) 2011-2015 Andrew Gilbert
           
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
#include "Snapshot.h"
#include "Preferences.h"
#include "gl2ps.h"
#include <QImageWriter>
#include <QFileDialog>

#include <QDebug>


using namespace qglviewer;

namespace IQmol {


Snapshot::Snapshot(Viewer* viewer, int const flags) : m_viewer(viewer), m_fileFormat(PNG),
    m_flags(flags), m_counter(0), m_movieProcess(0)
{
   if (m_flags & Movie) m_flags = m_flags | AutoIncrement;
}


// Returns false if the user cancels the action, true otherwise.
bool Snapshot::requestFileName()
{
if (0) {
   m_viewer->QGLViewer::saveSnapshot(false, false);
   return true;
}

   // Determine the formats supported by Qt
   QList<QByteArray> list = QImageWriter::supportedImageFormats();
   QStringList formatsAvailable;  
   for (int i=0; i < list.size(); ++i) {
       formatsAvailable << QString(list.at(i).toLower());
   }

   // Add on those provided by the GL2PS library
   /* - not working very well
   formatsAvailable += "eps";
   formatsAvailable += "pdf";
   formatsAvailable += "svg";
   */

   QStringList extensions;
   extensions << "jpg" << "png" << "tiff" << "ppm" << "bmp" 
              << "eps" << "pdf" << "svg"  << "mov" << "mp4";

   QString filter("PNG (*.png)");  // The default image type;
   QStringList menuTexts;
   menuTexts << "JPEG (*.jpg)"
             << filter
             << "Tagged Image File Format (*.tiff)"
             << "24bit RBG Bitmap (*.ppm)" 
             << "Windows Bitmap (*.bmp)" 
             << "Encapsulated Postscript (*.eps)" 
             << "Portable Document Format (*.pdf)" 
             << "Scalable Vector Graphics (*.svg)";

   QFileInfo fileInfo(Preferences::LastFileAccessed());
   fileInfo.setFile(fileInfo.dir(), "snapshot.png");
   QString title;

   if (m_flags & Movie) {
#ifdef Q_OS_MAC
      fileInfo.setFile(fileInfo.dir(), "movie.mov");
      title = "Save movie as";
      filter = "QuickTime Movie (*.mov)";
      menuTexts << filter
                << "MPEG4 Movie (*.mp4)";
      formatsAvailable.clear();
      formatsAvailable << "mov" << "mp4";
#else
      title = "Save movie sequence as";
#endif
   }else {
      title = "Save snapshot as";
   }

   QStringList menu;
   QStringList::iterator iter;
   for (iter = formatsAvailable.begin(); iter != formatsAvailable.end(); ++iter) {
       int index(extensions.indexOf(*iter));
       if (index >= 0) {
          menu += menuTexts[index];
       }
   }

   QString fileName = QFileDialog::getSaveFileName(m_viewer, title,
      fileInfo.filePath(), menu.join(";;"), &filter);

   if (fileName.isEmpty()) return false;

   m_flags = m_flags | Overwrite;

   int index(menuTexts.indexOf(filter));

   if (!fileName.endsWith(extensions[index])) fileName += "." + extensions[index];

   switch (index) {
      case 0:  m_fileFormat = JPG;  break;
      case 1:  m_fileFormat = PNG;  break;
      case 2:  m_fileFormat = TIFF; break;
      case 3:  m_fileFormat = PPM;  break;
      case 4:  m_fileFormat = BMP;  break;
      case 5:  m_fileFormat = EPS;  break;
      case 6:  m_fileFormat = PDF;  break;
      case 7:  m_fileFormat = SVG;  break;
      case 8:  m_fileFormat = PNG;  break;
      default:
         return false;
   }

   fileInfo.setFile(fileName);
   m_fileBaseName = fileInfo.path() + "/" + fileInfo.completeBaseName();
   m_fileExtension = extensions[m_fileFormat];
   
   return true;
}


void Snapshot::capture()
{
   if (m_fileBaseName.isEmpty()) return;

   QString fileName(m_fileBaseName);
   if (m_flags & AutoIncrement) {
      if (m_counter < 1000) fileName += "0";
      if (m_counter <  100) fileName += "0";
      if (m_counter <   10) fileName += "0";
      fileName += QString::number(m_counter);
      ++m_counter;
   }

if (1) {
fileName += ".png";
//qDebug() << "Saving snapshot to" << fileName;
m_viewer->QGLViewer::saveSnapshot(fileName, true);
m_fileNames << fileName;
return;
}

   switch (m_fileFormat) {
      case JPG: 
         fileName += ".jpg";
         capture(fileName); 
         break;
      case PNG:  
         fileName += ".png";
         capture(fileName); 
         break;
      case TIFF:  
         fileName += ".tiff";
         capture(fileName); 
         break;
      case PPM:  
         fileName += ".ppm";
         capture(fileName); 
         break;
      case BMP:  
         fileName += ".bmp";
         capture(fileName); 
         break;
      case EPS:
         captureVector(fileName, GL2PS_EPS);
         break;
      case PDF:
         captureVector(fileName, GL2PS_PDF);
         break;
      case SVG:
         captureVector(fileName, GL2PS_SVG);
         break;
   }
 
}


void Snapshot::capture(QString const& fileName)
{
   m_viewer->makeCurrent();
   //QImage image(m_viewer->renderPixmap().toImage());
   QImage image(m_viewer->grabFramebuffer());
   image.save(fileName);
   m_fileNames << fileName;
}


void Snapshot::makeMovie()
{
#ifdef Q_OS_MAC
   if (m_movieProcess) {
      QMsgBox::warning(0, "IQmol", "Movie making already in progress, please wait");
      return;
   }

   QDir dir(QApplication::applicationDirPath());
   dir.cdUp();
   dir.cd("Resources");
   QFileInfo script(dir,"crtimgseq.py");
   if (!script.exists()) {
      QMsgBox::warning(0, "IQmol", "Movie script not found");
      return;
   }

   QFile movie(m_fileBaseName + ".mov");
   if (movie.exists()) {
      if (!movie.remove()) {
         QMsgBox::warning(0, "IQmol", "Could not remove existing file " + movie.fileName());
         return;
      }
   }

   QStringList args;
   args << movie.fileName();

   for (int i = 0; i < m_fileNames.size(); ++i) {
       QFileInfo info(m_fileNames[i]);
       if (info.exists()) args << m_fileNames[i];
   }

   m_movieProcess = new QProcess;

   connect(m_movieProcess, SIGNAL(error(QProcess::ProcessError)), 
      this, SLOT(movieError(QProcess::ProcessError)));

   connect(m_movieProcess, SIGNAL(finished(int, QProcess::ExitStatus)), 
      this, SLOT(movieFinished(int, QProcess::ExitStatus)));

   qDebug() << "Start movie making";
   m_movieProcess->start(script.filePath(), args);

#endif
   return;
}


void Snapshot::makeFfmpegMovie()
{
   if (m_movieProcess) {
      QMsgBox::warning(0, "IQmol", "Movie making already in progress, please wait");
      return;
   }

   QDir dir(QApplication::applicationDirPath());
   QFileInfo ffmpeg(dir,"ffmpeg");
   if (!ffmpeg.exists()) {
      QMsgBox::warning(0, "IQmol", "ffmpeg executable not found");
      return;
   }

   QFile movie(m_fileBaseName + ".mp4");
   if (movie.exists()) {
      if (!movie.remove()) {
         QMsgBox::warning(0, "IQmol", "Could not remove existing file " + movie.fileName());
         return;
      }
   }
/*

The following ffmpeg command works, but does not work when the files are jpg 

ffmpeg -r 24 -i movie%04d.png -vcodec libx264 -y -an video.mp4 \
-vf "scale=trunc(iw/2)*2:trunc(ih/2)*2"

*/



   QStringList args;
   args << "-r" << "24" << "-i" << "movie%04d.png" 
        << "-vcodec" << "libx264" << "-y" << "-an" 
        << movie.fileName()
        << "-vf" << "\"scale=trunc(iw/2)*2:trunc(ih/2)*2\"";


   m_movieProcess = new QProcess;

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
   qDebug() << "movieFinished(int exitStatus, QProcess::ExitStatus) called";
   
   QString msg;
   switch (exitStatus) {
      case QProcess::NormalExit:
         msg = "Movie making finshed.  Remove image files?";
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




/*
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
*/


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



} // end namespace IQmol
