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
#include "AtomLayer.h"
#include "GroupLayer.h"
#include "Util/QsLog.h"
#include "Viewer/UndoCommands.h"


namespace IQmol {
namespace Layer {


Component::Component(QString const& label, QObject* parent) : Base(label, parent),
   m_surfaceList(this, "Surfaces")
{
   setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable);
   setCheckState(Qt::Checked);

   connect(&m_surfaceList, SIGNAL(updated()), this, SIGNAL(softUpdate()));

   initProperties();
}


void Component::draw() const
{
   useShader(m_shaderKey);
   useResolution(m_resolution);
   useDrawStyle(m_drawStyle);
   for (auto iter : m_visibleObjects) iter->draw();
}


void Component::deleteProperties()
{
   for (auto& property : m_properties) delete property;
   m_properties.clear();
}


void Component::initProperties()
{
   deleteProperties();
   m_properties << new Property::RadialDistance();
}


QStringList Component::getAvailableProperties2()
{
   QStringList properties;
   for (auto const& property : m_properties) {
       properties << property->text();
   }
   return properties;
}


Property::Base* Component::getProperty(QString const& name)
{
    for (auto& property : m_properties) {
       if (property->text() == name) {
          return property;
       }
   }
   return 0;
}


Data::Mesh::VertexFunction Component::getPropertyEvaluator2(QString const& name)
{
   for (auto& property : m_properties) {
       if (property->text() == name) {
          return property->evaluator();
       }
   }
  
   QLOG_WARN() << "Evaluator for property" << name << "not found, returning null function";
   return Data::NullFunction;
}

 
void Component::translateToCenter(GLObjectList const& selection)
{
   bool animate(true);
   Command::MoveObjects* cmd =
       new Command::MoveObjects(this, "Translate to center", animate);
   
   // The ordering here is important!!
   Atom* atom;
   AtomList atomList;
   GLObjectList::const_iterator iter;
   for (iter = selection.begin(); iter != selection.end(); ++iter) {
       if ( (atom = qobject_cast<Atom*>(*iter)) ) atomList.append(atom);
   }   
   
   switch (atomList.size()) {
      case 0: {
         qglviewer::Vec conc;
         int totalZ(0);
         centerOfNuclearCharge(conc, totalZ);
         translate(-conc/double(totalZ));
      }  break;

      case 1:
         translate(-atomList[0]->getPosition());
         break;

      case 2:
         translate(-atomList[0]->getPosition());
         alignToAxis(atomList[1]->getPosition());
         break;

      case 3:
         translate(-atomList[0]->getPosition());
         alignToAxis(atomList[1]->getPosition());
         rotateIntoPlane(atomList[2]->getPosition());
         break;

      default: 
         {  
            // Determine the CONC for the selection only.
            int totalCharge(0);
            qglviewer::Vec center;
            for (auto iter = atomList.begin(); iter != atomList.end(); ++iter) {
                int Z = (*iter)->getAtomicNumber();
                totalCharge += Z;
                center += Z * (*iter)->getPosition();
            }    
            if (totalCharge > 0.0) center = center / totalCharge;
            translate(-center);
         }  
         break;
   }   
   
   postCommand(cmd);
   softUpdate();
}


void Component::centerOfNuclearCharge(qglviewer::Vec& center, int& totalZ)
{
   int Z;

   AtomList atoms(findLayers<Atom>());
   GroupList groups(findLayers<Group>());

   for (auto group : groups) {
       atoms += group->getAtoms();
   }

   for (auto iter = atoms.begin(); iter != atoms.end(); ++iter) {
       Z = (*iter)->getAtomicNumber();
       totalZ += Z;
       center += Z * (*iter)->getPosition();
   }     
}


void Component::translate(qglviewer::Vec const& displacement)
{
qDebug() << "Translating component " << text() << " by "
         << displacement.x << " " << displacement.y << " " << displacement.z;
   GLObjectList objects(findLayers<GLObject>(Children));
   GLObjectList::iterator iter;
   for (iter = objects.begin(); iter != objects.end(); ++iter) {
       (*iter)->setPosition((*iter)->getPosition()+displacement);
   }

   // m_frame gets adjusted by the UndoCommand
   // m_frame.setPosition(m_frame.position()+displacement);
}


void Component::rotate(qglviewer::Quaternion const& rotation)
{
   GLObjectList objects(findLayers<GLObject>(Children));
   GLObjectList::iterator iter;
   for (iter = objects.begin(); iter != objects.end(); ++iter) {
       (*iter)->setPosition(rotation.rotate((*iter)->getPosition()));
       (*iter)->setOrientation(rotation * (*iter)->getOrientation());
   }
   // m_frame gets adjusted by the UndoCommand
   // m_frame.setPosition(rotation.rotate(m_frame.position()));
   // m_frame.setOrientation(rotation * m_frame.orientation());
}


// Aligns point along axis (default z-axis)
void Component::alignToAxis(qglviewer::Vec const& point, qglviewer::Vec const& axis)
{
   rotate(qglviewer::Quaternion(point, axis));
}


// Rotates point into the plane defined by the normal vector
void Component::rotateIntoPlane(qglviewer::Vec const& pt, qglviewer::Vec const& axis, 
   qglviewer::Vec const& normal)
{
   qglviewer::Vec pp(pt);
   pp.projectOnPlane(axis);
   rotate(qglviewer::Quaternion(pp, cross(normal, axis)));
}


Layer::Surface* Component::createSurfaceLayer(Data::Surface* surfaceData)
{
   Layer::Surface* surfaceLayer(new Layer::Surface(*surfaceData));
   connect(surfaceLayer, SIGNAL(updated()), this, SIGNAL(softUpdate()));
   surfaceLayer->setCheckState(Qt::Checked);
   surfaceLayer->setCheckStatus(Qt::Checked);
   return surfaceLayer;
}


void Component::appendSurface(Data::Surface* surfaceData)
{
   Layer::Surface* surface = createSurfaceLayer(surfaceData);
   surface->setComponent(this);
   m_surfaceList.appendLayer(surface);
   updated();
}


void Component::prependSurface(Data::Surface* surfaceData)
{
   Layer::Surface* surface = createSurfaceLayer(surfaceData);
   m_surfaceList.prependLayer(surface);
   updated();
}

} } // end namespace IQmol::Layer 
