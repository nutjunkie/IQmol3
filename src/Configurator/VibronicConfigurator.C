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

#include "VibronicConfigurator.h"
#include "Layer/VibronicLayer.h"
#include "Data/Vibronic.h"
#include "CustomPlot.h"



namespace IQmol {
namespace Configurator { 

Vibronic::Vibronic(Layer::Vibronic& vibronic) : m_vibronic(vibronic) 
{
   m_configurator.setupUi(this);

   m_configurator.widthSlider->setEnabled(false);
   m_configurator.widthLabel->setEnabled(false);
   m_configurator.spectrumTable->horizontalHeader()
      ->setSectionResizeMode(QHeaderView::Stretch);

   connect(this, SIGNAL(update()), &m_vibronic, SIGNAL(update()));

   m_pen.setColor(Qt::blue);
   m_pen.setStyle(Qt::SolidLine);
   m_pen.setWidthF(1);

   m_selectPen.setColor(Qt::red);
   m_selectPen.setStyle(Qt::SolidLine);
   m_selectPen.setWidthF(3);

   load();
   initSpectrum();
}


Vibronic::~Vibronic()
{
   if (m_spectrum) delete m_spectrum;
}


void Vibronic::load()
{
   QTableWidget* table(m_configurator.spectrumTable);
   Data::VibronicSpectrumList const& spectra(m_vibronic.spectra());
   table->setRowCount(spectra.size());

   int row(0);
   Data::VibronicSpectrumList::const_iterator iter;
   qDebug() << " Loading spectra" << spectra.size();
   for (iter = spectra.begin(); iter != spectra.end(); ++iter, ++row) {
       QTableWidgetItem* item = new QTableWidgetItem((*iter)->title());
       item->setTextAlignment(Qt::AlignCenter|Qt::AlignVCenter);
       item->setData(Qt::UserRole, QVariantPtr<Data::VibronicSpectrum>::toQVariant(*iter));
       table->setItem(row, 0, item);
   }
}


void Vibronic::initSpectrum()
{
   m_spectrum = new CustomPlot();
   m_spectrum->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
   m_spectrum->axisRect()->setRangeDrag(Qt::Horizontal);
   m_spectrum->axisRect()->setRangeZoom(Qt::Horizontal);

   m_spectrum->xAxis->setSelectableParts(QCPAxis::spNone);
   m_spectrum->xAxis->setLabel("Frequency");
   m_spectrum->yAxis->setLabel("Intensity");

   connect(m_spectrum, SIGNAL(mousePress(QMouseEvent*)),
      this, SLOT(setSelectionRectMode(QMouseEvent*)));

   QFrame* frame(m_configurator.spectrumFrame);
   QVBoxLayout* layout(new QVBoxLayout());
   frame->setLayout(layout);
   layout->addWidget(m_spectrum);
}



void Vibronic::updatePlot()
{
   m_spectrum->clearGraphs();
   double scale(m_configurator.scaleFactor->value());
   double width(m_configurator.widthSlider->value());

   if (m_configurator.impulseButton->isChecked()) {
      plotImpulse(scale);
      m_spectrum->yAxis->setLabel("Intensity");

   }else if (m_configurator.gaussianButton->isChecked()) {
      plotSpectrum(scale, width);
      m_spectrum->yAxis->setLabel("Rel. Intensity");

   }else if (m_configurator.lorentzianButton->isChecked()) {
      plotSpectrum(scale, width);
      m_spectrum->yAxis->setLabel("Rel. Intensity");

   }

   m_spectrum->replot();
}


void Vibronic::plotSelectionChanged(bool tf)
{
   QCPGraph* graph(qobject_cast<QCPGraph*>(sender()));
   if (!graph) return;

   if (tf) {
      graph->setPen(m_selectPen);
      graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc));
   }else {
      graph->setPen(m_pen);
      graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle));
      return;
   }

   if (!tf) return;
   if (!m_configurator.impulseButton->isChecked()) return;

   bool ok;
   int mode(graph->name().toInt(&ok));
   if (!ok) return;

   QTableWidget* table(m_configurator.spectrumTable);
   table->setCurrentCell(mode, 0, 
      QItemSelectionModel::Rows | QItemSelectionModel::ClearAndSelect);
   table->scrollToItem(table->item(mode,0));
}

void Vibronic::plotImpulse(double const)
{
}


void Vibronic::plotSpectrum(double const, double const)
{
}

void Vibronic::reset()
{
}

} } // end namespace IQmol::Configurator
