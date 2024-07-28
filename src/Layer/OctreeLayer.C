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

#include "OctreeLayer.h"

#include <cmath>
#include <limits>
#include <iterator>
#include <algorithm>

#include <QMap>

#include <QtDebug>


using namespace qglviewer;


namespace IQmol {
namespace Layer {



double Octree::s_lineWidth  = 2.5;
GLfloat Octree::s_selectionColor[] = {0.0, 0.8, 1.0, 0.2};


Octree::Octree(AtomList const& atoms) : GLObject("Octree Box"), m_atomList(atoms),
   m_configurator(*this), m_selectionRadius(0.0)
{
   setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
   setCheckState(Qt::Checked);
   setConfigurator(&m_configurator);
   computeBoundingBox();

   qDebug() << "Bounding box min:" 
            << m_boundingBoxMin.x << m_boundingBoxMin.y << m_boundingBoxMin.z;
   qDebug() << "Bounding box max:" 
            << m_boundingBoxMax.x << m_boundingBoxMax.y << m_boundingBoxMax.z;

   boxAtoms();

   connect(&m_configurator, SIGNAL(selectionRadiusChanged(double)),
      this, SLOT(selectionRadiusChanged(double)));

   connect(&m_configurator, SIGNAL(newMoleculeRequested()),
      this, SLOT(newMoleculeRequested()));

   setAlpha(s_selectionColor[3]);
   selectionChanged();
}


void Octree::selectionChanged()
{
   m_selectedAtoms.clear();
   for (auto atom : m_atomList) {
       if (atom->isSelected()) m_selectedAtoms.append(atom);
   }
//qDebug() << "Selection now has" << m_selectedAtoms.size() << "atoms";
}


void Octree::selectionRadiusChanged(double radius)
{
   m_selectionRadius = radius;
   AtomList atoms(atomsInDaHood());
   unsigned nHits(atoms.size());
   
//qDebug() << "Selection radius now:" << m_selectionRadius << "with hits =" << nHits;
   m_configurator.hits(nHits); 
   updated();
}



AtomList Octree::atomsInDaHood() 
{
   // accumulate the hits for each atom, including duplicates
   std::vector<uint32_t> hits;
   for (auto atom : m_selectedAtoms) {
       auto v = neighborhood(atom->getPosition(), m_selectionRadius);
       hits.insert(hits.end(), v.begin(), v.end());
   }

   // now get a list of the unique atoms only
   std::sort(hits.begin(), hits.end());
   std::vector<uint32_t>::iterator it;
   it = std::unique(hits.begin(), hits.end());
   hits.resize(std::distance(hits.begin(), it));

   AtomList atoms;
   for (auto idx : hits)  atoms.append(m_atomList[idx]);       
   return atoms;
}


void Octree::newMoleculeRequested()
{
   AtomList atoms(atomsInDaHood());
   newMoleculeRequested(atoms);
}


void Octree::draw()
{
   if (checkState() != Qt::Checked) return;

   glLineWidth(s_lineWidth);
   glColor3f(0.7, 0.7, 0.7);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_LINE_SMOOTH);
   glDisable(GL_LIGHTING);
   glPushMatrix();
   glMultMatrixd(m_frame.matrix());

   glBegin(GL_LINES);
      glVertex3f(m_boundingBoxMin.x, m_boundingBoxMin.y, m_boundingBoxMin.z);
      glVertex3f(m_boundingBoxMax.x, m_boundingBoxMin.y, m_boundingBoxMin.z);

      glVertex3f(m_boundingBoxMax.x, m_boundingBoxMin.y, m_boundingBoxMin.z);
      glVertex3f(m_boundingBoxMax.x, m_boundingBoxMax.y, m_boundingBoxMin.z);

      glVertex3f(m_boundingBoxMax.x, m_boundingBoxMax.y, m_boundingBoxMin.z);
      glVertex3f(m_boundingBoxMin.x, m_boundingBoxMax.y, m_boundingBoxMin.z);

      glVertex3f(m_boundingBoxMin.x, m_boundingBoxMax.y, m_boundingBoxMin.z);
      glVertex3f(m_boundingBoxMin.x, m_boundingBoxMin.y, m_boundingBoxMin.z);

      glVertex3f(m_boundingBoxMin.x, m_boundingBoxMin.y, m_boundingBoxMax.z);
      glVertex3f(m_boundingBoxMax.x, m_boundingBoxMin.y, m_boundingBoxMax.z);

      glVertex3f(m_boundingBoxMax.x, m_boundingBoxMin.y, m_boundingBoxMax.z);
      glVertex3f(m_boundingBoxMax.x, m_boundingBoxMax.y, m_boundingBoxMax.z);

      glVertex3f(m_boundingBoxMax.x, m_boundingBoxMax.y, m_boundingBoxMax.z);
      glVertex3f(m_boundingBoxMin.x, m_boundingBoxMax.y, m_boundingBoxMax.z);

      glVertex3f(m_boundingBoxMin.x, m_boundingBoxMax.y, m_boundingBoxMax.z);
      glVertex3f(m_boundingBoxMin.x, m_boundingBoxMin.y, m_boundingBoxMax.z);

      glVertex3f(m_boundingBoxMin.x, m_boundingBoxMin.y, m_boundingBoxMin.z);
      glVertex3f(m_boundingBoxMin.x, m_boundingBoxMin.y, m_boundingBoxMax.z);

      glVertex3f(m_boundingBoxMax.x, m_boundingBoxMin.y, m_boundingBoxMin.z);
      glVertex3f(m_boundingBoxMax.x, m_boundingBoxMin.y, m_boundingBoxMax.z);

      glVertex3f(m_boundingBoxMax.x, m_boundingBoxMax.y, m_boundingBoxMin.z);
      glVertex3f(m_boundingBoxMax.x, m_boundingBoxMax.y, m_boundingBoxMax.z);

      glVertex3f(m_boundingBoxMin.x, m_boundingBoxMax.y, m_boundingBoxMin.z);
      glVertex3f(m_boundingBoxMin.x, m_boundingBoxMax.y, m_boundingBoxMax.z);
   glEnd();


   if (m_configurator.isVisible() && !m_selectedAtoms.isEmpty()) {
      drawSelection();
   }

   glPopMatrix();
   glDisable(GL_BLEND);
   glEnable(GL_LIGHTING);
   glDisable(GL_LINE_SMOOTH);
}

void Octree::drawSelection()
{
   int n(std::floor(16*m_selectionRadius));
   n = std::max(16, n);
   glColor4fv(s_selectionColor);
   GLUquadric* quad = gluNewQuadric();

   glDisable(GL_LIGHTING);

   glClearStencil(0x4);
   glClear(GL_STENCIL_BUFFER_BIT);
   glEnable(GL_STENCIL_TEST);
   glStencilFunc(GL_ALWAYS, 0x0, 0x4);
   glStencilMask(0x4);
   glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

/*
   for (auto atom : m_selectedAtoms) {
       Vec pos(atom->getPosition());
       glPushMatrix();
       glTranslatef(pos.x, pos.y, pos.z);
       gluSphere(quad, m_selectionRadius, n, n);
       glPopMatrix();
   }
*/

   glStencilFunc(GL_EQUAL, 0x4, 0x4);
   glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT);

   glEnable(GL_BLEND);
   glBlendFunc(GL_ONE, GL_ONE);

   for (auto atom : m_selectedAtoms) {
       Vec pos(atom->getPosition());
       glPushMatrix();
       glTranslatef(pos.x, pos.y, pos.z);
       gluSphere(quad, m_selectionRadius, n, n);
       glPopMatrix();
   }

   gluDeleteQuadric(quad);

   glDisable(GL_STENCIL_TEST);
   glDisable(GL_BLEND);
   glEnable(GL_LIGHTING);
}


void Octree::computeBoundingBox()
{
   double min = std::numeric_limits<double>::min();
   double max = std::numeric_limits<double>::max();
   
   m_boundingBoxMin = {max, max, max};
   m_boundingBoxMax = {min, min, min};

   for (auto atom : m_atomList) {
       Vec pos(atom->getPosition());

       m_boundingBoxMin.x = std::min(m_boundingBoxMin.x, pos.x);
       m_boundingBoxMin.y = std::min(m_boundingBoxMin.y, pos.y);
       m_boundingBoxMin.z = std::min(m_boundingBoxMin.z, pos.z);

       m_boundingBoxMax.x = std::max(m_boundingBoxMax.x, pos.x);
       m_boundingBoxMax.y = std::max(m_boundingBoxMax.y, pos.y);
       m_boundingBoxMax.z = std::max(m_boundingBoxMax.z, pos.z);
   }
}

std::vector<uint32_t> Octree::neighborhood(Point const& origin, double const radius)
{
    std::vector<uint32_t> results;

    m_octree.radiusNeighbors<unibn::L2Distance<Point> >(origin, radius, results);

    unsigned hits(results.size());
    //qDebug() << hits << "radius neighbors (r ="<< radius << ") found for (" 
    //         << origin.x << "," << origin.y << "," << origin.z << ")" ;

    return results;
}


void Octree::boxAtoms()
{
   m_points.clear();
   m_points.reserve(m_atomList.size());

   for (auto atom : m_atomList) {
       m_points.push_back(atom->getPosition());
   }

   m_octree.clear();
   m_octree.initialize(m_points);
}


} } // end namespace IQmol::Layer
