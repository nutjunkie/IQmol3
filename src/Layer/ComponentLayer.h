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

#include "GLObjectLayer.h"
#include "Viewer/Animator.h"

class QUndoCommand;

namespace IQmol {

   namespace Layer {

      // Base class for components of a System, which may include QM Molecules,
      // Solvent, Protein, etc.
      // Shader context switching is implemented at this level.

      class Component : public Base 
      {
         enum class DrawStyle 
         { 
            BallsAndSticks, 
            Tubes, 
            SpaceFilling, 
            WireFrame, 
            Plastic, 
            Ribbon 
         };

         Q_OBJECT

         public:
            explicit Component(QString const& label = QString(), QObject* parent = 0) :
               Base(label, parent)
            {
               setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable );
               setCheckState(Qt::Checked);
            }

            ~Component() { }

            //virtual void loadFromFile(QString const& filePath) = 0
            //void align(QList<qglviewer::Vec> const& current);

            virtual double radius() = 0;

            void draw() const;

            void select() { for (auto iter : m_visibleObjects) iter->select(); }
   
            void deselect() { for (auto iter : m_visibleObjects) iter->deselect(); }

            void setDrawStyle(DrawStyle const drawStyle, unsigned resolution = 0);

            void setShaderKey(QString const& shaderKey) { m_shaderKey = shaderKey; }

            void updateVisibleObjectList()
            {
               m_visibleObjects = findLayers<Layer::GLObject>(Layer::Children | 
                  Layer::Visible | Layer::Nested);
            }

            // This can be used to keep a running tally for computing the
            // center of nuclear 
            void centerOfNuclearCharge(qglviewer::Vec&, int& totalZ);

            void translateToCenter(GLObjectList const& selection);

            void translate(qglviewer::Vec const& displacement);

            void rotate(qglviewer::Quaternion const& rotation);

            void alignToAxis(qglviewer::Vec const& point, 
               qglviewer::Vec const& axis = qglviewer::Vec(0.0,0.0,1.0));

            void rotateIntoPlane(qglviewer::Vec const& point, 
               qglviewer::Vec const& axis = qglviewer::Vec(0.0,0.0,1.0),
               qglviewer::Vec const& normal = qglviewer::Vec(0.0,1.0,0.0));

            qglviewer::Frame const& getReferenceFrame() const { return m_frame; }
            void setReferenceFrame(qglviewer::Frame const& frame) { m_frame = frame; }

         public Q_SLOTS:
            virtual void invalidateSymmetry() { }
            virtual void autoDetectSymmetry() { }

         Q_SIGNALS:
            void useShader(QString const&) const;
            void useDrawStyle(DrawStyle const) const;
            void useResolution(unsigned const) const;
            void softUpdate(); // Issue if the number of primitives does not change
            void pushAnimators(AnimatorList const&);
            void popAnimators(AnimatorList const&);

            void postCommand(QUndoCommand*);
            void postMessage(QString const&);

         private:
            QString   m_shaderKey;
            unsigned  m_resolution;
            DrawStyle m_drawStyle;
            qglviewer::Frame m_frame;
            GLObjectList m_visibleObjects;
      };

   }  // end namespace Layer

   typedef QList<Layer::Component*> ComponentList;

} // end namespace IQmol::Layer 
