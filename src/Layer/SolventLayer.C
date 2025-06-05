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

#include "SolventLayer.h"


using namespace qglviewer;

namespace IQmol {
namespace Layer {


Solvent::Solvent(Data::Solvent const& solvent) : m_solvent(solvent)
{
   setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled |
      Qt::ItemIsEditable);
               
   setCheckState(Qt::Unchecked);

   m_color[0] = 0.0;
   m_color[1] = 0.8;
   m_color[2] = 1.0;
   setAlpha(0.2);

   //setConfigurator(&m_configurator); 
   //m_configurator.setWindowTitle("Configure Surface: " + m_surface.description());
   setText("Solvent");
}


void Solvent::draw() 
{
   double radius(m_solvent.radius());
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 
   glColor4fv(m_color);
   GLUquadric* quad = gluNewQuadric();

   glPushMatrix();
   glMultMatrixd(m_frame.matrix());

   size_t n(m_solvent.nCenters());
   for (size_t i(0); i < n; ++i) {
       glPushMatrix();
       Vec const& v(m_solvent[i]);
       glTranslatef(v.x, v.y, v.z);
       gluSphere(quad, radius, 16, 16);
       glPopMatrix();
   }

   glPopMatrix();
   gluDeleteQuadric(quad); 
}


void Solvent::setAlpha(double const alpha) 
{
   m_alpha = alpha;
   m_color[3] = m_alpha;
}


} } // end namespace IQmol::Layer
