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

#include "Data/DataList.h"
#include "Data/Mesh.h"
#include "Math/Function.h"

#include <QDebug>

namespace IQmol {

namespace Property {
   class Base;
}

namespace Data {

   class SurfaceInfo;

   class Surface : public Base {

      public:
         Type::ID typeID() const { return Type::Surface; }

         Surface(SurfaceInfo const&);
         Surface(Mesh const&);
         Surface() { }  // for serialization

         void clearSurfaceProperty();

         void computeSurfaceProperty(Function3D const&);
         void computeSurfaceProperty(Data::Mesh::VertexFunction const&);
         void computeSurfaceProperty(Property::Base*);

         void getPropertyRange(double& min, double& max) const;
         void setPropertyRange(double const min, double const max);

         QList<QColor> const& colors() const { return m_colors; }
         void setColors(QList<QColor> const& colors);

         bool blend() const { return m_blend; }
         void setBlend(bool const blend) { m_blend = blend; }

         double opacity() const { return m_opacity; }
         void setOpacity(double const opacity ) { m_opacity = opacity; }
         bool isVisible() const { return m_isVisible; }
         void setVisibility(bool const tf) { m_isVisible = tf; }

         bool hasProperty() const;
         bool propertyIsSigned() const;
         bool isSigned() const { return m_isSigned; }
         QString const& description() const { return m_description; }
         void setDescription(QString const& description) { m_description = description; }

         Mesh& meshPositive() { return m_meshPositive; }
         Mesh& meshNegative() { return m_meshNegative; }

         double area() const;

         void dump() const;

      private:
         void computeSurfacePropertyRange();

         QString m_description;
         QList<QColor> m_colors;
         double m_opacity;
         bool   m_blend;
         bool   m_isSigned;
         bool   m_isVisible;
         Mesh   m_meshPositive;
         Mesh   m_meshNegative;

         // These don't need to be serialized
         double m_min;
         double m_max;
   };

   class SurfaceList : public Data::List<Data::Surface>  {
         IQmol::Data::Type::ID typeID() const { return IQmol::Data::Type::SurfaceList; }
   };

} } // end namespace IQmol::Data
