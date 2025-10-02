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

   m_function = std::bind(&BasisEvaluator::functionValues, this, 
      std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

   double thresh(0.001);
   m_evaluator = new GridEvaluator(m_grids, m_function, thresh);
   connect(m_evaluator, SIGNAL(progress(int)), this, SIGNAL(progress(int)));

   m_totalProgress = m_evaluator->totalProgress();
}


BasisEvaluator::~BasisEvaluator()
{
   if (m_evaluator) delete m_evaluator;
}


Vector const& BasisEvaluator::functionValues(double const x, double const y, double const z)
{
// TODO make this more efficient by avoiding evaluation of all of the basis functions
   unsigned nfun(m_indices.size());
   unsigned offset(0);
   unsigned nbfs;
   double const* vals;

   Vector f({m_shellList.nBasis()});

   f.zero();
   m_basisValues.zero();
  
   for (auto shell = m_shellList.begin(); shell != m_shellList.end(); ++shell) {
       nbfs = (*shell)->nBasis();
       vals = (*shell)->evaluate(x, y, z);

       if (vals) {
          for (unsigned s = 0; s < (*shell)->nBasis(); ++s) {
              f(offset+s) = vals[s];
          }
       }
       offset += nbfs; 
   }

   for (unsigned i = 0; i < nfun; ++i) {
       m_basisValues(i) = f(m_indices[i]);
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
