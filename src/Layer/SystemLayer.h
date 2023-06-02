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
#include "Data/Bank.h"
#include <QFileInfo>


using qglviewer::Vec;

class QUndoCommand;

namespace IQmol {

   namespace Layer {

      // This is the top-level Layer to handle a collection of Components. 
      // It will eventually supercede the Molecule class
      class System : public Base {

         Q_OBJECT

         public:
            explicit System(QString const& label = QString(), QObject* parent = 0);

            QString fileName() const { return m_inputFile.fileName(); }

            void setFile(QString const& fileName) 
            {
               m_inputFile.setFile(fileName);
               setText(m_inputFile.completeBaseName());
            }

            double radius();

            qglviewer::Vec center();

            void appendData(Data::Bank&);

            void translateToCenter(GLObjectList const& selection = GLObjectList());

            //qglveiwer::Frame const& getReferenceFrame() const { return m_frame; }

            //void setReferenceFrame(qglviewer::Frame const& frame) { m_frame = frame; }
    

         Q_SIGNALS:
            void softUpdate(); // Issue when number of primitives does not change
            void postCommand(QUndoCommand*);

         private:
            void translate(Vec const&);

            void rotate(qglviewer::Quaternion const&);

            void alignToAxis(qglviewer::Vec const& point, 
               qglviewer::Vec const& axis = qglviewer::Vec(0.0, 0.0, 1.0));

            void rotateIntoPlane(qglviewer::Vec const& point, 
               qglviewer::Vec const& axis = qglviewer::Vec(0.0, 0.0, 1.0),
               qglviewer::Vec const& normal = qglviewer::Vec(0.0, 1.0, 0.0));

            void appendData(Layer::List& list);
            QFileInfo m_inputFile;

            //qglveiwer::Frame m_frame;
            
      };

   } // end namespace Layer

   typedef QList<Layer::System*> SystemList;

} // end namespace IQmol
