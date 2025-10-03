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

#include "BasisEvaluator.h"
#include "GridEvaluator.h"
#include "ShellList.h"
#include "QsLog.h"
#include <QApplication>


using namespace qglviewer;

namespace IQmol {

BasisEvaluator::BasisEvaluator(
   Data::GridDataList& grids, 
   Data::ShellList& shellList, 
   QList<int> indices) 
    : m_grids(grids), 
      m_shellList(shellList), 
      m_indices(indices),
      m_evaluator(0)
{
   m_basisValues.resize({(size_t)m_indices.size()});

   using namespace std::placeholders;
   m_function = std::bind(&BasisEvaluator::functionValues, this, _1, _2, _3);

   double thresh(0.001);
   m_evaluator = new GridEvaluator(m_grids, m_function, thresh);
   connect(m_evaluator, SIGNAL(progress(int)), this, SIGNAL(progress(int)));

   m_totalProgress = m_evaluator->totalProgress();

   // Minimal list of required shells
   QList<unsigned> map(m_shellList.basisToShellMap());
   QSet<unsigned> set;
   for (auto idx : m_indices) {
       set.insert(map[idx]);
   }
   m_shellIndices = set.values();
   std::sort(m_shellIndices.begin(), m_shellIndices.end());

   m_shellOffsets = m_shellList.shellOffsets();
}


BasisEvaluator::~BasisEvaluator()
{
   if (m_evaluator) delete m_evaluator;
}


Vector const& BasisEvaluator::functionValues(double const x, double const y, double const z)
{
   double const* vals;
   m_basisValues.zero();

   for (auto idx : m_shellIndices) {
       vals = m_shellList[idx]->evaluate(x,y,z);

       if (vals) { // check if significant on this point
          unsigned bmin(m_shellOffsets[idx]);
          unsigned bmax = bmin +  m_shellList[idx]->nBasis();
          for (size_t i = 0; i < m_indices.size(); ++i) {
              if (bmin <= m_indices[i] && m_indices[i] < bmax) {
                 m_basisValues(i) = vals[m_indices[i]-bmin];
              }
          }
       }
   }
 
   return m_basisValues;
}


void BasisEvaluator::run()
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
   finished();
}

} // end namespace IQmol
