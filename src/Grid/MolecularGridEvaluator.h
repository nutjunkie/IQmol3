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
#include "Math/Matrix.h"
#include "Data/GridData.h"


namespace IQmol {

   namespace Data {
      class ShellList;
      class Density;
   }

   class MolecularGridEvaluator : public Task {

      Q_OBJECT

      public:
         MolecularGridEvaluator(Data::GridDataList& grids, Data::ShellList& shellList, 
            Matrix const& alphaCoefficients, Matrix const& betaCoefficients, 
            QList<Data::Density*> const& densities,
            Matrix const& alphaImaginaryCoefficients = Matrix(), 
            Matrix const& betaImaginaryCoefficients = Matrix()
         );

         Data::GridDataList const& getGrids() const { return m_grids; }

      Q_SIGNALS:
         void progressLabelText(QString const& label);
         void progressMaximum(int max);
         void progressValue(int progress);

      protected:
         void run();

      private:
         void runTask(Task& task);

         Data::GridDataList m_grids;
         Data::ShellList&   m_shellList;
         Matrix const&      m_alphaCoefficients;
         Matrix const&      m_betaCoefficients;

         QList<Data::Density*> m_densities;

         Matrix const&      m_alphaImaginaryCoefficients;
         Matrix const&      m_betaImaginaryCoefficients;
   };

} // end namespace IQmol
