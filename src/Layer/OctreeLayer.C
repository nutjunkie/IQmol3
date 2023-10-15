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
#include <limits>
#include <cmath>
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
   setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
   setCheckState(Qt::Checked);
   setConfigurator(&m_configurator);
   computeBoundingBox();

   qDebug() << "Bounding box min:" 
            << m_boundingBoxMin.x << m_boundingBoxMin.y << m_boundingBoxMin.z;
   qDebug() << "Bounding box max:" 
            << m_boundingBoxMax.x << m_boundingBoxMax.y << m_boundingBoxMax.z;

   boxAtoms();

//   connect(newAction("Find Neighborhood"), SIGNAL(triggered()),
//       this, SLOT(neighborhood()));

   connect(&m_configurator, SIGNAL(selectionRadiusChanged(double)),
      this, SLOT(selectionRadiusChanged(double)));

   setAlpha(s_selectionColor[3]);
}


void Octree::selectionRadiusChanged(double radius)
{
   m_selectionRadius = radius;
   
   unsigned hits = neighborhood(Point(0,0,0), m_selectionRadius);
qDebug() << "Selection radius now:" << m_selectionRadius << "with hits =" << hits;
   m_configurator.hits(hits); 
   updated();
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

   glEnable(GL_LIGHTING);

   if (m_configurator.isVisible()) {
      int n(std::floor(16*m_selectionRadius));
      glColor4fv(s_selectionColor);

      GLUquadric* quad = gluNewQuadric();
      glPushMatrix();
      glTranslatef(m_selectionCenter.x, m_selectionCenter.y, m_selectionCenter.z);
      gluSphere(quad, m_selectionRadius, n, n);
      glPopMatrix();
      gluDeleteQuadric(quad);
   }

   glPopMatrix();
   glDisable(GL_BLEND);
   glDisable(GL_LINE_SMOOTH);
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


/*
void Octree::neighborhood()
{
   neighborhood(Point(0,0,0), 10.0);
}
*/


unsigned Octree::neighborhood(Point const& origin, double const radius)
{
    std::vector<uint32_t> results;

    m_octree.radiusNeighbors<unibn::L2Distance<Point> >(origin, radius, results);

    unsigned hits(results.size());
    qDebug() << hits << "radius neighbors (r ="<< radius << ") found for (" 
             << origin.x << "," << origin.y << "," << origin.z << ")" ;

    return hits;
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
