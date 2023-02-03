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

#include "OSGViewer.h"
#include <osg/Camera>
#include <osg/Material>
#include <osg/StateSet>
#include <osg/ShapeDrawable>
#include <osgGA/TrackballManipulator>

#include <QMouseEvent>
#include <QWheelEvent>


namespace IQmol {

OSGViewer::OSGViewer(qreal scaleX, qreal scaleY, QWidget* parent) 
   : QOpenGLWidget(parent), m_graphicsWindow(new osgViewer::GraphicsWindowEmbedded( 
     x(), y(), width(), height() ) ) , m_viewer(new osgViewer::Viewer),
     m_scaleX(scaleX), m_scaleY(scaleY)
{
   osg::Cylinder* cylinder    = new osg::Cylinder( osg::Vec3( 0.f, 0.f, 0.f ), 0.25f, 0.5f );
   osg::ShapeDrawable* sd = new osg::ShapeDrawable( cylinder );
   sd->setColor( osg::Vec4( 0.8f, 0.5f, 0.2f, 1.f ) );
   osg::Geode* geode = new osg::Geode;
   geode->addDrawable(sd);

   osg::Camera* camera = new osg::Camera;
   camera->setViewport( 0, 0, width(), height() );
   camera->setClearColor( osg::Vec4( 0.9f, 0.9f, 1.f, 1.f ) );
   float aspectRatio = static_cast<float>( width()) / static_cast<float>( height() );
   camera->setProjectionMatrixAsPerspective( 30.f, aspectRatio, 1.f, 1000.f );
   camera->setGraphicsContext( m_graphicsWindow );

   m_viewer->setCamera(camera);
   m_viewer->setSceneData(geode);
   osgGA::TrackballManipulator* manipulator = new osgGA::TrackballManipulator;
   manipulator->setAllowThrow( false );
   setMouseTracking(true);
   m_viewer->setCameraManipulator(manipulator);
   m_viewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
   m_viewer->realize();
}


void OSGViewer::setScale(qreal X, qreal Y)
{
   m_scaleX = X;
   m_scaleY = Y;
   resizeGL(width(), height());
}


void OSGViewer::paintGL() 
{
   m_viewer->frame();
}


void OSGViewer::resizeGL( int width, int height ) 
{
   getEventQueue()->windowResize(x()*m_scaleX, y() * m_scaleY, width*m_scaleX, height*m_scaleY);
   m_graphicsWindow->resized(x()*m_scaleX, y()*m_scaleY, width*m_scaleX, height*m_scaleY);
   osg::Camera* camera = m_viewer->getCamera();
   camera->setViewport(0, 0, this->width()*m_scaleX, this->height()*m_scaleY);
}


void OSGViewer::initializeGL()
{
   osg::Geode* geode = dynamic_cast<osg::Geode*>(m_viewer->getSceneData());
   osg::StateSet* stateSet = geode->getOrCreateStateSet();
   osg::Material* material = new osg::Material;
   material->setColorMode( osg::Material::AMBIENT_AND_DIFFUSE );
   stateSet->setAttributeAndModes( material, osg::StateAttribute::ON );
   stateSet->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );
}


void OSGViewer::mouseMoveEvent(QMouseEvent* event)
{
   getEventQueue()->mouseMotion(event->x()*m_scaleX, event->y()*m_scaleY);
}


void OSGViewer::mousePressEvent(QMouseEvent* event)
{
   unsigned int button = 0;
   switch (event->button()){
   case Qt::LeftButton:
       button = 1;
       break;
   case Qt::MiddleButton:
       button = 2;
       break;
   case Qt::RightButton:
       button = 3;
       break;
   default:
       break;
   }
   getEventQueue()->mouseButtonPress(event->x()*m_scaleX, event->y()*m_scaleY, button);
}


void OSGViewer::mouseReleaseEvent(QMouseEvent* event)
{
   unsigned int button = 0;
   switch (event->button()){
   case Qt::LeftButton:
       button = 1;
       break;
   case Qt::MiddleButton:
       button = 2;
       break;
   case Qt::RightButton:
       button = 3;
       break;
   default:
       break;
   }
   getEventQueue()->mouseButtonRelease(event->x()*m_scaleX, event->y()*m_scaleY, button);
}


void OSGViewer::wheelEvent(QWheelEvent* event)
{
   int delta = event->delta();
   osgGA::GUIEventAdapter::ScrollingMotion motion = delta > 0 ?
      osgGA::GUIEventAdapter::SCROLL_UP : osgGA::GUIEventAdapter::SCROLL_DOWN;
   getEventQueue()->mouseScroll(motion);
}

bool OSGViewer::event(QEvent* event)
{
   bool handled = QOpenGLWidget::event(event);
   update();
   return handled;
}


osgGA::EventQueue* OSGViewer::getEventQueue() const 
{
   osgGA::EventQueue* eventQueue = m_graphicsWindow->getEventQueue();
   return eventQueue;
}

} // end namespace IQmol
