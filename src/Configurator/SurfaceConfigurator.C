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

#include "QsLog.h"
#include "SurfaceConfigurator.h"
#include "Layer/SurfaceLayer.h"
#include "Layer/MoleculeLayer.h"
#include "Layer/ComponentLayer.h"

#include "Util/ColorDialog.h"
#include "Util/Color.h"

#include <openbabel/elements.h> 
#include <QColorDialog>


using namespace qglviewer;

namespace IQmol {
namespace Configurator {

Surface::Surface(Layer::Surface& surface) : m_surface(surface), 
   m_gradientColors(Color::toList(Color::Gradient::Default)), m_initialized(false)
{        
   m_ui.setupUi(this);
   m_ui.ambientOcclusionCheckBox->setVisible(false);
   m_ui.centerButton->setEnabled(false);

   m_ui.minValue->setVisible(false);
   m_ui.maxValue->setVisible(false);

   m_ui.swapColorsButton->setEnabled(m_surface.isSigned());
   m_ui.negativeColorButton->setVisible(m_surface.isSigned());
   m_ui.negativeLabel->setVisible(m_surface.isSigned());
}


// Should only be called when the surface is constructed
void Surface::init()
{
   if (m_initialized) return;
   m_initialized = true;

   setPositiveColor(m_surface.colorPositive());
   setNegativeColor(m_surface.colorNegative());

   double area(m_surface.area());
   m_ui.areaLabel->setText(QString::number(area, 'f', 3));

   m_ui.transparencySlider->setValue(100*m_surface.getAlpha());
   
   switch (m_surface.m_drawMode) {
      case Layer::Surface::Fill:
           m_ui.fillButton->setChecked(true);
         break;
      case Layer::Surface::Lines:
           m_ui.linesButton->setChecked(true);
         break;
      case Layer::Surface::Dots:
           m_ui.dotsButton->setChecked(true);
         break;
   }  

   ComponentList parents(m_surface.findLayers<Layer::Component>(Layer::Parents));
   if (parents.isEmpty()) {
      QLOG_ERROR() << "Could not find Component parent for surface";
   }else {
      QStringList properties(parents.first()->getAvailableProperties2());
      if (!m_surface.isVdW())  properties.removeAll("Nuclei");
      m_ui.propertyCombo->addItems(properties);
   }
}  


void Surface::on_propertyCombo_currentIndexChanged(int)
{
   disconnect(m_ui.positiveColorButton, 0, 0, 0);
   QString type(m_ui.propertyCombo->currentText());

   if (type == "None") {

      m_ui.minValue->setVisible(false);
      m_ui.maxValue->setVisible(false);
      m_ui.centerButton->setEnabled(false);
      m_ui.swapColorsButton->setEnabled(m_surface.isSigned());
      m_ui.negativeColorButton->setVisible(m_surface.isSigned());
      m_ui.negativeLabel->setVisible(m_surface.isSigned());
      m_ui.positiveLabel->setVisible(true);

      setPositiveColor(m_surface.colorPositive());
      connect(m_ui.positiveColorButton, SIGNAL(clicked(bool)),
         this, SLOT(on_positiveColorButton_clicked(bool)));

      m_surface.clearPropertyData();

   }else if (type == "Nuclei") {

      m_ui.minValue->setVisible(false);
      m_ui.maxValue->setVisible(false);
      m_ui.centerButton->setEnabled(false);
      m_ui.negativeColorButton->setVisible(false);
      m_ui.swapColorsButton->setEnabled(false);
      m_ui.negativeLabel->setVisible(false);
      m_ui.positiveLabel->setVisible(true);

      setPositiveColor(m_surface.colorPositive());

      bool blend(false);
      m_surface.setColors(Color::atomColors(),blend);

      QList<Layer::Component*> parents = 
         m_surface.findLayers<Layer::Component>(Layer::Parents);

      if (parents.isEmpty()) {
         QLOG_ERROR() << "No parent Component found";
      }else {
         m_surface.setColors(Color::residueColors(), blend);
         m_surface.computePropertyData(parents.first()->getProperty(type));
      }

    //  m_surface.computeIndexField();
      updateScale();

   }else if (type == "Residue") {

      m_ui.minValue->setVisible(false);
      m_ui.maxValue->setVisible(false);
      m_ui.centerButton->setEnabled(false);
      m_ui.negativeColorButton->setVisible(false);
      m_ui.swapColorsButton->setEnabled(false);
      m_ui.negativeLabel->setVisible(false);
      m_ui.positiveLabel->setVisible(true);

      bool blend(false);
      setPositiveColor(Color::residueColors(), blend);

      connect(m_ui.positiveColorButton, SIGNAL(clicked(bool)),
         this, SLOT(editGradientColors(bool)));

      QList<Layer::Component*> parents = 
         m_surface.findLayers<Layer::Component>(Layer::Parents);

      if (parents.isEmpty()) {
         QLOG_ERROR() << "No parent Component found";
      }else {
         m_surface.setColors(Color::residueColors(), blend);
         m_surface.computePropertyData(parents.first()->getProperty(type));
      }

      updateScale();

   }else {

      m_ui.minValue->setVisible(true);
      m_ui.maxValue->setVisible(true);
      m_ui.centerButton->setEnabled(true);
      m_ui.swapColorsButton->setEnabled(false);
      m_ui.negativeColorButton->setVisible(false);
      m_ui.negativeLabel->setVisible(false);
      m_ui.positiveLabel->setVisible(false);

     bool blend(m_surface.blend());
     setPositiveColor(m_gradientColors, blend);

      connect(m_ui.positiveColorButton, SIGNAL(clicked(bool)),
         this, SLOT(editGradientColors(bool)));

      QList<Layer::Component*> parents = 
         m_surface.findLayers<Layer::Component>(Layer::Parents);

      if (parents.isEmpty()) {
         QLOG_ERROR() << "No parent Component found";
      }else {
         m_surface.setColors(m_gradientColors, blend);
         m_surface.computePropertyData(parents.first()->getProperty(type));
      }

      m_ui.centerButton->setEnabled(m_surface.propertyIsSigned());

      updateScale();
   }

   m_surface.updated();
}


void Surface::on_minValue_valueChanged(double)
{
   m_surface.setPropertyRange(m_ui.minValue->value(), m_ui.maxValue->value());
   m_surface.updated();
}

void Surface::on_maxValue_valueChanged(double)
{
   m_surface.setPropertyRange(m_ui.minValue->value(), m_ui.maxValue->value());
   m_surface.updated();
}

  
void Surface::on_positiveColorButton_clicked(bool)
{
   QColor color(m_surface.colorPositive());
   setPositiveColor(QColorDialog::getColor(color, this));
   m_surface.updated();
}     
      
   
void Surface::on_negativeColorButton_clicked(bool)
{
   QColor color(m_surface.colorNegative());
   setNegativeColor(QColorDialog::getColor(color, this));
   m_surface.updated();
}


void Surface::on_centerButton_clicked(bool tf)
{
   m_surface.balanceScale(tf);
   updateScale();
}


void Surface::updateScale()
{
   double min, max;
   m_surface.getPropertyRange(min, max);
   QString v1(QString::number(min, 'f', 4));
   QString v2(QString::number(max, 'f', 4));
   m_ui.minValue->setValue(min);
   m_ui.maxValue->setValue(max);
}



void Surface::on_swapColorsButton_clicked(bool)
{
   QColor positive(m_surface.colorPositive());
   QColor negative(m_surface.colorNegative());
   setPositiveColor(negative);
   setNegativeColor(positive);
   m_surface.updated();
}
   

/*
Color::List Surface::atomColors(unsigned const maxAtomicNumber)
{
   Color::List atomColors;
   QColor color;
   for (unsigned int Z = 1; Z <= maxAtomicNumber; ++Z) {
       double r, g, b;
       OpenBabel::OBElements::GetRGB(Z, &r, &g, &b);
       color.setRgbF(r, g, b, 1.0);
       atomColors.append(color);
   }

   return atomColors;
}
*/

 
void Surface::editGradientColors(bool)
{
   QList<QColor> colors(m_gradientColors);
   bool blend(m_surface.blend());
   m_gradientColors = Color::GetGradient(colors, blend, this); 
   setPositiveColor(m_gradientColors, blend);
   m_surface.recompile();
   m_surface.updated();
}


void Surface::setPositiveColor(QList<QColor> const& colors, bool blend)
{
   m_ui.positiveColorButton->setProperty("gradient",true);
   QString bg("background-color: ");
   bg += Color::toString(colors,blend);
   m_ui.positiveColorButton->setStyleSheet(bg);
   m_surface.setColors(colors, blend);
}


void Surface::setPositiveColor(QColor const& color)
{
   if (color.isValid()) {
      m_ui.positiveColorButton->setProperty("gradient",false);
      QString bg("background-color: ");
      bg += color.name();
      m_ui.positiveColorButton->setStyleSheet(bg);

      QColor negative(m_surface.colorNegative());
      m_surface.setColors(negative, color);
   }
}


void Surface::setNegativeColor(QColor const& color)
{
   if (color.isValid()) {
      QString bg("background-color: ");
      bg += color.name();
      m_ui.negativeColorButton->setStyleSheet(bg);

      QColor positive(m_surface.colorPositive());
      m_surface.setColors(color, positive);
   }
}


void Surface::setArea(double const area)
{
   m_ui.areaLabel->setText(QString::number(area, 'f', 3));
}


void Surface::on_transparencySlider_valueChanged(int value)
{
   m_surface.setAlpha(value/100.0);
   m_surface.updated();
}


void Surface::on_clipCheckBox_clicked(bool tf)
{
   m_surface.setClip(tf);
   m_surface.updated();
}


void Surface::on_fillButton_clicked(bool)
{
   m_surface.setDrawMode(Layer::Surface::Fill);
   m_surface.updated();
}


void Surface::on_linesButton_clicked(bool)
{
   m_surface.setDrawMode(Layer::Surface::Lines);
   m_surface.updated();
}


void Surface::on_dotsButton_clicked(bool)
{
   m_surface.setDrawMode(Layer::Surface::Dots);
   m_surface.updated();
}

} } // end namespace IQmol::Configurator
