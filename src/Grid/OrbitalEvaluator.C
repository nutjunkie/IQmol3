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

#include "OrbitalEvaluator.h"
#include "GridEvaluator.h"
#include "ShellList.h"
#include "QsLog.h"
#include <QApplication>


using namespace qglviewer;

namespace IQmol {

OrbitalEvaluator::OrbitalEvaluator(Data::GridDataList& grids, Data::ShellList& shellList, 
   Matrix const& coefficients, QList<int> indices) : m_grids(grids), m_shellList(shellList),
   m_coefficients(coefficients), m_indices(indices)
{
   m_shellList.setOrbitalVectors(coefficients, indices);
   m_returnValues.resize(m_indices.size());
   m_function = std::bind(&Data::ShellList::orbitalValues, &m_shellList, 
      std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

   double thresh(0.001);
   m_evaluator = new MultiGridEvaluator(m_grids, m_function, thresh);
   connect(m_evaluator, SIGNAL(progress(int)), this, SIGNAL(progress(int)));
   connect(m_evaluator, SIGNAL(finished()), this, SLOT(evaluatorFinished()));

   m_totalProgress = m_evaluator->totalProgress();
}


void OrbitalEvaluator::run()
{
   m_evaluator->start();
   while (m_evaluator->isRunning()) {
      msleep(100); 
      QApplication::processEvents();
      if (m_terminate) {
         m_evaluator->stopWhatYouAreDoing();
         m_evaluator->wait();
      }
   }
}


void OrbitalEvaluator::evaluatorFinished()
{
   qDebug() << "OrbitalEvaluator finished";
//   m_evaluator->deleteLater();
//   m_evaluator = 0;
   finished();
}

} // end namespace IQmol
