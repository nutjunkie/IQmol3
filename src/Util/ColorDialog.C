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

#include "Color.h"
#include "ColorDialog.h"
#include "Preferences.h"
#include "Math/Numerical.h"

#include <QColorDialog>
#include <QMenu>
#include <QDebug>


void maxRange()
{
     double lmin(999.99), lmax(-999.99); 
     double cmin(999.99), cmax(-999.99); 
     double hmin(999.99), hmax(-999.99); 
  
    ColorM::Lch lch;
     
     for (int r = 0; r < 256; ++r) {
         for (int g = 0; g < 256; ++g) {
             for (int b = 0; b < 256; ++b) {
                 ColorM::Rgb rgb(r,g,b);
                 ColorM::Lch lch(rgb);
                 lch = lch.clip();

                 lmin = std::min(lmin, lch.lightness());
                 lmax = std::max(lmax, lch.lightness());

                 cmin = std::min(cmin, lch.chroma());
                 cmax = std::max(cmax, lch.chroma());

                 hmin = std::min(hmin, lch.hue());
                 hmax = std::max(hmax, lch.hue());
             }
         }   
     }

    qDebug() << "L range: " << lmin <<" - " << lmax;
    qDebug() << "C range: " << cmin <<" - " << cmax;
    qDebug() << "H range: " << hmin <<" - " << hmax;
}


namespace IQmol {
namespace Color {


Dialog::Dialog(List const& colors, bool blend, QWidget* parent) 
 : QDialog(parent),  m_colors(colors)
{
   for (auto& c : m_colors) {
       qDebug() << "Dialog color: " << c;
   }
   init();
   m_dialog.blendButton->setChecked(blend);
}


Dialog::Dialog(Gradient const gradient, bool blend, QWidget* parent) 
 : QDialog(parent), m_colors(toList(gradient))
{
   init();
   m_dialog.blendButton->setChecked(blend);
}


List Dialog::colors() const
{
   for (auto& color : m_colors) {
       qDebug() << "Returning color" << color << color.name(QColor::HexArgb);
   }
   return m_colors; 
}


bool Dialog::blend() const
{
   return m_dialog.blendButton->isChecked();
}


void Dialog::init()
{
   m_activeStopButton = 0;
   m_dialog.setupUi(this);
   m_dialog.stopsSpin->setValue(m_colors.size());

   // Fix global button background to default
   QString styleSheet =
      "QToolButton "
      "{"
      "   background-color: #eee;"
      "   min-width: 24px;"
      "   max-width: 24px;"
      "   min-height: 24px;"
      "   max-height: 24px;"
      "   margin: 2px;"
      "}"
      "QToolButton:checked {"
      "   border-width: 1px;"
      "   margin: 2px;"
      "}";

   m_dialog.globalButton->setStyleSheet(styleSheet);
   m_dialog.globalButton->hide();

   updateStopColors();
   //::maxRange();

   int index(0);
   if (m_colors == toList(Gradient::Default)) {
      index = toInt(Gradient::Default);
   }else if (m_colors == toList(Gradient::Spectrum)) {
      index = toInt(Gradient::Spectrum);
   }else {
      index = toInt(Gradient::Custom);
   }

   m_dialog.gradientCombo->blockSignals(true);
   m_dialog.gradientCombo->setCurrentIndex(index);
   m_dialog.gradientCombo->blockSignals(false);
}


void Dialog::updateStops()
{
//   unsigned nStops(countStops());
   unsigned nColors(m_colors.size());

   //if (nColors == nStops) return;

   clearStops();

   QHBoxLayout* layout(m_dialog.buttonLayout);
   QSpacerItem* spacer;
   StopButton*  button;


   for (int i = 0; i < nColors; ++i) {

       button = new StopButton(this, i, m_colors[i]);
       button->setCheckable(true);
       button->setAutoExclusive(true);

       connect(button, SIGNAL(toggled(bool)), this, SLOT(setActiveButton(bool)));
       connect(button, SIGNAL(colorChanged(int const, QColor const&)), 
          this, SLOT(updateGradient(int const, QColor const&)));

       layout->addWidget(button);

       if (i == nColors-1) continue;
       spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
       layout->addItem(spacer);
   }
}


void Dialog::updateStopColors()
{
   unsigned nColors(m_colors.size());

   if (nColors == countStops()) {
      QList<StopButton*> buttons(getButtons());
      assert(buttons.size() == nColors);

      for (unsigned i(0); i < nColors; ++i) {
          buttons[i]->setColor(m_colors[i]);
      }
   }else {
      updateStops();
   }

   updateGradient();
}


QList<StopButton*> Dialog::getButtons()
{
   QFrame* frame(m_dialog.frame);
   QList<StopButton*> buttons(frame->findChildren<StopButton*>());
   return buttons;
}


unsigned Dialog::countStops()
{
   return getButtons().size();
}


void Dialog::clearStops()
{
   QHBoxLayout* layout(m_dialog.buttonLayout);
   QLayoutItem* item;

   while ( (item = layout->takeAt(0)) != nullptr) {
      if (item->widget()) delete item->widget();
      delete item;
   }

   m_activeStopButton = 0;
}


void Dialog::on_gradientCombo_currentIndexChanged(int n)
{
   Gradient grad = static_cast<Gradient>(n);
   m_colors = toList(grad);
   updateStopColors();
}


void Dialog::on_stopsSpin_valueChanged(int n)
{
   if (n == m_colors.size()) return;

   QColor first(m_colors.first());
   QColor last(m_colors.last());

   ColorM::Rgb col0(first.red(),first.green(),first.blue());
   col0.setAlpha8(first.alpha());
   ColorM::Rgb col1(last.red(),last.green(),last.blue());
   col1.setAlpha8(last.alpha());

   ColorM::ColorScale<ColorM::Rgb> scale({col0,col1}); 
   std::vector<ColorM::Rgb> colors(scale.colors(n));

   m_colors.clear();
   for (auto& color : colors) {
       m_colors.push_back(QColor(color.red(), color.green(),color.blue(),color.alpha8()));
   } 
   
   updateStopColors();
   setCustom();
}


void Dialog::on_hueSlider_valueChanged(int value)
{
   m_dialog.hueLabel->setText(QString::number(value));
   updateActiveButton();
}

void Dialog::on_chromaSlider_valueChanged(int value)
{
   m_dialog.chromaLabel->setText(QString::number(value));
   updateActiveButton();
}

void Dialog::on_lightnessSlider_valueChanged(int value)
{
   m_dialog.lightnessLabel->setText(QString::number(value));
   updateActiveButton();
}


void Dialog::on_alphaSlider_valueChanged(int value)
{
   double tmp(value/100.0);
   m_dialog.alphaLabel->setText(QString("%1").arg(tmp,3,'f',2,0));
   updateActiveButton();
}





void Dialog::updateActiveButton()
{
   if (m_activeStopButton == nullptr) return;

   int hue(m_dialog.hueSlider->value());
   int chroma(m_dialog.chromaSlider->value());
   int lightness(m_dialog.lightnessSlider->value());
   int alpha(m_dialog.alphaSlider->value());

   ColorM::Lch lch(lightness,chroma,hue,alpha/100.0);

   m_activeStopButton->setColor(lch);
}


void Dialog::setActiveButton(bool active)
{
   if (!active) return;
   StopButton* stopButton = qobject_cast<StopButton*>(sender());
   if (stopButton == nullptr) return;

   m_activeStopButton = stopButton;

   ColorM::Lch lch(m_activeStopButton->lch());

   int L(Math::round(lch.lightness()));
   int C(Math::round(lch.chroma()));
   int H(Math::round(lch.hue()));
   double A(lch.alpha());

   // These do not get update properly, see
   // https://github.com/nodegui/nodegui/issues/921

   m_dialog.lightnessSlider->blockSignals(true);
   m_dialog.lightnessSlider->setValue(L);
   m_dialog.lightnessLabel->setText(QString::number(L));
   m_dialog.lightnessSlider->blockSignals(false);

   m_dialog.chromaSlider->blockSignals(true);
   m_dialog.chromaSlider->setValue(C);
   m_dialog.chromaLabel->setText(QString::number(C));
   m_dialog.chromaSlider->blockSignals(false);

   m_dialog.hueSlider->blockSignals(true);
   m_dialog.hueSlider->setValue(H);
   m_dialog.hueLabel->setText(QString::number(H));
   m_dialog.hueSlider->blockSignals(false);

   m_dialog.alphaSlider->blockSignals(true);
   m_dialog.alphaSlider->setValue(A*100);
   m_dialog.alphaLabel->setText(QString("%1").arg(A,3,'f',2,0));
   m_dialog.alphaSlider->blockSignals(false);
}


void Dialog::updateGradient()
{
   bool blend(m_dialog.blendButton->isChecked());
   QString grad("background-color: " + toString(m_colors, blend));
   m_dialog.gradientView->setStyleSheet(grad);
}


void Dialog::updateGradient(int const index, QColor const& color)
{
   if (0 <= index && index < m_colors.size()) {
      m_colors[index] = color;
      updateGradient();
   }
   setCustom();
}


void Dialog::setCustom()
{
   m_dialog.gradientCombo->blockSignals(true);
   m_dialog.gradientCombo->setCurrentIndex(toInt(Gradient::Custom));
   m_dialog.gradientCombo->blockSignals(false);
}


void Dialog::accept()
{
   Gradient type(fromInt(m_dialog.gradientCombo->currentIndex()));
   if (type  == Gradient::Custom) {
       for (auto& color : m_colors) {
           qDebug() << color;
       }
       Preferences::CustomGradientColors(m_colors);
   }
   QDialog::accept();
}


// --------------- StopButton ----------------

QColor StopButton::toQColor(ColorM::Lch const& lch)
{
   ColorM::Rgb rgb(lch);
   rgb = rgb.clip();
   return QColor(Math::round(rgb.red()),  Math::round(rgb.green()), 
                 Math::round(rgb.blue()), Math::round(rgb.alpha8())); 
}


ColorM::Lch StopButton::toLch(QColor const& color)
{
   ColorM::Rgb rgb(color.red(), color.green(), color.blue());
   rgb.setAlpha8(color.alpha());
   ColorM::Lch lch(rgb);
   return lch;
}


StopButton::StopButton(QWidget* parent, int const index, QColor const& color) 
   : QToolButton(parent), m_index(index)
{
   setColor(color);
   m_lch = toLch(m_color);
}


void StopButton::getColor()
{
   QColorDialog colorDialog;
   QColor color = QColorDialog::getColor(m_color, this);
   qDebug() << "Color returned" << color << color.isValid();
   if (color.isValid()) {
      m_lch = toLch(color);
      setColor(color);
   }
   toggled(true);   
}


void StopButton::contextMenuEvent(QContextMenuEvent*)
{
   QMenu menu;
   QAction* action(new QAction("Select color", this));
   connect(action, SIGNAL(triggered()), this, SLOT(getColor()));
   menu.addAction(action);
   menu.exec(QCursor::pos());
   delete action; 
}


void StopButton::mouseDoubleClickEvent(QMouseEvent*)
{
   getColor();
}



void StopButton::setColor(ColorM::Lch const& lch)
{
   m_lch = lch;
   setColor(toQColor(lch));
}


void StopButton::setColor(QColor const& color) 
{
   m_color = color;

   QString styleSheet = 
      "QToolButton {" 
      "   border-width: 1px;"
      "   border-color: #333;"
      "   border-style: solid;"
      "   border-radius: 4;"
      "   padding: 0px;"
      "   min-width: 20px;"
      "   max-width: 20px;"
      "   min-height: 20px;"
      "   max-height: 20px;"
      "   margin: 6px;"
      "   background-color: ";

   styleSheet += color.name(QColor::HexArgb);

   styleSheet += 
      ";}"
      "QToolButton:checked {"
      "   border-width: 3px;"
      "   margin: 0px;"
      "   min-width: 20px;"
      "}";

   setStyleSheet(styleSheet);
   colorChanged(m_index,m_color);
}


// - - - - - - - - - - - - - - - - - -

List GetGradient(Color::Gradient gradient, bool& blend, QWidget* parent) 
{
   Dialog dialog(gradient, blend, parent);
   dialog.exec();
   blend = dialog.blend();
   return dialog.colors();
}

List GetGradient(List const& colors, bool& blend, QWidget* parent) 
{
   Dialog dialog(colors, blend, parent);
   dialog.exec();
   blend = dialog.blend();
   return dialog.colors();
}

} } // end namespace IQmol::Color
