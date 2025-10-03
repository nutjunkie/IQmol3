#pragma once
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

#include "Util/Task.h"
#include "Data/GridData.h"
#include "Math/Function.h"
#include "Math/Matrix.h"


namespace IQmol {

   class GridEvaluator;

   namespace Data {
      class ShellList;
   }

   class OrbitalEvaluator : public Task {

      Q_OBJECT

      public:
         OrbitalEvaluator(
            Data::GridDataList& grids, 
            Data::ShellList const& shellList, 
            Matrix const& coefficients, 
            QList<int> indices);

         ~OrbitalEvaluator();

      Q_SIGNALS:
         void progress(int);

      protected:
         void run();

      private:
         Vector const& orbitalValues(double const x, double const y, double const z);

         MultiFunction3D         m_function;
         Data::GridDataList      m_grids;
         Data::ShellList const&  m_shellList;
         Matrix const&           m_coefficients;
         QList<int>              m_indices;
         GridEvaluator*          m_evaluator;

         Vector                  m_orbitalValues;
   };

} // end namespace IQmol
