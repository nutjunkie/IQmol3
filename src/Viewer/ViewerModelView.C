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

#include "ViewerModelView.h"
#include "QVariantPtr.h"
#include "Layer/MoleculeLayer.h"

#include <QContextMenuEvent>
#include <QModelIndex>
#include <QMenu>

#include <QDebug>


namespace IQmol {

ViewerModelView::ViewerModelView(QWidget* parent) : QTreeView(parent)
{ 
   setDragDropMode(QAbstractItemView::DropOnly); 
   setDropIndicatorShown(true);
   setIndentation(15);
   setSelectionMode(QAbstractItemView::ExtendedSelection);
   setEditTriggers( QAbstractItemView::EditKeyPressed);
   setExpandsOnDoubleClick(false);
}


void ViewerModelView::contextMenuEvent(QContextMenuEvent*)
{
   QItemSelectionModel* selectModel(selectionModel());
   QModelIndexList selection(selectModel->selectedRows());

   Layer::Base* base;
   QMenu menu(this);

   if (selection.size() == 1) {
      QModelIndex target(selection.first());
      base = QVariantPtr<Layer::Base>::toPointer(target.data(Qt::UserRole+1));
	  // QVariantPtr is not guaranteed to return a valid Layer::Base pointer.
      if ( (base = qobject_cast<Layer::Base*>(base)) ) {
         QList<QAction*> actions(base->getActions());
         for (auto action : actions) menu.addAction(action);
      }
   }else {
      // Custom actions when more than one Layer is selected
      bool allMolecules(true);
      for (auto item : selection) {
          base = QVariantPtr<Layer::Base>::toPointer(item.data(Qt::UserRole+1));
          if (//!qobject_cast<Layer::Group*>(base) && 
              !qobject_cast<Layer::Molecule*>(base) ) {
              allMolecules = false;
              break;
          }
      }
      if (allMolecules) {
         QAction* action(new QAction("Merge Molecules")); 
         connect(action, SIGNAL(triggered()), this, SLOT(mergeSelection()));
         menu.addAction(action);

         action = new QAction("Show Molecules"); 
         connect(action, SIGNAL(triggered()), this, SLOT(showMolecules()));
         menu.addAction(action);

         action = new QAction("Hide Molecules"); 
         connect(action, SIGNAL(triggered()), this, SLOT(hideMolecules()));
         menu.addAction(action);
      }
   }

   bool canPromote(true);
   for (auto item : selection) {
       base = QVariantPtr<Layer::Base>::toPointer(item.data(Qt::UserRole+1));
       if (!qobject_cast<Layer::Group*>(base) && 
           !qobject_cast<Layer::Atom*>(base)  && 
           !qobject_cast<Layer::Bond*>(base)  &&
           !qobject_cast<Layer::Molecule*>(base) ) {
           canPromote = false;
           break;
       }
   }

   if (canPromote) {
      QAction* action(new QAction("New Molecule From Selection")); 
      connect(action, SIGNAL(triggered()), this, SLOT(newMoleculeFromSelection()));
      menu.addAction(action);
   }

   if (!menu.isEmpty()) menu.exec(QCursor::pos());
}


void ViewerModelView::newMoleculeFromSelection()
{
   QItemSelectionModel* selectModel(selectionModel());
   QModelIndexList selection(selectModel->selectedRows());

   newMoleculeFromSelection(selection);
}


void ViewerModelView::mergeSelection()
{
   QItemSelectionModel* selectModel(selectionModel());
   QModelIndexList selection(selectModel->selectedRows());

   mergeSelection(selection);
}


void ViewerModelView::deleteSelection()
{
   QItemSelectionModel* selectModel(selectionModel());
   QModelIndexList selection(selectModel->selectedRows());

   deleteSelection(selection);
}


void ViewerModelView::showMolecules()
{
   QItemSelectionModel* selectModel(selectionModel());
   QModelIndexList selection(selectModel->selectedRows());

   showMolecules(selection);
}


void ViewerModelView::hideMolecules()
{
   QItemSelectionModel* selectModel(selectionModel());
   QModelIndexList selection(selectModel->selectedRows());

   hideMolecules(selection);
}

} // end namespace IQmol
