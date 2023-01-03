#ifndef IQMOL_CONFIGURATOR_VIBRONIC_H
#define IQMOL_CONFIGURATOR_VIBRONIC_H
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
#include "Configurator/ui_VibronicConfigurator.h"
#include <QPen>
#include <QBrush>


namespace IQmol {

class CustomPlot;

namespace Layer {
   class Vibronic;
}

namespace Configurator {

   class Vibronic : public Base {

      Q_OBJECT

      friend class Layer::Vibronic;

      public:
         explicit Vibronic(Layer::Vibronic&);
         ~Vibronic();
      
      Q_SIGNALS:
         void update();

      public Q_SLOTS:
         void reset();

      protected:
         void load();

      private Q_SLOTS:
         void plotSelectionChanged(bool);
         void updatePlot();

      private:
         Ui::VibronicConfigurator m_configurator;
         Layer::Vibronic& m_vibronic;
         CustomPlot* m_spectrum;

         QPen m_pen;
         QPen m_selectPen;

         void initSpectrum();
         void plotImpulse(double const scaleFactor);
         void plotSpectrum(double const scaleFactor, double const width);
   };

} } // end namespace IQmol::Configurator

#endif
