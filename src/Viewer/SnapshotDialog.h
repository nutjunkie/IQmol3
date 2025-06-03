#pragma once
/*******************************************************************************

  Copyright (C) 2025 Andrew Gilbert

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

#include "ui_SnapshotImageDialog.h"
#include "ui_SnapshotVideoDialog.h"


namespace IQmol {

class SnapshotImageDialog : public QDialog {

   Q_OBJECT

   public:
      SnapshotImageDialog(QWidget* parent = 0);

      QSize size() const { return m_size; }
      bool antialias() const { return m_antialias; }
      int  dpi() const  { return m_dpi; }

   private Q_SLOTS:
      void finished();
      void updateSizeLabel(double);

   private:
      Ui::SnapshotImageDialog m_dialog;

      QSize m_size;
      bool  m_antialias;
      int   m_dpi;
};


class SnapshotVideoDialog : public QDialog {

   Q_OBJECT

   public:
      SnapshotVideoDialog(QWidget* parent = 0);

      QSize size() const { return m_size; }
      int framerate() const { return m_framerate; }
      bool usePNG() const { return m_usePng; }
      bool continuousRecording() const { return m_continuousRecording; }

   private Q_SLOTS:
      void finished();
      void updateSizeLabel(int);
      QSize getSize(int format);

   private:
      Ui::SnapshotVideoDialog m_dialog;

      QSize m_size;
      int   m_framerate;
      bool  m_usePng;
      bool  m_continuousRecording;
};


} // end namespace IQmol
