#pragma once 
/*******************************************************************************

  Copyright (C) 2023 Andrew Gilbert

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

#include "ComponentLayer.h"
#include "OctreeLayer.h"
#include <QFileInfo>


namespace IQmol {

   namespace Data {
      class Bank;
   }   

   namespace Layer {

      // This is the top-level Layer to handle a collection of Components. 
      class System : public Component
      {
         Q_OBJECT

         public:
            explicit System(QString const& label = QString(), QObject* parent = 0);

            QString fileName() const { return m_inputFile.fileName(); }

            void setFile(QString const& fileName) 
            {
               m_inputFile.setFile(fileName);
               setText(m_inputFile.completeBaseName());
            }

            void appendData(Data::Bank&);

            double radius();

         Q_SIGNALS:
            void selectionChanged();
            void newMoleculeRequested(AtomList const&);
            void connectComponent(Layer::Component*);

         private Q_SLOTS:
            void boxSystem();

         private:
            Octree* m_octree;
            void appendData(Layer::List& list);

            QFileInfo m_inputFile;
      };

   } // end namespace Layer

   typedef QList<Layer::System*> SystemList;

} // end namespace IQmol
