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

#include <QFileDialog>

///  Wrappers around QFileDialog functions that avoid the use of the native
///  file browser on MacOS, which causes problems in Sequoia

namespace IQmol {
namespace FileDialog {

QString getSaveFileName(
   QWidget *parent = nullptr, 
   QString const& caption = "Save File", 
   QString const& dir = QString(), 
   QString const& filter = QString(), 
   QString *selectedFilter = nullptr, 
   QFileDialog::Options options = QFileDialog::Options());


QString getExistingDirectory(
   QWidget *parent = nullptr,
   const QString &caption = QString(),
   const QString &dir = QString(),
   QFileDialog::Options options = QFileDialog::ShowDirsOnly);


QString getOpenFileName(
   QWidget *parent = nullptr,
   const QString &caption = QString(),
   const QString &dir = QString(),
   const QString &filter = QString(),
   QString *selectedFilter = nullptr,
   QFileDialog::Options options = QFileDialog::Options());

} } // end namespace IQmol::FileDialog
