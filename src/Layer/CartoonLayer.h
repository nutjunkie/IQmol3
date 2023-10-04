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
#include "Configurator/SurfaceConfigurator.h"
#include "Data/PdbData.h"
#include <QColor>


namespace IQmol {

   class MeshDecimatorTask;

   namespace Layer {

      class Molecule;

      class Cartoon : public GLObject {
   
         Q_OBJECT
   
         friend class Configurator::Surface;
         friend class Animator::Combo;
   
         public: 
            Cartoon(Data::Pdb&);
            ~Cartoon();
   
            void draw();
            void drawFast();
            void drawSelected();
            void setAlpha(double const alpha);
            void setDrawMode(GLObject::DrawMode const mode) { m_drawMode = mode; }
            void setClip(bool const tf);
            void povray(PovRayGen&);

            void setMolecule(Molecule*);
            void setCheckStatus(Qt::CheckState const);

         protected:
            void setColors(QList<QColor> const& colors);
            void setColors(QColor const& negative, QColor const& positive);
            QColor colorPositive() const;
            QColor colorNegative() const;

            QList<QColor> const& colors() const;

            void computePropertyData(Function3D const&);
            void computeIndexField();
            void clearPropertyData();
            bool isSigned() const { return false; }
            bool hasProperty() const { return m_surface.hasProperty(); }
            bool propertyIsSigned() const { return m_surface.propertyIsSigned(); }
            void getPropertyRange(double& min, double& max) const;
            void setPropertyRange(double const min, double const max);
            double area() const { return m_surface.area(); }
            void balanceScale(bool const);

         private Q_SLOTS:
            void toggleVertexNormals();
            void toggleFaceNormals();
            void decimate();
            void decimateFinished();
            void dumpMeshInfo() const;
   
         private:
            void recompile();
            GLuint compile(Data::Mesh const&);
            bool isTransparent() const { return 0.01 <= m_alpha && m_alpha < 0.99; }
            void drawVertexNormals();
            void drawFaceNormals();
            void drawVertexNormals(Data::Mesh const&);
            void drawFaceNormals(Data::Mesh const&);
   
            Data::Surface& m_surface;
            Configurator::Surface m_configurator;
            GLObject::DrawMode m_drawMode;
   
            GLuint  m_callListPositive; 
            GLfloat m_colorPositive[4];
            GLfloat m_colorNegative[4];

            bool m_drawVertexNormals;
            bool m_drawFaceNormals;
            bool m_balanceScale;  // for properties

            Molecule* m_molecule;
            MeshDecimatorTask* m_decimator;
            void povray(PovRayGen&, Data::OMMesh const&, QColor const&);
            void povrayLines(PovRayGen&, Data::OMMesh const&, QColor const&);
      };
   
   } // end namespace Layer
   
} // end namespace IQmol
