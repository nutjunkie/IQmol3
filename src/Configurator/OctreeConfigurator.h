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

#include "Configurator.h"
#include "ui_OctreeConfigurator.h"


namespace IQmol {

namespace Layer {
   class Octree;
}

namespace Configurator {

   class Octree: public Base {

      Q_OBJECT

      public:
         Octree(Layer::Octree& octree);
         void hits(unsigned);

      Q_SIGNALS:
         void selectionRadiusChanged(double);

      public Q_SLOTS:
         void init();
         void sync();
         void on_radiusSlider_valueChanged(int);
         void on_radiusSpin_valueChanged(double);

      private:
         Layer::Octree& m_octree;
         Ui::OctreeConfigurator m_configurator;
   };

} } // end namespace IQmol::Configurator
