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

#include "DensityEvaluator.h"
#include "GridEvaluator.h"
#include "ShellList.h"
#include "QsLog.h"
#include <QDebug>
#include <QApplication>

using namespace qglviewer;

namespace IQmol {


DensityEvaluator::DensityEvaluator(Data::GridDataList& grids, Data::ShellList& shellList, 
   QList<Vector const*> const& densities) : m_grids(grids), m_shellList(shellList),
   m_densities(densities), m_evaluator(0)
{
   if (grids.isEmpty()) return;

   m_returnValues.resize({(size_t)m_densities.size()});

   m_shellList.setDensityVectors(densities);
   using namespace std::placeholders;
//   m_function = std::bind(&Data::ShellList::densityValues, &m_shellList, _1, _2, _3);
   m_function = std::bind(&DensityEvaluator::densityValues, this, _1, _2, _3);

   double thresh(0.001);
   m_evaluator = new GridEvaluator(m_grids, m_function, thresh);
   
   size_t nBasis(m_shellList.nBasis());
   m_sigBasis = new unsigned[nBasis];
   m_basisValues.resize({nBasis});

   connect(m_evaluator, SIGNAL(progress(int)), this, SIGNAL(progress(int)));
   connect(m_evaluator, SIGNAL(finished()), this, SLOT(evaluatorFinished()));

   m_totalProgress = m_evaluator->totalProgress();
}


DensityEvaluator::~DensityEvaluator()
{
   if (m_sigBasis)  delete [] m_sigBasis;
   if (m_evaluator) delete m_evaluator;
}


Vector const& DensityEvaluator::densityValues(double const x, double const y, double const z)
{
   unsigned numbas, nSigBas(0), basoff(0);
   double const* values;
   unsigned nBasis(m_shellList.nBasis());

   // Determine the significant shells, and corresponding basis function indices
   for (auto shell = m_shellList.begin(); shell != m_shellList.end(); ++shell) {
       values = (*shell)->evaluate(x,y,z);
       numbas = (*shell)->nBasis(); 
      
       if (values) { // only add the significant shells
          for (unsigned i = 0; i < numbas; ++i, ++nSigBas, ++basoff) {
              m_basisValues(nSigBas) = values[i];
              m_sigBasis[nSigBas]    = basoff;
          }
       }else {
          basoff += numbas;
       }
   }
  
   double   xi, xij;
   unsigned ii, jj, Ti;
   unsigned nden(m_densities.size());
  
   m_returnValues.zero();
  
   // Now compute the basis function pair values on the grid
   for (unsigned i = 0; i < nSigBas; ++i) {
       xi = m_basisValues(i);
       ii = m_sigBasis[i];
       Ti = (ii*(ii+1))/2;
       for (unsigned j = 0; j < i; ++j) {
           xij = 2.0*xi*m_basisValues(j);
           jj  = m_sigBasis[j];
  
           for (unsigned k = 0; k < nden; ++k) {
               m_returnValues(k) += 2.0*xij*(*m_densities[k])(Ti+jj);
            }
       }
       
       for (unsigned k = 0; k < nden; ++k) {
           m_returnValues(k) += xi*xi*(*m_densities[k])(Ti+ii);
       }
   }

   return m_returnValues;
}


void DensityEvaluator::run()
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


void DensityEvaluator::evaluatorFinished()
{
   qDebug() << "DensityEvaluator finished";
//   m_evaluator->deleteLater();
//   m_evaluator = 0;
   finished();
}

} // end namespace IQmol
