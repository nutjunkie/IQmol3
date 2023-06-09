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

#include <QWidget>

namespace Qui {

   /// Template class for the Tabs that appear in the Setup panel.  Each Tab is
   /// specialized via the ui 

   template <class U>
   class Tab : public QWidget {

      public:
         Tab(QWidget* parent) : QWidget(parent) 
         {
            m_ui.setupUi(this);
         }

         // Public access for the logic to operate
         U m_ui; 
   };

} // end namespace Qui
