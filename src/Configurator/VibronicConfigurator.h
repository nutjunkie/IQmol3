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
#include "Data/Vibronic.h"
#include <QPen>
#include <QBrush>


class QCPGraph;

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
         void load() { }

      private Q_SLOTS:
         void plotSelectionChanged(bool);
         void setSelectionRectMode(QMouseEvent* e);
         void on_spectrumTable_itemSelectionChanged();
         void on_spectrumTable_cellDoubleClicked(int row, int col);
         void on_clearSelectionButton_clicked();
         void on_selectAllButton_clicked();
         void on_originTransition_clicked(bool);

         void on_fcButton_clicked(bool);
         void on_htButton_clicked(bool);
         void on_fchtButton_clicked(bool);

      private:
         Ui::VibronicConfigurator m_configurator;
         Layer::Vibronic& m_vibronic;
         CustomPlot* m_plotCanvas;
         Data::VibronicSpectrum::Theory m_currentTheory;

         void initPlotCanvas();
         void initGraphs();
         void initTable();

         void resetTable(Data::VibronicSpectrum::Theory);
         void resetCanvas(Data::VibronicSpectrum::Theory);

         void clearAllGraphs();
         void selectGraph(QCPGraph*, bool);
         void closeEvent(QCloseEvent*);

         // Mode -2       debug spectrum formed by summing individual modes
         // Mode -1       corresponds to the total spectra
         // Mode 0 -> n-1 correspond to the individual mode spectra
         // Mode n        is the electronic only impulse
         typedef QPair<Data::VibronicSpectrum::Theory, int> ModeIndex;
         QMap<ModeIndex, QCPGraph*> m_modeMap;
   };

} } // end namespace IQmol::Configurator

#endif
