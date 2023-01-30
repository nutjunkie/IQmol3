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

#include "ExcitedStatesConfigurator.h"
#include "Layer/ExcitedStatesLayer.h"
#include "ElectronicTransition.h"
#include "ExcitedStates.h"
#include "CustomPlot.h"

#include "Util/Constants.h"
#include "Util/NumericalTableItem.h"
#include <cmath>


namespace IQmol {
namespace Configurator {

ExcitedStates::ExcitedStates(Layer::ExcitedStates& excitedStates) : 
  m_excitedStates(excitedStates), m_moPlot(0), m_spectrum(0), m_units(Constants::ElectronVolt)
{
   m_configurator.setupUi(this);

   QString label(m_excitedStates.stateData().typeLabel());
   label += " Excited States";
   setWindowTitle(label);

   m_configurator.widthSlider->setEnabled(false);
   m_configurator.widthLabel->setEnabled(false);

   m_pen.setColor(Qt::blue);
   m_pen.setStyle(Qt::SolidLine);
   m_pen.setWidth(1);

   m_selectedPen.setColor(Qt::red);
   m_selectedPen.setStyle(Qt::SolidLine);
   m_selectedPen.setWidth(3);

   initSpectrum();
   initTable();
   initMoPlot();

   m_configurator.unitsCombo->clear();
   for (const auto unit : Constants::AllUnits) {
       m_configurator.unitsCombo->addItem(Constants::toString(unit), unit);
   }

   m_configurator.unitsCombo->setCurrentIndex(Constants::ElectronVolt);
}


ExcitedStates::~ExcitedStates()
{
   clearTransitionLines();
   if (m_moPlot) delete m_moPlot;
   if (m_spectrum) delete m_spectrum;
}


void ExcitedStates::initSpectrum()
{
   m_spectrum = new CustomPlot(); 
   m_spectrum->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
   m_spectrum->setMultiSelectModifier(Qt::ShiftModifier);
   m_spectrum->setSelectionRectMode(QCP::srmZoom);
   m_spectrum->axisRect()->setRangeDrag(Qt::Horizontal);
   m_spectrum->axisRect()->setRangeZoom(Qt::Horizontal);

   m_spectrum->xAxis->setSelectableParts(QCPAxis::spNone);

   connect(m_spectrum, SIGNAL(mousePress(QMouseEvent*)),
      this, SLOT(setSelectionRectMode(QMouseEvent*)));

   QFrame* frame(m_configurator.spectrumFrame);
   QVBoxLayout* layout(new QVBoxLayout());
   frame->setLayout(layout);
   layout->addWidget(m_spectrum);
}


void ExcitedStates::initMoPlot()
{
   m_moPlot = new CustomPlot(); 
   m_moPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
   m_moPlot->axisRect()->setRangeDrag(Qt::Vertical);
   m_moPlot->axisRect()->setRangeZoom(Qt::Vertical);
   m_moPlot->xAxis->setSelectableParts(QCPAxis::spNone);
   
   QFrame* frame(m_configurator.moFrame);
   QVBoxLayout* layout(new QVBoxLayout());
   frame->setLayout(layout);
   layout->addWidget(m_moPlot);

   m_moPlot->yAxis->setLabel("Energy / Hartree");
   QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
   m_moPlot->xAxis->setTicker(textTicker);
   m_moPlot->xAxis->setRange(0,3.25);
   textTicker->addTick(0.75, "Alpha");
   textTicker->addTick(2.5, "Beta");

   Data::OrbitalSymmetries const& orbitals(m_excitedStates.stateData().orbitalSymmetries());

   unsigned nOrbs(orbitals.nOrbitals());
   unsigned nAlpha(orbitals.nAlpha());
   unsigned nBeta(orbitals.nBeta());
   QVector<double>  xAlpha(nOrbs), yAlpha(nOrbs), xBeta(nOrbs), yBeta(nOrbs); 
   QVector<double> a(1), b(1), y(1);
   QCPGraph* graph(0);


   Data::Spin spin(Data::Alpha);
   unsigned nElectrons(nAlpha);
   unsigned i(0), g(0);

   while (i < nOrbs) {
       y[0] = orbitals.energy(spin, i);

       g = 1; // degeneracy
       while (i+g < nOrbs && 
              std::abs(y[0]-orbitals.energy(spin, i+g)) < 0.001) { ++g; }

       for (unsigned k = i; k < i+g; ++k) {
           a[0]  = 0.75 - 0.25*(g-1) + (k-i)*0.50;
           graph = m_moPlot->addGraph();
           graph->setData(a, y);
           graph->setName(QString::number(k));
           graph->selectionDecorator()->setPen(m_selectedPen);
           graph->setScatterStyle(k < nElectrons ? QCPScatterStyle::ssOccupied 
                                                 : QCPScatterStyle::ssVirtual);
           connect(graph, SIGNAL(selectionChanged(bool)), this, SLOT(moSelectionChanged(bool)));
       }

       i += g;
   }

   i    = 0;
   spin = Data::Beta;
   nElectrons = nBeta;

   while (i < nOrbs) {
       y[0] = orbitals.energy(spin, i);

       g = 1; // degeneracy
       while (i+g < nOrbs && 
              std::abs(y[0]-orbitals.energy(spin, i+g)) < 0.001) { ++g; }

       for (unsigned k = i; k < i+g; ++k) {
           a[0]  = 2.50 - 0.25*(g-1) + (k-i)*0.50;
           graph = m_moPlot->addGraph();
           graph->setData(a, y);
           graph->setName(QString::number(k+nOrbs));
           graph->selectionDecorator()->setPen(m_selectedPen);
           graph->setScatterStyle(k < nElectrons ? QCPScatterStyle::ssOccupied 
                                                 : QCPScatterStyle::ssVirtual);
           connect(graph, SIGNAL(selectionChanged(bool)), this, SLOT(moSelectionChanged(bool)));
       }

       i += g;
   }

   // Set the scale to show 5 occupied and virtual orbitals to start with
   int nShow(5);

   // Use the beta energies if there are more of them
   if (nBeta > nAlpha) {
      spin = Data::Beta;
      nElectrons = nBeta;
   }else {
      spin = Data::Alpha;
      nElectrons = nAlpha;
   }

   unsigned index(std::max(0, (int)nElectrons-nShow));
   double   yMin(orbitals.energy(spin, index));
   index =  std::min((int)nElectrons+nShow, (int)nOrbs);
   double   yMax(orbitals.energy(spin, index));
   
   // enlarge the range by 5% for asthetics
   double dy(0.05*std::abs(yMax-yMin));
   m_moPlot->yAxis->setRange(yMin-dy,yMax+dy);
}



void ExcitedStates::initTable()
{
   Data::ElectronicTransitionList const& transitions(m_excitedStates.stateData().transitions());

   QTableWidget* table(m_configurator.energyTable);
   table->setSortingEnabled(true);
   table->setRowCount(transitions.size());

   QHeaderView* header(table->horizontalHeader());
   header->setSectionResizeMode(QHeaderView::Stretch);
   header->setSortIndicatorShown(true);

   QTableWidgetItem* item;

   m_maxValues.first  = 0;
   m_maxValues.second = 0;

   int row(0);
   Data::ElectronicTransitionList::const_iterator iter;
   for (iter = transitions.begin(); iter != transitions.end(); ++iter) {
       double energy((*iter)->energy());
       double strength((*iter)->strength());
       double spinSquared((*iter)->spinSquared());

       m_maxValues.first  = std::max(m_maxValues.first,  energy);
       m_maxValues.second = std::max(m_maxValues.second, strength);

       QString text(QString::number(energy, 'f', 3));
       item = new DoubleTableWidgetItem();
       item->setText( text + "    ");
       item->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
       item->setData(Qt::UserRole, row);
       table->setItem(row, 0, item);

       text = QString::number(strength, 'f', 3);
       item = new DoubleTableWidgetItem();
       item->setText( text + "    ");
       item->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
       item->setData(Qt::UserRole, row);
       table->setItem(row, 1, item);

       text = QString::number(spinSquared, 'f', 3);
       item = new DoubleTableWidgetItem();
       item->setText( text + "    ");
       item->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
       item->setData(Qt::UserRole, row);
       table->setItem(row, 2, item);

       ++row;
   }
}


void ExcitedStates::updateMoPlot(int const index)
{
   clearTransitionLines();
   QList<Data::Amplitude>  amplitudes(m_excitedStates.stateData().amplitudes(index));

   QPen pen;
   QColor color;
   pen.setStyle(Qt::SolidLine);
   pen.setWidth(3);

   double dx(-0.2);
   QList<Data::Amplitude>::const_iterator iter;
   for (iter = amplitudes.begin(); iter != amplitudes.end(); ++iter) {

       color.setHsv(280, 255*std::abs((*iter).m_amplitude), 255);
       pen.setColor(color);

       QCPItemLine* line(new QCPItemLine(m_moPlot));
       line->setSelectable(true);
       m_transitionLines.append(line);

       double x( ((*iter).m_spin == Data::Alpha) ? 0.75 : 2.5 );
       line->position("start")->setCoords(x+dx, (*iter).m_ei);
       line->position("end"  )->setCoords(x+dx, (*iter).m_ea);
       dx += 0.1;

       line->setPen(pen);
       // decorator?
       line->setSelectedPen(m_selectedPen);
       line->setHead(QCPLineEnding::esSpikeArrow);
   }

   m_moPlot->replot();
}


void ExcitedStates::updateTable(Constants::Units const units)
{
   QString label("Energy / ");
   label += Constants::toString(units);

   QTableWidget* table(m_configurator.energyTable); 
   table->horizontalHeaderItem(0)->setText(label);

   int row(0);
   Data::ElectronicTransitionList const& transitions(m_excitedStates.stateData().transitions());
   for (auto iter(transitions.begin()); iter != transitions.end(); ++iter, ++row) {
       double energy(Constants::convert((*iter)->energy(), Constants::ElectronVolt, units));
       double strength((*iter)->strength());
       QString text;

       switch (units) {
          case Constants::KJoulePerMol:
          case Constants::KCalPerMol:
          case Constants::Wavenumber:
             text = QString::number(energy, 'f', 0);
             break;
          case Constants::MegaHertz:
             text = QString::number(energy, 'g', 4);
             break;
          default:
             text = QString::number(energy, 'f', 3);
             break;
       }

       table->item(row, 0)->setText(text + "     ");
   }
}


void ExcitedStates::updateSpectrum(Constants::Units const units)
{
   QString label("Energy / ");
   label += Constants::toString(units);
   m_spectrum->xAxis->setLabel(label);
   m_spectrum->clearGraphs();

   double width(m_configurator.widthSlider->value());

   if (m_configurator.impulseButton->isChecked()) {
      plotImpulse();
      m_spectrum->yAxis->setLabel("Strength");

   }else if (m_configurator.gaussianButton->isChecked()) {
      plotSpectrum(Gaussian, width);
      m_spectrum->yAxis->setLabel("Rel. Strength");

   }else if (m_configurator.lorentzianButton->isChecked()) {
      plotSpectrum(Lorentzian, width);
      m_spectrum->yAxis->setLabel("Rel. Strength");
   }

   m_spectrum->replot();
}


void ExcitedStates::plotImpulse()
{
   QVector<double> x(1), y(1);
   double xmin(0), xmax(0);

   int i(0);
   Data::ElectronicTransitionList const& 
      transitions(m_excitedStates.stateData().transitions());

   for (auto iter = transitions.begin(); iter != transitions.end(); ++iter, ++i) {
       x[0] = Constants::convert((*iter)->energy(), Constants::ElectronVolt, m_units);
       y[0] = (*iter)->strength();
       xmin = std::min(xmin, x[0]);
       xmax = std::max(xmax, x[0]);

       QCPGraph* graph(m_spectrum->addGraph());
       graph->setData(x, y);
       graph->setName(QString::number(i));
       graph->setLineStyle(QCPGraph::lsImpulse);
       graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle));
       graph->setPen(m_pen);
       graph->selectionDecorator()->setPen(m_selectedPen);
       connect(graph, SIGNAL(selectionChanged(bool)), this, SLOT(plotSelectionChanged(bool)));
   }

   // The extra range is for the tails of the non-impulse spectra
   m_spectrum->xAxis->setRange(0.9*xmin, 1.1*xmax);
   m_spectrum->yAxis->setRange(0, 1.05*m_maxValues.second);
}


void ExcitedStates::plotSpectrum(Profile const profile, double const width)
{
   unsigned const bins(200);
   // The extra range is for the tails of the non-impulse spectra
   double const maxEnergy(m_maxValues.first+1);
   double const delta(maxEnergy/bins);

   QVector<double> x(bins), y(bins);
   for (unsigned xi = 0; xi < bins; ++xi) {
       x[xi] = xi*delta;
       y[xi] = 0.0;
   }

   Data::ElectronicTransitionList const& 
      transitions(m_excitedStates.stateData().transitions());

   switch (profile) {
      // These parameters only work for eV.
      case Gaussian: {
         double g(0.02*width);
         double A(std::sqrt(4.0*std::log(2.0)/(g*M_PI)));
         double a(-4.0*std::log(2.0)/g);

         for (auto iter = transitions.begin(); iter != transitions.end(); ++iter) {
             double nu((*iter)->energy()); 
             double I((*iter)->strength());
             for (unsigned xi = 0; xi < bins; ++xi) {
                 y[xi] += I*A*std::exp(a*(x[xi]-nu)*(x[xi]-nu));
             }
         }
      } break;

      case Lorentzian: {
         double A(2.0/M_PI);
         double g(0.005*width);
         double g2(g*g);

         for (auto iter = transitions.begin(); iter != transitions.end(); ++iter) {
             double nu((*iter)->energy()); 
             double I((*iter)->strength());
             for (unsigned xi = 0; xi < bins; ++xi) {
                 y[xi] += I*A*g / (g2+(x[xi]-nu)*(x[xi]-nu));
             }
         }
      } break;

   }

   double maxIntensity(0.0);
   for (unsigned xi = 0; xi < bins; ++xi) {
       if (y[xi] > maxIntensity) maxIntensity = y[xi];
   }

   for (unsigned xi = 0; xi < bins; ++xi) {
       x[xi] = Constants::convert(x[xi], Constants::ElectronVolt, m_units);
       y[xi] /= maxIntensity;
   }

   QCPGraph* graph(m_spectrum->addGraph());
   graph->setData(x, y);
   graph->setPen(m_pen);
   graph->setAntialiased(true);
   graph->selectionDecorator()->setPen(m_selectedPen);

   double xmin = std::min(x.first(), x.last());
   double xmax = std::max(x.first(), x.last());
   m_spectrum->xAxis->setRange(0.9*xmin, 1.1*xmax);
   m_spectrum->yAxis->setRange(0, 1.04);
}


void ExcitedStates::plotSelectionChanged(bool tf)
{      
   QCPGraph* graph(qobject_cast<QCPGraph*>(sender()));
   if (!graph) return;
       
   if (tf) {
      graph->setPen(m_selectedPen);
      graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc));
   }else {
      graph->setPen(m_pen);
      graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle));
      return;
   }

   if (!tf) return;
   if (!m_configurator.impulseButton->isChecked()) return;

   bool ok;
   int row(graph->name().toInt(&ok));
   if (!ok) return;

   QTableWidget* table(m_configurator.energyTable);
   table->setCurrentCell(row, 0, 
      QItemSelectionModel::Rows | QItemSelectionModel::ClearAndSelect);
   table->scrollToItem(table->item(row,0));
   updateMoPlot(row);
}


void ExcitedStates::on_energyTable_itemSelectionChanged()
{      
   QList<QTableWidgetItem*> selection = m_configurator.energyTable->selectedItems();
   if (selection.isEmpty()) return;

   int index(selection.first()->row());

   updateMoPlot(index);
   if (!m_configurator.impulseButton->isChecked()) return;

   int nGraphs(m_spectrum->graphCount());

   if (index < 0 || index >= nGraphs) {
      qDebug() << "Unmatched graph requested" << index;
      return;
   }

   for (int i = 0; i < nGraphs; ++i) {
       QCPGraph* graph(m_spectrum->graph(i));

       if (i == index) {
          QCPDataSelection selection(QCPDataRange(0,graph->dataCount()));
          m_spectrum->graph(i)->setSelection(selection);
       }else {
          graph->setSelection(QCPDataSelection());  // Empty selection
       }
   }

   m_spectrum->replot();
}


void ExcitedStates::on_resetZoomButton_clicked(bool)
{
   updateSpectrum(m_units);
}


void ExcitedStates::on_unitsCombo_currentIndexChanged(int)
{
   int u(m_configurator.unitsCombo->currentData().toInt());
   m_units = static_cast<Constants::Units>(u);
   updateTable(m_units); 
   updateSpectrum(m_units); 
}


void ExcitedStates::on_impulseButton_clicked(bool)
{
   m_configurator.widthSlider->setEnabled(false);
   m_configurator.widthLabel->setEnabled(false);
   updateSpectrum(m_units);
}


void ExcitedStates::on_gaussianButton_clicked(bool)
{
   m_configurator.widthSlider->setEnabled(true);
   m_configurator.widthLabel->setEnabled(true);
   updateSpectrum(m_units);
}


void ExcitedStates::on_lorentzianButton_clicked(bool)
{
   m_configurator.widthSlider->setEnabled(true);
   m_configurator.widthLabel->setEnabled(true);
   updateSpectrum(m_units);
}


void ExcitedStates::on_widthSlider_valueChanged(int)
{
   updateSpectrum(m_units);
}



void ExcitedStates::clearTransitionLines()
{
   QList<QCPAbstractItem*>::iterator iter;
   for (iter = m_transitionLines.begin(); iter != m_transitionLines.end(); ++iter) {
       m_moPlot->removeItem(*iter);   // item gets automatically deleted
   }

   m_transitionLines.clear();
}



void ExcitedStates::moSelectionChanged(bool tf)
{
   QCPGraph* graph(qobject_cast<QCPGraph*>(sender()));

   if (!tf || !graph) {
      m_configurator.orbitalLabel->setText("");
      return;
   }
   
   bool ok(true);;
   unsigned orb(graph->name().toUInt(&ok));
   if (!ok) return;

   Data::OrbitalSymmetries const& orbitals(m_excitedStates.stateData().orbitalSymmetries());
   unsigned nOrbs(orbitals.nOrbitals());
   Data::Spin spin(Data::Alpha);

   if (orb >= nOrbs) {
      orb -= nOrbs; 
      spin = Data::Beta;
   }

   double energy(orbitals.energy(spin, orb));
   QString symmetry(orbitals.symmetry(Data::Alpha, orb));
   QString label(Data::SpinLabel(spin));

   label += " " + QString::number(orb+1);

   label += " orbital energy: ";
   label += QString::number(energy, 'f', 3);
   label += " Eh";
   if (!symmetry.isEmpty()) label += "  Irrep " + symmetry;

   m_configurator.orbitalLabel->setText(label);
}


void ExcitedStates::setSelectionRectMode(QMouseEvent* e)
{
   if (e->modifiers() == Qt::ShiftModifier) {
      m_spectrum->setSelectionRectMode(QCP::srmZoom);
   }else {
      m_spectrum->setSelectionRectMode(QCP::srmNone);
   }   
}

} } // end namespace IQmol::Configurator
