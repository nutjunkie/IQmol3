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
   QList<int> indices) 
    : m_grids(grids), 
      m_shellList(shellList),
      m_coefficients(coefficients), 
      m_indices(indices),
      m_evaluator(0)
{
   m_orbitalValues.resize({(size_t)m_indices.size()});

   using namespace std::placeholders;
   m_function = std::bind(&OrbitalEvaluator::orbitalValues, this, _1, _2, _3);

   double thresh(0.001);
   m_evaluator = new GridEvaluator(m_grids, m_function, thresh);
   connect(m_evaluator, SIGNAL(progress(int)), this, SIGNAL(progress(int)));
   connect(m_evaluator, SIGNAL(finished()), this, SIGNAL(finished()));

   m_totalProgress = m_evaluator->totalProgress();
}


OrbitalEvaluator::~OrbitalEvaluator()
{
   if (m_evaluator) delete m_evaluator;
}


Vector const& OrbitalEvaluator::orbitalValues(double const x, double const y, double const z)
{
   unsigned norb(m_indices.size());
   unsigned offset(0);
   unsigned nbfs;
   double const* vals;

   m_orbitalValues.zero();

   for (auto shell = m_shellList.begin(); shell != m_shellList.end(); ++shell) {
       vals = (*shell)->evaluate(x,y,z);
       nbfs = (*shell)->nBasis();

       if (vals) { // only add the significant shells
          for (unsigned i = 0; i < nbfs; ++i) {
              for (unsigned k = 0; k < norb; ++k) {
                  m_orbitalValues(k) += m_coefficients(m_indices[k], offset+i) * vals[i];
              } 
          } 
       }
       offset += nbfs;
   }   
   return m_orbitalValues;
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
