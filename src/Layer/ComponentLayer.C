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


namespace IQmol {
namespace Layer {


qglviewer::Vec Component::centerOfNuclearCharge()
{
   qglviewer::Vec center;
   int Z;
   double totalCharge(0.0);
   AtomList atoms(findLayers<Atom>());
   GroupList groups(findLayers<Group>());

   for (auto group : groups) {
       atoms += group->getAtoms();
   }

   qDebug() << "Component::centerOfNuclearCharge() returning number of atoms:" << atoms.size();
      
   for (auto iter = atoms.begin(); iter != atoms.end(); ++iter) {
       Z = (*iter)->getAtomicNumber();
       totalCharge += Z;
       center += Z * (*iter)->getPosition();
   }     
            
   if (totalCharge > 0.0) center = center / totalCharge;
qDebug() << "COC returning" << center.x << center.y << center.z;
   return center;
}


void Component::translate(qglviewer::Vec const& displacement)
{
   GLObjectList objects(findLayers<GLObject>(Children));
   GLObjectList::iterator iter;
   for (iter = objects.begin(); iter != objects.end(); ++iter) {
       (*iter)->setPosition((*iter)->getPosition()+displacement);
   }
}


void Component::rotate(qglviewer::Quaternion const& rotation)
{
   GLObjectList objects(findLayers<GLObject>(Children));
   GLObjectList::iterator iter;
   for (iter = objects.begin(); iter != objects.end(); ++iter) {
       (*iter)->setPosition(rotation.rotate((*iter)->getPosition()));
       (*iter)->setOrientation(rotation * (*iter)->getOrientation());
   }
   //m_frame.setPosition(rotation.rotate(m_frame.position()));
   //m_frame.setOrientation(rotation * m_frame.orientation());
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

} } // end namespace IQmol::Layer 
