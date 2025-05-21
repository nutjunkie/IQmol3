#pragma once
/*******************************************************************************

  Copyright (C) 2022 Andrew Gilbert

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

#include "Layer.h"


class QAction;

namespace IQmol {

namespace Configurator 
{
   class Base;
}


namespace Layer 
{
   class Configurable : public Base {

      Q_OBJECT

      public:

         explicit Base(QString const& text = QString(), QObject* parent = 0);
         virtual ~Base();

		 /// Allows custom property flags to be set for the Layer.

		 /// Removes a child Layer from this, if it exists.  Note that deletion
		 /// of the Layer must be handled explicitly.
         virtual void removeLayer(Base* child);
         virtual void orphanLayer();

		 // This is used to pass the change in the checkbox status from the
		 // QStandardItem to the Layer.
         virtual void setCheckStatus(Qt::CheckState const) { }

         //virtual void setMolecule(Molecule* molecule) { }
         //Molecule* molecule() const { return m_molecule; }
        
		 /// Returns a list of actions that should appear in the context menu.
         QList<QAction*> const& getActions() 
         { 
            return m_actions; 
         }

         /// Creates a new action for the context menu, ensuring it gets added
         /// to the m_actions list.
         QAction* newAction(QString const& text);


      Q_SIGNALS:
         /// Signals sent when this Layer has changed.
         void updated(); 

         /// Signals sent just before this Layer is about to be deleted.
         void deleted();

         /// Signals sent when this Layer has been removed from its parent. 
         void orphaned();

         /// Signals sent when this Layer has been added to its parent. 
         void adopted();

      public Q_SLOTS:
         virtual void configure() { 
            if (m_configurator) m_configurator->display(); 
         }

         virtual void closeConfigurator() { 
            if (m_configurator) m_configurator->close(); 
         }

		 /// The persistent parent exists even if the Layer has been removed
		 /// from the model.  This is useful for undo actions that need to know
		 /// where the Layer was so that it can be put back if redo is called.
         /// In most cases this does not need to be explicity called as it is 
         /// called by appendLayer, but if the RemoveWhenChildless flag is set
         /// on a Layer that hasn't been appended yet, it will need to be called.
         void setPersistentParent(Base* parent);

      protected:
         void setConfigurator(Configurator::Base* configurator) 
         {
            m_configurator = configurator; 
         }

      private Q_SLOTS:
         void persistentParentDeleted() { m_persistentParent = 0; }

      private:
		 /// Removes this Layer from the Model but does not delete it.  The
		 /// Layer can linger (in a QUndoAction for example) and later be 
		 /// added back to the Model if required using adopt.
         void orphan();

		 /// Returns this Layer to the model after being orphaned.  This should 
		 /// only be called for Layers that have had a persistent parent set.
         void adopt();

         /// Returns a list of the child Layers of a given type, excluding itself.
         template <class T>
         void findChildren(QList<T*>& children, unsigned int flags)
         {
            if (flags & Visible && isCheckable() && checkState() == Qt::Unchecked) return;

            for (int i = 0; i < rowCount(); ++i) {
                Base* ptr(QVariantPtr<Base>::toPointer(child(i)->data()));

                bool append(true);
                if (flags & Visible && ptr->isCheckable()) {
                   append = append && (ptr->checkState() == Qt::Checked);
                }
 
                if (append) {
                   if (flags & SelectedOnly) append = append && ptr->hasProperty(Selected);
                   T* t;
                   if (append && (t = dynamic_cast<T*>(ptr)) ) {
                      children.append(t);
                      if (flags & Nested) ptr->findChildren<T>(children, flags);
                   }else {
                      ptr->findChildren<T>(children, flags);
                   }
                }
            }
         }

         // Data members
         Configurator::Base* m_configurator;
         QList<QAction*> m_actions;

         Base* m_persistentParent;
   };

} } // end namespace IQmol::Layer
