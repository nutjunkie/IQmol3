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

OrbitalEvaluator::OrbitalEvaluator(
   Data::GridDataList& grids, 
   Data::ShellList const& shellList, 
   Matrix const& coefficients, 
   QList<int> indices,
   bool coarseGrain) 
    : m_grids(grids), 
      m_shellList(shellList),
      m_coefficients(coefficients), 
      m_indices(indices),
      m_evaluator(0)
{
   using namespace std::placeholders;
   m_function = std::bind(&OrbitalEvaluator::orbitalValues, this, _1, _2, _3, _4);

   double thresh(0.001);
   m_evaluator = new GridEvaluator(m_grids, m_function, thresh, coarseGrain);
   connect(m_evaluator, SIGNAL(progress(int)), this, SIGNAL(progress(int)));
   connect(m_evaluator, SIGNAL(finished()), this, SIGNAL(finished()));

   m_totalProgress = m_evaluator->totalProgress();

   QList<unsigned> const shellOffsets(m_shellList.shellOffsets());
   m_shells.reserve(m_shellList.size());
   m_shellOffsets.reserve(shellOffsets.size());
   for (int shellIndex = 0; shellIndex < m_shellList.size(); ++shellIndex) {
      m_shells.push_back(m_shellList[shellIndex]);
      m_shellOffsets.push_back(shellOffsets[shellIndex]);
   }

   size_t const nBasis(m_coefficients.shape()[1]);
   double const* coefficientData(m_coefficients.data());
   m_coefficientRows.reserve(m_indices.size());
   for (int orbitalIndex : m_indices) {
      m_coefficientRows.push_back(coefficientData + size_t(orbitalIndex) * nBasis);
   }
}


OrbitalEvaluator::~OrbitalEvaluator()
{
   if (m_evaluator) delete m_evaluator;
}


void OrbitalEvaluator::orbitalValues(double const x, double const y, double const z, Vector& values)
{
   std::vector<double> shellValues;
   size_t const norb(m_coefficientRows.size());

   if (values.size() != norb) {
      values.resize({norb});
   }
   values.zero();
   double* const output(values.data());

   for (size_t shellIndex = 0; shellIndex < m_shells.size(); ++shellIndex) {
       Data::Shell const* shell(m_shells[shellIndex]);
       if (!shell->evaluate(x, y, z, shellValues)) continue;

       unsigned const offset(m_shellOffsets[shellIndex]);
       unsigned const nbfs(shell->nBasis());
       double const* const shellData(shellValues.data());

       for (size_t orbital = 0; orbital < norb; ++orbital) {
           double const* const coefficients(m_coefficientRows[orbital] + offset);
           double sum(output[orbital]);
           for (unsigned i = 0; i < nbfs; ++i) {
               sum += coefficients[i] * shellData[i];
           }
           output[orbital] = sum;
       }
   }
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


} // end namespace IQmol
