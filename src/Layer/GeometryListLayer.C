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

#include "GeometryListLayer.h"
#include "GeometryLayer.h"
#include "InfoLayer.h"
#include "AtomLayer.h"

#include "Configurator/GeometryListConfigurator.h"
#include "Data/Geometry.h"
#include "Data/Energy.h"
#include "Data/GeometryList.h"
#include "QsLog.h"
#include "Viewer/Animator.h"
#include <QDebug>


using namespace qglviewer;

namespace IQmol {
namespace Layer {


GeometryList::GeometryList(Data::GeometryList& geometryList)
 : Base(geometryList.label()), m_molecule(0), m_configurator(0), m_geometryList(geometryList),
   m_speed(0.125), m_reperceiveBonds(false), m_bounce(false), m_loop(false), 
   m_allowModifications(false)
{
   setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
   setProperty(RemoveWhenChildless);
   m_defaultIndex = m_geometryList.defaultIndex();

   Data::GeometryList::const_iterator iter;
   for (iter = m_geometryList.begin(); iter != m_geometryList.end(); ++iter) {
       Data::Geometry* geometry(const_cast<Data::Geometry*>(*iter));
       if (geometry) {
          Layer::Geometry* layer(new Layer::Geometry(*geometry));
          appendRow(layer);
          QAction* remove(layer->newAction("Remove"));
          connect(remove, SIGNAL(triggered()), this, SLOT(removeGeometry()));
       }
   }

   // This logic may not be correct.  We assume we only want a configurator if
   // we have more than one geometry, and only allow adding additional
   // geometries if we start with at most one.
   if (geometryList.size() < 2) {
      connect(newAction("Copy Geometry"), SIGNAL(triggered()), 
         this, SLOT(cloneLastGeometry()));
      m_allowModifications = true;
   }else {
      m_configurator = new Configurator::GeometryList(*this);
      setConfigurator(m_configurator);
   }
}


GeometryList::~GeometryList()
{
   deleteAnimators();
   if (m_configurator) delete m_configurator;
}


void GeometryList::setMolecule(Molecule* molecule)
{
   if (!molecule) return;
   m_molecule = molecule;

   connect(this, SIGNAL(pushAnimators(AnimatorList const&)),
      m_molecule, SIGNAL(pushAnimators(AnimatorList const&)));

   connect(this, SIGNAL(popAnimators(AnimatorList const&)),
      m_molecule, SIGNAL(popAnimators(AnimatorList const&)));

   connect(this, SIGNAL(update()), m_molecule, SIGNAL(softUpdate()));
}


// Copies the last geometry and appends it to the list
void GeometryList::cloneLastGeometry()
{
   if (m_geometryList.isEmpty()) return; 

   // First copy the Data object
   Data::Geometry* last(m_geometryList.last());
   Data::Geometry* geom(new Data::Geometry(*last));
   m_geometryList.append(geom);
   m_geometryList.setDefaultIndex(-1);

   
   // Now create the new layer
   Geometry* geometry(new Geometry(*geom));

   QString label("Geom ");
   label += QString::number(m_geometryList.size());
   geometry->setText(label);

   QAction* remove(geometry->newAction("Remove"));
   connect(remove, SIGNAL(triggered()), this, SLOT(removeGeometry()));
   appendRow(geometry);

   setCurrentGeometry(m_geometryList.defaultIndex());
}



void GeometryList::removeGeometry()
{
    if (m_molecule == 0) return;
    Data::Geometry* target(0);

    Data::GeometryList::iterator iter;
    for (iter = m_geometryList.begin(); iter != m_geometryList.end(); ++iter) {
        if (m_molecule->isCurrentGeometry(*iter)) {
           target = *iter;
           // qDebug() << "Data::Geometry Match found"<< target;
           QList<Geometry*> geometryLayers(findLayers<Geometry>(Children));
           QList<Geometry*>::iterator geom;
           for (geom = geometryLayers.begin(); geom != geometryLayers.end(); ++geom) {
               if (&((*geom)->geomData()) == target) {
                  //qDebug() << "Layer::Geometry Match found"<< target;
                  removeLayer(*geom);
                  m_geometryList.removeAll(target);
                  if (m_configurator) m_configurator->load();
                  return;
                  delete target;
                  delete (*geom);
               }
           }
        }
    }
}



void GeometryList::setCurrentGeometry(unsigned const index)
{
   //qDebug() << "Layer::GeometryList::setCurrentGeometry with index" << index;
   if (!m_molecule || index >= (unsigned)m_geometryList.size()) return;

   Base* ptr(QVariantPtr<Base>::toPointer(child(index)->data()));
   Layer::Geometry* geometry(dynamic_cast<Layer::Geometry*>(ptr));
   if (!geometry) return;

   if (m_allowModifications) m_molecule->saveToCurrentGeometry();
   m_molecule->setGeometry(geometry->geomData());

   if (m_reperceiveBonds) {
      m_molecule->reperceiveBonds(true);
   }else {
      update(); 
   }

 //     m_molecule->reperceiveBondsForAnimation();
}


void GeometryList::deleteAnimators()
{
   AnimatorList::iterator iter;
   for (iter = m_animatorList.begin(); iter != m_animatorList.end(); ++iter) {
       delete (*iter);
   }
   m_animatorList.clear();
}


void GeometryList::makeAnimators()
{
   //qDebug() << "GeometryList::makeAnimators()";
   if (!m_animatorList.isEmpty()) {
      popAnimators(m_animatorList);
      deleteAnimators();
   }

   AtomList atomList(m_molecule->findLayers<Atom>(Children));
   QList<Geometry*> geometries(findLayers<Geometry>(Children));

   QLOG_DEBUG() << "Number of atoms and geometries" << atomList.size() << geometries.size();

   for (int i = 0; i < atomList.size(); ++i) {
       QList<Vec> waypoints;
       for (int j = 0; j < geometries.size(); ++j) {
           waypoints.append(geometries[j]->atomicPosition(i));
       }
       m_animatorList.append(new Animator::Path(atomList[i], waypoints, m_speed, m_bounce)); 
   }
   setLoop(m_loop);

   if (!m_animatorList.isEmpty() && m_configurator) {
      connect(m_animatorList.last(), SIGNAL(finished()), m_configurator, SLOT(reset()));
   }
}


void GeometryList::setPlay(bool const play)
{
   if (play) {
      AnimatorList::iterator iter;
      for (iter = m_animatorList.begin(); iter != m_animatorList.end(); ++iter) {
          (*iter)->reset();
      }
      pushAnimators(m_animatorList);
   }else {
      popAnimators(m_animatorList);
      m_molecule->reperceiveBondsForAnimation();
   }
}


void GeometryList::setSpeed(double const speed)
{
   m_speed = speed;
   AnimatorList::iterator iter;
   for (iter = m_animatorList.begin(); iter != m_animatorList.end(); ++iter) {
       (*iter)->setSpeed(m_speed);
   }
}


void GeometryList::setBounce(bool const bounce)
{
   m_bounce = bounce;
   AnimatorList::iterator iter;
   Animator::Path* pathAnimator;

   unsigned nGeometries(m_geometryList.size());
   int cycles(m_loop ? -1.0 : nGeometries-1);
   if (m_bounce) cycles *= 2;

   for (iter = m_animatorList.begin(); iter != m_animatorList.end(); ++iter) {
       pathAnimator = qobject_cast<Animator::Path*>(*iter); 
       if (pathAnimator) {
          pathAnimator->setBounceMode(bounce);
          pathAnimator->setCycles(cycles);
       }
   }
}


void GeometryList::setLoop(bool const loop)
{
   m_loop = loop;
   AnimatorList::iterator iter;
   Animator::Path* pathAnimator;

   unsigned nGeometries(m_geometryList.size());
   int cycles(m_loop ? -1.0 : nGeometries-1);
   if (m_bounce) cycles *= 2;

   for (iter = m_animatorList.begin(); iter != m_animatorList.end(); ++iter) {
       pathAnimator = qobject_cast<Animator::Path*>(*iter); 
       if (pathAnimator) pathAnimator->setCycles(cycles);
   }
}


void GeometryList::setReperceiveBonds(bool const tf) 
{ 
   m_reperceiveBonds = tf; 
   m_molecule->setReperceiveBondsForAnimation(m_reperceiveBonds);
}


void GeometryList::configure()
{
   if (m_geometryList.size() > 1) {
      resetGeometry();
      if (m_configurator) {
         m_configurator->load();
         m_configurator->reset();
         m_configurator->display();
      }
   }
}


void GeometryList::resetGeometry() 
{ 
   setCurrentGeometry(m_defaultIndex);
}

} } // end namespace IQmol::Layer
