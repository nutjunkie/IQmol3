#pragma once
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

#include "WaitingSpinner.h"

class QLabel;
class QBoxLayout;

namespace IQmol {
namespace Util {

   class StatusWidget : public QWidget 
   {
      Q_OBJECT

      public:
         StatusWidget(QWidget* parent = 0);

         ~StatusWidget();

         void clear(QString const& msg, bool startSpinner = false);
         void startSpinner();

         void stopSpinner()  { m_spinner->hide(); }

      public Q_SLOTS:
         void showMessage(QString const& msg, bool spin = false);
         void clearMessage();
     
      private:
         WaitingSpinner* m_spinner;
         QBoxLayout* m_layout;
         QLabel* m_label;
   };

} } // end namespace IQmol::Util
