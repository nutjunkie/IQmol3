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

#include "OctreeConfigurator.h"
#include "Layer/OctreeLayer.h"


using namespace qglviewer;

namespace IQmol {
namespace Configurator {

Octree::Octree(Layer::Octree& octree) : m_octree(octree) 
{ 
   m_configurator.setupUi(this); 
   m_configurator.hitsTextEdit->setText(QString::number(0));
}


void Octree::init()
{
}


void Octree::sync()
{
}


void Octree::hits(unsigned n)
{
   m_configurator.hitsTextEdit->setText(QString::number(n));
}



void Octree::on_radiusSlider_valueChanged(int value)
{
   double x(value*value*0.01);
   double spin(m_configurator.radiusSpin->value());

   if (std::abs(x-spin) > 0.01) {
      m_configurator.radiusSpin->setValue(x);
   }
    selectionRadiusChanged(x); 
}


void Octree::on_radiusSpin_valueChanged(double value)
{
    int x(std::round(std::sqrt(100*value)));
    int slider(m_configurator.radiusSlider->value());
    if (x != slider) {
       m_configurator.radiusSlider->setValue(x);
    }
   
    selectionRadiusChanged(value); 
}



} } // end namespace IQmol::Configurator
