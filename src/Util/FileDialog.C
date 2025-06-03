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

#include "FileDialog.h"

namespace IQmol {
namespace FileDialog {

QString getSaveFileName(
   QWidget *parent, 
   QString const& caption, 
   QString const& dir, 
   QString const& filter, 
   QString *selectedFilter, 
   QFileDialog::Options options)
{
   QString fileName;
#ifdef Q_OS_MAC
    QFileDialog dialog(parent, caption, dir, filter);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setOptions(options | QFileDialog::DontUseNativeDialog);

    if (selectedFilter) dialog.selectNameFilter(*selectedFilter);
        
    if (dialog.exec() == QDialog::Accepted) {
        if (selectedFilter) *selectedFilter = dialog.selectedNameFilter();
            
        fileName = dialog.selectedFiles().first();
    }
#else
   fileName = QFileDialog::getSaveFileName(parent, caption, dir, filter, 
      selectedfilter, options);
#endif
   return fileName;
}


QString getExistingDirectory(
   QWidget *parent,
   const QString &caption,
   const QString &dir,
   QFileDialog::Options options)
{
   QString dirName;
#ifdef Q_OS_MAC
    QFileDialog dialog(parent, caption, dir);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOptions(options | QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog);

    if (dialog.exec() == QDialog::Accepted) {
        QStringList selected = dialog.selectedFiles();
        if (!selected.isEmpty()) dirName = selected.first();
    }

#else
   dirName = QFileDialog::getExistingDirectory( parent, caption, dir, options);
#endif
    return dirName;
}


QString getOpenFileName(
   QWidget *parent,
   const QString &caption,
   const QString &dir,
   const QString &filter,
   QString *selectedFilter,
   QFileDialog::Options options)
{
   QString fileName;
#ifdef Q_OS_MAC
    QFileDialog dialog(parent, caption, dir, filter);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setOptions(options | QFileDialog::DontUseNativeDialog);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);

    if (selectedFilter)
        dialog.selectNameFilter(*selectedFilter);

    if (dialog.exec() == QDialog::Accepted) {
        if (selectedFilter)
            *selectedFilter = dialog.selectedNameFilter();
        fileName = dialog.selectedFiles().first();
    }
#else
   fileName = QFileDialog::getOpenFileName( parent, caption, dir, filter,
      selectedFilter, options)
#endif

    return fileName;
}



} } // end namespace IQmol::FileDialog
