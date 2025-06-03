#pragma once
/*******************************************************************************

  Copyright (C) 2011-2025 Andrew Gilbert

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

#include <QStringList>
#include <QProcess>
#include <QTimer>
#include <QSize>


///  Manages the saving of snapshots, writing them to file and creating movies
///  if requested.

namespace IQmol {

   class Viewer;

   class Snapshot : public QObject {

      Q_OBJECT

      public:
         enum Format { JPG, PNG, TIFF, PPM, BMP, EPS, PDF, SVG };

         enum Flags { 
            None          = 0x00,  
            AutoIncrement = 0x01,
            Overwrite     = 0x02,
            Movie         = 0x04,
            Continuous    = 0x08,  // Time-based recoding
            Vector        = 0x10
         };

         Snapshot(Viewer* viewer, unsigned const flags = 0);

         bool requestFileName();
         void resetCounter() { m_counter = 0; } 
         void setFramerate(double fps) { m_framerate = fps; }
         void makeMovie();

      Q_SIGNALS:
         void movieFinished();

      public Q_SLOTS:
         void capture();
         void startRecord();
         void stopRecord();

      private Q_SLOTS:
         void movieError(QProcess::ProcessError);
         void movieFinished(int, QProcess::ExitStatus);

      private:
         void removeImageFiles(QString const& msg);

         // Deprecate
         //void writefile(int format, int sort, int options, int nbcol,
         //      const char *filename, const char *extension);
         //void captureVector(QString const& fileName, int const format);

         Viewer*  m_viewer;
         unsigned m_flags;
         unsigned m_counter;

         QSize  m_size;
         double m_framerate;
         int    m_antialias;
         int    m_dpi;

         QStringList m_fileNames;
         QProcess*   m_movieProcess;
         QTimer      m_recordTimer;

         QString m_fileBaseName; 
         QString m_fileExtension;
         QString m_videoExtension;
   };


} // end namespace IQmol
