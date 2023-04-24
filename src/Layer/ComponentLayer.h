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


namespace IQmol {
namespace Layer {

   // Base class for components of a System, which may include QM Molecules,
   // Solvent, Protein, etc.
   // Shader context is implemented at this level.

   class Component : public Base {

      enum DrawStyle { BallsAndSticks, Tubes, SpaceFilling, WireFrame, Plastic, Ribbon };

      Q_OBJECT

      public:
         explicit Component(QString const& label = QString(), QObject* parent = 0) :
            Base(label, parent)
         {
            setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable );
         }

         ~Component() { }

         //virtual void loadFromFile(QString const& filePath) = 0
         //void align(QList<qglviewer::Vec> const& current);

         void draw() const
         {
            useShader(m_shaderKey);
            useResolution(m_resolution);
            useDrawStyle(m_drawStyle);
            for (auto iter : m_visibleObjects) iter->draw();
         }

         void select() 
         {
            for (auto iter : m_visibleObjects) iter->select();
         }

         void deselect()
         {
            for (auto iter : m_visibleObjects) iter->deselect();
         }

         void setDrawStyle(DrawStyle const drawStyle, unsigned resolution = 0);

         void setShaderKey(QString const& shaderKey) { m_shaderKey = shaderKey; }

         void updateVisibleObjectList()
         {
            m_visibleObjects = findLayers<Layer::GLObject>(Layer::Children | 
               Layer::Visible | Layer::Nested);
         }

      Q_SIGNALS:
         void useShader(QString const&) const;
         void useDrawStyle(DrawStyle const) const;
         void useResolution(unsigned const) const;
    
      private:
         QString  m_shaderKey;
         unsigned m_resolution;
         DrawStyle m_drawStyle;
         GLObjectList m_visibleObjects;
   };

   typedef QList<Component*> ComponentList;

} } // end namespace IQmol::Layer 
