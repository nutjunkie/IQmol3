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
#include "Configurator/ui_ExcitedStatesConfigurator.h"
#include "Util/Constants.h"
#include <QPen>


class QCPItemText;
class QCPAbstractItem;


namespace IQmol {

class CustomPlot;

namespace Data {
   class ExcitedStates;
}

namespace Layer {
   class ExcitedStates;
}

namespace Configurator {

   class ExcitedStates : public Base {

      Q_OBJECT

      public:
         explicit ExcitedStates(Layer::ExcitedStates&);
         ~ExcitedStates();

      private Q_SLOTS:
         void moSelectionChanged(bool);
         void plotSelectionChanged(bool);
         void setSelectionRectMode(QMouseEvent* e);

         void on_impulseButton_clicked(bool);
         void on_gaussianButton_clicked(bool);
         void on_lorentzianButton_clicked(bool);
         void on_widthSlider_valueChanged(int);
         void on_energyTable_itemSelectionChanged();
         void on_resetZoomButton_clicked(bool);
         void on_unitsCombo_currentIndexChanged(int);

      private:
         enum Profile { Gaussian, Lorentzian };

         void initTable();
         void initMoPlot();
         void initSpectrum();

         void updateTable();
         void updateSpectrum();
         void updateMoPlot(int const index);

         void plotImpulse();
         void plotSpectrum(Profile const, double const width);
         void clearTransitionLines();

         Layer::ExcitedStates& m_excitedStates; 
         Ui::ExcitedStatesConfigurator m_configurator;

         CustomPlot* m_moPlot;
         CustomPlot* m_spectrum;
         QCPItemText* m_label;
         QPen m_pen;
         QPen m_selectedPen;

         // Keep units in eV and only scale user-side
         Constants::Units m_units;
         QPair<double, double> m_maxValues;  

         QList<QCPAbstractItem*> m_transitionLines;
   };

} } // End namespace IQmol::Configurator
