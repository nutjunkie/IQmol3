/*******************************************************************************

  Copyright (C) 2022-2025 Andrew Gilbert

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

#include "CustomPlot.h"
#include "FileDialog.h"
#include "Preferences.h"

#include <QContextMenuEvent>
#include <QRegularExpression>


namespace IQmol {

void CustomPlot::contextMenuEvent(QContextMenuEvent* event)
{
   QMenu *menu(new QMenu(this));
   menu->setAttribute(Qt::WA_DeleteOnClose);
   menu->addAction("Save Image As", this, SLOT(saveAs()));
   menu->popup(mapToGlobal(event->pos()));
}


void CustomPlot::saveAs()
{
   QFileInfo info(Preferences::LastFileAccessed());

   QString selectedFilter(tr("PNG") + " (*.png)");
   QStringList filter;
   filter << selectedFilter
          << tr("JPG") + " (*.jpg)"
          << tr("PDF") + " (*.pdf)";

   while (1) {
	  QString fileName = FileDialog::getSaveFileName(
          this, 
          tr("Save File"),
          info.filePath(), 
          filter.join(";;"), 
          &selectedFilter);

      if (fileName.isEmpty()) return; // user cancels action

      QRegularExpression rx("\\*(\\..+)\\)");
      QRegularExpressionMatch match(rx.match(selectedFilter));
      if (match.hasMatch()) { 
         selectedFilter = match.captured(1);
         if (!fileName.endsWith(selectedFilter, Qt::CaseInsensitive)) {
            fileName += selectedFilter;
         }    
      }    

      QSize dim(size());
      int upscale(2);

      if (selectedFilter == ".pdf") {
         savePdf(fileName, dim.width(), dim.height());
      }else if (selectedFilter == ".png") {
         savePng(fileName, dim.width(), dim.height(), upscale);
      }else if (selectedFilter == ".jpg") {
         saveJpg(fileName, dim.width(), dim.height(), upscale);
      }

      Preferences::LastFileAccessed(fileName);
      break;
   }    
}

} // end namespace IQmol
