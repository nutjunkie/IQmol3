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
#include "Math/Matrix.h"


namespace IQmol {

   class OrbitalEvaluator;

   namespace Data {
      class ShellList;
   }

   class ComplexOrbitalEvaluator : public Task {

      Q_OBJECT

      public:
         ComplexOrbitalEvaluator(
            Data::GridDataList& realGgrids, 
            Data::GridDataList& imaginaryGrids, 
            Data::ShellList const& shellList, 
            Matrix const& realCoefficients, 
            Matrix const& imaginaryCoefficients, 
            QList<int> indices);

         ~ComplexOrbitalEvaluator();

      Q_SIGNALS:
         void progress(int);

      protected:
         void run();

      private:
         Data::GridDataList  m_realGrids;
         Data::GridDataList  m_imaginaryGrids;

         OrbitalEvaluator* m_reEvaluator;
         OrbitalEvaluator* m_imEvaluator;

         void convertToPolar();
   };

} // end namespace IQmol
