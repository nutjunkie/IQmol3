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
   QList<Vector const*> const& densities, bool coarseGrain) : m_grids(grids), m_shellList(shellList),
   m_densities(densities), m_evaluator(0)
{
   if (grids.isEmpty()) return;

//   m_shellList.setDensityVectors(densities);
   using namespace std::placeholders;
//   m_function = std::bind(&Data::ShellList::densityValues, &m_shellList, _1, _2, _3);
   m_function = std::bind(&DensityEvaluator::densityValues, this, _1, _2, _3, _4);

   double thresh(0.001);
   m_evaluator = new GridEvaluator(m_grids, m_function, thresh, coarseGrain);
   
   m_nBasis = m_shellList.nBasis();

   QList<unsigned> const shellOffsets(m_shellList.shellOffsets());
   m_shells.reserve(m_shellList.size());
   m_shellOffsets.reserve(shellOffsets.size());
   for (int shellIndex = 0; shellIndex < m_shellList.size(); ++shellIndex) {
      m_shells.push_back(m_shellList[shellIndex]);
      m_shellOffsets.push_back(shellOffsets[shellIndex]);
   }

   m_densityData.reserve(m_densities.size());
   for (Vector const* density : m_densities) {
      m_densityData.push_back(density->data());
   }

   connect(m_evaluator, SIGNAL(progress(int)), this, SIGNAL(progress(int)));
   connect(m_evaluator, SIGNAL(finished()), this, SLOT(evaluatorFinished()));

   m_totalProgress = m_evaluator->totalProgress();
}


DensityEvaluator::~DensityEvaluator()
{
   if (m_evaluator) delete m_evaluator;
}


void DensityEvaluator::densityValues(double const x, double const y, double const z, Vector& values)
{
   std::vector<double> shellValues;
   std::vector<unsigned> sigBasis(m_nBasis);
   std::vector<double> basisValues(m_nBasis);
   size_t const nden(m_densityData.size());
   unsigned nSigBas(0);

   if (values.size() != nden) {
      values.resize({nden});
   }
   values.zero();
   double* const output(values.data());

   // Determine the significant shells, and corresponding basis function indices
   for (size_t shellIndex = 0; shellIndex < m_shells.size(); ++shellIndex) {
       Data::Shell const* shell(m_shells[shellIndex]);
       if (!shell->evaluate(x, y, z, shellValues)) continue;

       unsigned const offset(m_shellOffsets[shellIndex]);
       unsigned const nbfs(shell->nBasis());
       for (unsigned i = 0; i < nbfs; ++i, ++nSigBas) {
           basisValues[nSigBas] = shellValues[i];
           sigBasis[nSigBas] = offset + i;
       }
   }
  
   // Now compute the basis function pair values on the grid
   for (unsigned i = 0; i < nSigBas; ++i) {
       double const xi(basisValues[i]);
       unsigned const ii(sigBasis[i]);
       unsigned const Ti((ii*(ii+1))/2);
       double const diagonal(xi*xi);

       for (unsigned j = 0; j < i; ++j) {
           double const pairWeight(4.0*xi*basisValues[j]);
           unsigned const jj(sigBasis[j]);
           unsigned const pairIndex(Ti + jj);
  
           for (size_t k = 0; k < nden; ++k) {
               output[k] += pairWeight * m_densityData[k][pairIndex];
           }
       }
       
       unsigned const diagonalIndex(Ti + ii);
       for (size_t k = 0; k < nden; ++k) {
           output[k] += diagonal * m_densityData[k][diagonalIndex];
       }
   }
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
