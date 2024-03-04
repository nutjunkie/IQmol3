/*******************************************************************************

  Copyright (C) 2024 Andrew Gilbert

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

#include "StatusWidget.h"

#include <QLabel>
#include <QBoxLayout>

namespace IQmol {
namespace Util{

StatusWidget::StatusWidget(QWidget* parent) : QWidget(parent)
{
   QPalette palette;
   palette.setColor(QPalette::Window, Qt::white);

   setAutoFillBackground(true);
   setPalette(palette);

   m_layout  = new QBoxLayout(QBoxLayout::LeftToRight, this);
   m_spinner = new WaitingSpinner(this);
   m_label   = new QLabel();

   m_spinner->setRoundness(70.0);
   m_spinner->setMinimumTrailOpacity(15.0);
   m_spinner->setTrailFadePercentage(70.0);
   m_spinner->setNumberOfLines(12);
   m_spinner->setLineLength(5);
   m_spinner->setLineWidth(2);
   m_spinner->setInnerRadius(3);
   m_spinner->setRevolutionsPerSecond(1);

   m_layout->addWidget(m_spinner);
   m_layout->addWidget(m_label);
   this->setLayout(m_layout);
}


StatusWidget::~StatusWidget()
{
   delete m_spinner;
   delete m_layout;
   delete m_label;
}

void StatusWidget::startSpinner() 
{ 
   m_spinner->show(); 
   m_spinner->start(); 
}



void StatusWidget::showMessage(QString const& msg, bool spin)
{
   if (spin) startSpinner();
   m_label->setText(msg);
}


void StatusWidget::clearMessage()
{
   stopSpinner();
   m_label->setText("");
}

} } // end namespace IQmol::Util
