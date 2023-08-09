#pragma once
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

#include "GLObjectLayer.h"
#include "Data/Solvent.h"
#include <QMap>


class QColor;

namespace IQmol {

namespace Layer {

   class Solvent : public GLObject {

      Q_OBJECT

      public:
         Solvent(Data::Solvent const& solvent);

         void draw();
         void drawFast() { draw(); }
         void drawSelected() { draw(); }

         QColor color() const { 
             QColor col;
             col.setRgbF(m_color[0],m_color[1],m_color[2],m_color[3]); 
             return col;
         }

      protected:
         GLfloat m_color[4];

      private:
         void setAlpha(double const alpha);
         Data::Solvent const& m_solvent;
   };

} } // end namespace IQmol::Layer
