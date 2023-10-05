#pragma once
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

#include <QTreeView>
#include "ViewerModel.h"


class QContextMenuEvent;

namespace IQmol {

   /// QTreeView derived class that implements context menu handling.
   class ViewerModelView : public QTreeView {

      Q_OBJECT

      public:
         ViewerModelView(QWidget* parent = 0);

      Q_SIGNALS:
         void newMoleculeFromSelection(QModelIndexList const&);
         void mergeSelection(QModelIndexList const&);
         void deleteSelection(QModelIndexList const&);

      private Q_SLOTS:
         void newMoleculeFromSelection();
         void mergeSelection();
         void deleteSelection();

      private Q_SLOTS:
         void contextMenuEvent(QContextMenuEvent*);
   };

} // end namespace IQmol
