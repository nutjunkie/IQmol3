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

#include "GLObjectLayer.h"
#include "AtomLayer.h"
#include "Util/Octree.h"
#include "Configurator/OctreeConfigurator.h"


namespace IQmol {

   namespace Layer {


      class Octree : public GLObject
      {
         Q_OBJECT

         typedef qglviewer::Vec Point;

         public:
            Octree(AtomList const&);

            void draw();

         Q_SIGNALS:
            void newMoleculeRequested(AtomList const&);

         public Q_SLOTS:
            void selectionChanged();

         private Q_SLOTS:
            void selectionRadiusChanged(double);
            void newMoleculeRequested();

         private:
            static double s_lineWidth;
            static GLfloat s_selectionColor[4];

            std::vector<uint32_t> neighborhood(Point const& center, double const radius);
            AtomList atomsInDaHood();
            void computeBoundingBox();
            void drawSelection();
            void boxAtoms();

            qglviewer::Vec m_boundingBoxMin;
            qglviewer::Vec m_boundingBoxMax;

            AtomList m_atomList;
            AtomList m_selectedAtoms;

            unibn::Octree<Point> m_octree;
            std::vector<Point> m_points;

            Configurator::Octree m_configurator;
            double m_selectionRadius;
      };

   } 
} // end namespace IQmol::Layer

