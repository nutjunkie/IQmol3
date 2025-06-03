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

#include "SnapshotDialog.h"


namespace IQmol {

// ------------ Image ------------ 

SnapshotImageDialog::SnapshotImageDialog(QWidget* parent) 
 : QDialog(parent),
   m_size(parent->size()),
   m_antialias(false)
{
   m_dialog.setupUi(this);
   connect(m_dialog.buttonBox, SIGNAL(accepted()), this, SLOT(finished()));
   connect(m_dialog.buttonBox, SIGNAL(rejected()), this, SLOT(close()));

   connect(m_dialog.sizeSpin, SIGNAL(valueChanged(double)), 
      this, SLOT(updateSizeLabel(double)));

   updateSizeLabel(1.0);
}


void SnapshotImageDialog::finished()
{
   m_size     *= m_dialog.sizeSpin->value();
   m_antialias = m_dialog.antialias->isChecked();
   m_dpi = m_dialog.dpi->value();
   accept();
}


void SnapshotImageDialog::updateSizeLabel(double scale)
{
   QSize size = m_size * scale;
   QString str = QString("%1px x %2px").arg(size.width()).arg(size.height());
   m_dialog.sizeLabel->setText(str);
}


// ------------ Video ------------ 

SnapshotVideoDialog::SnapshotVideoDialog(QWidget* parent) 
 : QDialog(parent),
   m_size(parent->size()),
   m_framerate(15),
   m_usePng(false),
   m_continuousRecording(false)
{

   m_dialog.setupUi(this);
   connect(m_dialog.buttonBox, SIGNAL(accepted()), this, SLOT(finished()));
   connect(m_dialog.buttonBox, SIGNAL(rejected()), this, SLOT(close()));

   connect(m_dialog.sizeCombo, SIGNAL(currentIndexChanged(int)), 
      this, SLOT(updateSizeLabel(int)));

   updateSizeLabel(0);
}


void SnapshotVideoDialog::finished()
{
   m_framerate = m_dialog.framerate->value();
   m_usePng    = m_dialog.png->isChecked();
   m_continuousRecording = m_dialog.continuousRecording->isChecked();
   accept();
}


void SnapshotVideoDialog::updateSizeLabel(int format)
{
   QSize size(getSize(format));
   QString str = QString("%1px x %2px").arg(size.width()).arg(size.height());
   m_dialog.sizeLabel->setText(str);
}


QSize SnapshotVideoDialog::getSize(int format)
{
   QSize size(m_size);

   switch (format) {
      case 0: 
         // Viewer, default
         break;
      case 1: 
         size = QSize(640,480);    // 480p
         break;
      case 2: 
         size = QSize(1280,720);   // 720p
         break;
      case 3:
         size = QSize(1920,1080);  // 1080p
         break;
      case 4: 
         size = QSize(2560,1440);  // 2K
         break;
      case 5: // 4K
         size = QSize(3840,2160);  // 4K
         break;
   }

   return size;
}


} // end namespace IQmol
