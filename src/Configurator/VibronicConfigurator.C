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
#include "Util/Color.h"
#include "Util/QsLog.h"
#include "Util/NumericalTableItem.h"
#include "CustomPlot.h"
#include <algorithm>


namespace IQmol {
namespace Configurator { 


Vibronic::Vibronic(Layer::Vibronic& vibronic) : m_vibronic(vibronic),
   m_currentTheory(Data::VibronicSpectrum::FC), m_units(Constants::Wavenumber)
{
   m_configurator.setupUi(this);

   initPlotCanvas();
   initTable();
   updateSpectra(m_units);

   m_configurator.unitsCombo->clear();
   for (const auto unit : Constants::AllUnits) {
       m_configurator.unitsCombo->addItem(Constants::toString(unit), unit);
   }

   connect(m_configurator.initialButton, SIGNAL(clicked(bool)), 
      &m_vibronic, SLOT(connectInitialFrequencies(bool)));
   connect(m_configurator.finalButton, SIGNAL(clicked(bool)), 
      &m_vibronic, SLOT(connectFinalFrequencies(bool)));

   Data::Vibronic const& data(m_vibronic.vibronicData());

   if (!data.hasTheory(Data::VibronicSpectrum::FC)) {
      m_configurator.fcButton->setEnabled(false);
   }
   if (!data.hasTheory(Data::VibronicSpectrum::FC)) {
      m_configurator.htButton->setEnabled(false);
   }
   if (!data.hasTheory(Data::VibronicSpectrum::FC)) {
      m_configurator.fchtButton->setEnabled(false);
   }

   reset();

   m_configurator.unitsCombo->setCurrentIndex(Constants::Wavenumber);
}


Vibronic::~Vibronic()
{
   if (m_plotCanvas) delete m_plotCanvas;
}


void Vibronic::initPlotCanvas()
{
   m_plotCanvas = new CustomPlot();
   m_plotCanvas->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
   m_plotCanvas->setAntialiasedElements(QCP::aePlottables);
   m_plotCanvas->axisRect()->setRangeDrag(Qt::Horizontal);
   m_plotCanvas->axisRect()->setRangeZoom(Qt::Horizontal);

   m_plotCanvas->xAxis->setSelectableParts(QCPAxis::spNone);
   m_plotCanvas->yAxis->setLabel("Intensity");
   m_plotCanvas->legend->setVisible(true);

   connect(m_plotCanvas, SIGNAL(mousePress(QMouseEvent*)),
      this, SLOT(setSelectionRectMode(QMouseEvent*)));

   QFrame* frame(m_configurator.spectrumFrame);
   QVBoxLayout* layout(new QVBoxLayout());
   frame->setLayout(layout);
   layout->addWidget(m_plotCanvas);
}


void Vibronic::initTable()
{
   QTableWidget* table(m_configurator.spectrumTable);
   table->setSortingEnabled(true);

   QHeaderView* header(table->horizontalHeader());
   header->setSectionResizeMode(QHeaderView::Stretch);
   header->setSortIndicatorShown(true);

   Data::Vibronic const& vibronic(m_vibronic.vibronicData());

   QList<double> const& initialFrequencies(vibronic.initialFrequencies()); 
   QList<double> const& finalFrequencies(vibronic.finalFrequencies()); 

   unsigned nModes(initialFrequencies.size());
   if (nModes == 0) return;
   table->setRowCount(nModes);

   for (unsigned mode(0); mode < nModes; ++mode) {
       QTableWidgetItem* freq0 = new DoubleTableWidgetItem();
       freq0->setData(Qt::UserRole, mode);
       freq0->setText(QString::number(initialFrequencies[mode], 'f', 2) + "    ");
       freq0->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
       table->setItem(mode, 0, freq0);

       QTableWidgetItem* freq1 = new DoubleTableWidgetItem();
       freq1->setData(Qt::UserRole, mode);
       freq1->setText(QString::number(finalFrequencies[mode], 'f', 2) + "    ");
       freq1->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
       table->setItem(mode, 1, freq1);

       QTableWidgetItem* intensity = new DoubleTableWidgetItem();
       intensity->setData(Qt::UserRole, mode);
       intensity->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
       table->setItem(mode, 2, intensity);
   }
}


void Vibronic::updateSpectra(Constants::Units const units)
{
   QString label("Energy / ");
   label += Constants::toString(units);
   m_plotCanvas->xAxis->setLabel(label);
   m_plotCanvas->clearGraphs();

   Data::Vibronic const& vibronic(m_vibronic.vibronicData());
   unsigned nModes(vibronic.nModes());
   unsigned nPoints(vibronic.nPoints());

   if (nPoints == 0) return;

   double xmin(vibronic.frequencyDomainMin());
   double xmax(vibronic.frequencyDomainMax());
   double delta(vibronic.frequencyDomainDelta());

   QVector<double> x(nPoints);
   for (unsigned xi = 0; xi < nPoints; ++xi) {
       x[xi] = Constants::convert(xmin + xi*delta, Constants::Wavenumber, units);
   }   

   Color::List const& colors = { 
      QColor("#a6cee3"), QColor("#1f78b4"), QColor("#b2df8a"), QColor("#33a02c"),
      QColor("#fb9a99"), QColor("#e31a1c"), QColor("#fdbf6f"), QColor("#ff7f00"),
      QColor("#cab2d6"), QColor("#6a3d9a")
   };

   QCPGraph* graph;

   for (const auto theory : Data::VibronicSpectrum::AllTheories) {

       if (!vibronic.hasTheory(theory)) continue;
       
       for (int mode = -1; mode < (int)nModes; ++mode) {
           Data::VibronicSpectrum const& spectrum(vibronic.spectrum(theory, mode));

           QVector<double> y(nPoints);
           for (unsigned xi = 0; xi < nPoints; ++xi) {
               y[xi] = spectrum.data()[xi];
           }   

           QPen pen;
           pen.setStyle(Qt::SolidLine);
           if (mode == -1) {
              pen.setWidthF(2);
              pen.setColor(Qt::darkGray);
           }else if (mode == -2) {
              pen.setWidthF(2);
              pen.setColor(Qt::darkGray);
              pen.setStyle(Qt::DotLine);
           }else {
              pen.setWidthF(1);
              pen.setColor(colors[mode % colors.size()]);
           }

           graph = m_plotCanvas->addGraph();
           graph->setData(x, y); 
           graph->setPen(pen);
           pen.setWidthF(3);
           graph->selectionDecorator()->setPen(pen);
           graph->setName("Mode " + QString::number(mode+1));
           connect(graph, SIGNAL(selectionChanged(bool)), this, SLOT(plotSelectionChanged(bool)));
           ModeIndex index(theory, mode);
           m_modeMap[index] = graph;
       } 

       { // Add electronic transition impulse
           QPen pen;
           pen.setStyle(Qt::SolidLine);
           QColor color;
           color.setHsv(280, 255, 255);
           pen.setColor(color);
           pen.setWidthF(2);

           QVector<double> x(1), y(1);
           x[0] = Constants::convert(vibronic.electronicEnergy(), Constants::Wavenumber, units);
           y[0] = 0.25*vibronic.spectrum(theory,-1).max();

           graph = m_plotCanvas->addGraph();
           graph->setData(x, y);
           graph->setName("Electronic Transition");
           graph->setLineStyle(QCPGraph::lsImpulse);
           graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle));
           graph->setPen(pen);
           graph->setSelectable(QCP::stNone);

           ModeIndex idx(theory, (int)nModes);
           m_modeMap[idx] = graph;
       }
   }

   if (vibronic.hasTheory(Data::VibronicSpectrum::FC)) {
      graph = m_modeMap[ModeIndex(Data::VibronicSpectrum::FC,-1)];
      graph->setSelectable(QCP::stNone);
      graph->setName("Franck-Condon");
   }

   if (vibronic.hasTheory(Data::VibronicSpectrum::HT)) {
      graph = m_modeMap[ModeIndex(Data::VibronicSpectrum::HT,-1)];
      graph->setName("Herzberg-Teller");
      graph->setSelectable(QCP::stNone);
   }

  
   if (vibronic.hasTheory(Data::VibronicSpectrum::FCHT)) {
      graph = m_modeMap[ModeIndex(Data::VibronicSpectrum::FCHT,-1)];
      graph->setName("FC + HT");
      graph->setSelectable(QCP::stNone);
   }

   xmin = std::min(x.first(), x.last());
   xmax = std::max(x.first(), x.last());
   m_plotCanvas->xAxis->setRange(xmin, xmax);
   m_plotCanvas->replot();
}


void Vibronic::reset()
{
   Data::Vibronic const& vibronic(m_vibronic.vibronicData());

   double xmin(Constants::convert(vibronic.frequencyDomainMin(), 
      Constants::Wavenumber, m_units));
   double xmax(Constants::convert(vibronic.frequencyDomainMax(), 
      Constants::Wavenumber, m_units));
   m_plotCanvas->xAxis->setRange(std::min(xmin, xmax), std::max(xmin, xmax));

   on_fcButton_clicked(true);
}


void Vibronic::resetTable(Data::VibronicSpectrum::Theory theory)
{
   QTableWidget* table(m_configurator.spectrumTable);

   Data::Vibronic const& vibronic(m_vibronic.vibronicData());
   unsigned nRows(table->rowCount());
   bool ok(true);

   for (int row(0); row < nRows; ++row) {
       QTableWidgetItem* item = table->item(row, 2);
       int mode = item->data(Qt::UserRole).toInt(&ok);  if (!ok) return;
       double max(0.0); 
       if (vibronic.hasTheory(theory)) {
          Data::VibronicSpectrum const& spectrum(vibronic.spectrum(theory, mode));
          max = spectrum.max();
       }
       item->setText(QString::number(max, 'f', 5) + "    ");
   }
}


void Vibronic::resetCanvas(Data::VibronicSpectrum::Theory theory)
{
   clearAllGraphs();
   if (!m_vibronic.vibronicData().hasTheory(theory)) return;

   Data::VibronicSpectrum const& spectrum(m_vibronic.vibronicData().spectrum(theory, -1));
   double min(spectrum.min());
   double max(spectrum.max());


   ModeIndex index(theory, -1);
   QCPGraph* graph(m_modeMap[index]);
   if (graph) {
      graph->setVisible(true);
      graph->addToLegend(m_plotCanvas->legend);
   }

//--------------------------------------------------------
// The following is to show the additivity (or otherwise)
// of the mode spectra for debug purposes.
//--------------------------------------------------------
/*
   ModeIndex index2(theory, -2);
   graph = m_modeMap[index2];
   if (graph) {
      graph->setVisible(true);
      graph->addToLegend(m_plotCanvas->legend);

      Data::VibronicSpectrum const& spectrum(m_vibronic.vibronicData().spectrum(theory, -2));
      min = std::min(min, spectrum.min());
      max = std::max(max, spectrum.max());
   }else {
      qDebug() << "Failed to find summation plot";
   }
*/
//--------------------------------------------------------
 
   m_plotCanvas->yAxis->setRange(min,max);
   
   QList<QTableWidgetItem*> selection(m_configurator.spectrumTable->selectedItems());
   bool (ok);

   for (auto iter = selection.begin(); iter != selection.end(); ++iter) {
       int mode = (*iter)->data(Qt::UserRole).toInt(&ok);  
       if (ok) {
          ModeIndex index(m_currentTheory, mode);
          QCPGraph* graph = m_modeMap[index];
          if (graph) graph->setVisible(true);
       }
   }

   // Electronic transition
   int n = m_vibronic.vibronicData().nModes();
   bool checked(m_configurator.originTransition->isChecked());
   ModeIndex idx(m_currentTheory, n);
   graph = m_modeMap[idx];
   if (graph && checked) {
      graph->setVisible(true);
      graph->addToLegend(m_plotCanvas->legend);
   }

   m_vibronic.playMode(-1);
   m_plotCanvas->replot();
}


void Vibronic::on_fcButton_clicked(bool tf)
{
   m_currentTheory = Data::VibronicSpectrum::FC;
   resetTable(m_currentTheory);
   resetCanvas(m_currentTheory);
}


void Vibronic::on_htButton_clicked(bool tf)
{
   m_currentTheory = Data::VibronicSpectrum::HT;
   resetTable(m_currentTheory);
   resetCanvas(m_currentTheory);
}


void Vibronic::on_fchtButton_clicked(bool tf)
{
   m_currentTheory = Data::VibronicSpectrum::FCHT;
   resetTable(m_currentTheory);
   resetCanvas(m_currentTheory);
}


void Vibronic::clearAllGraphs()
{
   int count(m_plotCanvas->graphCount()); 
   for (int i(0); i < count; ++i) {
       selectGraph(m_plotCanvas->graph(i), false);
       m_plotCanvas->graph(i)->setVisible(false);
   }
   m_plotCanvas->legend->clearItems();
}


void Vibronic::on_spectrumTable_itemSelectionChanged() 
{
   resetCanvas(m_currentTheory);

   QList<QTableWidgetItem*> selection(m_configurator.spectrumTable->selectedItems());
   bool (ok);

   for (auto iter = selection.begin(); iter != selection.end(); ++iter) {
       int mode = (*iter)->data(Qt::UserRole).toInt(&ok);  
       if (ok) {
          ModeIndex index(m_currentTheory, mode);
          QCPGraph* graph = m_modeMap[index];
          if (graph) graph->setVisible(true);
       }
   }

   m_vibronic.playMode(-1);
   m_plotCanvas->replot();
}


void Vibronic::on_clearSelectionButton_clicked()
{
   m_configurator.spectrumTable->clearSelection();
   resetCanvas(m_currentTheory);
}


void Vibronic::on_selectAllButton_clicked()
{
   resetCanvas(m_currentTheory);
   m_configurator.spectrumTable->selectAll();
}


void Vibronic::on_originTransition_clicked(bool)
{
   resetCanvas(m_currentTheory);
}


void Vibronic::on_unitsCombo_currentIndexChanged(int)
{
   int u(m_configurator.unitsCombo->currentData().toInt());
   m_units = static_cast<Constants::Units>(u);
   updateSpectra(m_units);
   resetCanvas(m_currentTheory);
}


void Vibronic::on_spectrumTable_cellDoubleClicked(int row, int col) 
{
   QTableWidget* table(m_configurator.spectrumTable);

   int nCol(table->columnCount());
   for (int col(0); col < nCol; ++col) {
       table->item(row,col)->setSelected(true);
   }

   bool ok;
   int mode = table->item(row,0)->data(Qt::UserRole).toInt(&ok);  if (!ok) mode = -1;

   ModeIndex index(m_currentTheory,mode);
   QCPGraph* graph = m_modeMap[index];
   if (graph) selectGraph(graph, true);

   m_vibronic.playMode(mode);
}


void Vibronic::selectGraph(QCPGraph* graph, bool selected)
{
   if (selected) {
      graph->addToLegend(m_plotCanvas->legend);
      graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
   }else {
      graph->setSelection(QCPDataSelection());
      graph->removeFromLegend(m_plotCanvas->legend);
   }
   
   m_plotCanvas->replot();
}


void Vibronic::plotSelectionChanged(bool tf)
{
   QCPGraph* graph(qobject_cast<QCPGraph*>(sender()));
   m_vibronic.playMode(-1);
   if (!graph) return;

   selectGraph(graph, tf);

   if (!tf) return;

   QList<ModeIndex> keys(m_modeMap.keys(graph));
   if (keys.size() != 1) {
      QLOG_WARN() << "Unexpected number of spectrum indices returned";
      return;
   }

   int mode(keys.first().second);
   m_vibronic.playMode(mode);
}


void Vibronic::setSelectionRectMode(QMouseEvent* e)
{
   if (e->modifiers() == Qt::ShiftModifier) {
      m_plotCanvas->setSelectionRectMode(QCP::srmZoom);
   }else {
      m_plotCanvas->setSelectionRectMode(QCP::srmNone);
   }
}


void Vibronic::closeEvent(QCloseEvent* e)
{
   m_vibronic.playMode(-1);
   Base::closeEvent(e);
}

} } // end namespace IQmol::Configurator
