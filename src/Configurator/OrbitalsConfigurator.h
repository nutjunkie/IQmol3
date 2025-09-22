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
#include "Configurator/ui_OrbitalsConfigurator.h"
#include "Data/SurfaceInfo.h"
#include "Math/Matrix.h"
#include <QPen>


class QMouseEvent;

namespace IQmol {

class CustomPlot;

namespace Layer {
   class Orbitals;
}

namespace Configurator {

   /// Dialog that allows the user to select orbitals (and densities) for plotting.
   class Orbitals : public Base {

      Q_OBJECT

      public:
         explicit Orbitals(Layer::Orbitals&);
         ~Orbitals();

      public Q_SLOTS:
         void init();

      Q_SIGNALS:
         void queueSurface(Data::SurfaceInfo const&);
         void calculateSurfaces();
         void clearQueue();

      private Q_SLOTS:
         void on_surfaceType_currentIndexChanged(int);
         void on_addToQueueButton_clicked(bool);
         void on_calculateButton_clicked(bool);
         void on_cancelButton_clicked(bool);
         void on_positiveColorButton_clicked(bool);
         void on_negativeColorButton_clicked(bool);
         void on_mullikenDecompositionsButton_clicked(bool);
         void plotSelectionChanged(bool tf);
         void clearSelectedOrbitals(int);
         void setMullikenDecompositions(Matrix const&);
         void on_isovalueRadio_clicked(bool);
         void on_percentageRadio_clicked(bool);
         void on_atomicUnits_clicked(bool);

         void on_densityMatrixButton_clicked(bool);

      private:
         void enableOrbitalSelection(bool);
         void enableNegativeColor(bool);
         void enableMullikenDecompositions(bool);
         void updateOrbitalRange(bool alpha);
         void updateBasisRange();
         void setPositiveColor(QColor const& color);
         void setNegativeColor(QColor const& color);
         void setIsovalueUnits(bool atomicUnits);
         void initPlot();

         Layer::Orbitals& m_orbitals;
         Ui::OrbitalsConfigurator m_configurator;

         unsigned int m_nAlpha;
         unsigned int m_nBeta;
         unsigned int m_nOrbitals;
         CustomPlot* m_customPlot;
         QPen m_pen;
         QPen m_selectedPen;
   };

} } // End namespace IQmol::Configurator
