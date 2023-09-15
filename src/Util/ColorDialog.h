#pragma once
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

#include "ui_ColorDialog.h"
#include "Color.h"
#include "ColorM.h"


namespace IQmol {
namespace Color {

   class StopButton;

   class Dialog : public QDialog {

      Q_OBJECT

      public:
         Dialog(List const&, bool blend, QWidget* parent = 0);
         Dialog(Gradient const grad = Gradient::Default, 
           bool blend = true, QWidget* parent = 0);
         List colors() const;
         bool blend() const;

      private Q_SLOTS:
         void on_gradientCombo_currentIndexChanged(int);
         void on_stopsSpin_valueChanged(int);
         void on_hueSlider_valueChanged(int);
         void on_chromaSlider_valueChanged(int);
         void on_lightnessSlider_valueChanged(int);
         void on_alphaSlider_valueChanged(int);
         void on_blendButton_toggled(bool) { updateGradient(); }

         void updateGradient(int const index, QColor const&);
         void updateGradient();
         void setActiveButton(bool);

      private:
         void init();
         void accept();
         void setCustom();
         void clearStops();
         void updateStops();
         void updateStopColors();
         void updateActiveButton();
         unsigned countStops();

         QList<StopButton*> getButtons();

         StopButton* m_activeStopButton;
         Ui::ColorDialog m_dialog;
         List m_colors;
   };


   class StopButton : public QToolButton {

      Q_OBJECT

      public:
         StopButton(QWidget* parent, int const index, QColor const& color);
         int index() const { return m_index; }

         void setColor(QColor const&);
         void setColor(ColorM::Lch const&);

         QColor const& color() const { return m_color; }
         ColorM::Lch const& lch() const { return m_lch; }

      Q_SIGNALS:
         void colorChanged(int const index, QColor const&);

      private Q_SLOTS:
         void getColor();

      private:
         void contextMenuEvent(QContextMenuEvent*);
         void mouseDoubleClickEvent(QMouseEvent*);

         static QColor toQColor(ColorM::Lch const&);
         static ColorM::Lch toLch(QColor const&); 


         ColorM::Lch m_lch;
         QColor m_color;
         int    m_index;
   };

   List GetGradient(List const& colors, bool& blend, QWidget* parent);

   List GetGradient(Gradient, bool& blend, QWidget* parent);


} } // end namespace IQmol::ColorGradient
