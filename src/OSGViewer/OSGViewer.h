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

#include <QOpenGLWidget>
#include <osg/ref_ptr>
#include <osgViewer/Viewer>


namespace osgGA {
   class EventQueue;
}

namespace IQmol {

class OSGViewer : public QOpenGLWidget
{
   public:
      OSGViewer(qreal scaleX, qreal scaleY, QWidget* parent = 0);
      virtual ~OSGViewer() { }

      void setScale(qreal X, qreal Y);

   protected:
      virtual void paintGL();
      virtual void resizeGL( int width, int height );
      virtual void initializeGL(); 
      virtual void mouseMoveEvent(QMouseEvent* event);
      virtual void mousePressEvent(QMouseEvent* event);
      virtual void mouseReleaseEvent(QMouseEvent* event);
      virtual void wheelEvent(QWheelEvent* event);
      virtual bool event(QEvent* event);

   private:
      osgGA::EventQueue* getEventQueue() const;
      osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> m_graphicsWindow;
      osg::ref_ptr<osgViewer::Viewer> m_viewer;
      qreal m_scaleX, m_scaleY;
};

} // end namespace IQmol
