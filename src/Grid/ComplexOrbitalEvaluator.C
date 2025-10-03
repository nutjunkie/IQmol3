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

#include "ComplexOrbitalEvaluator.h"
#include "OrbitalEvaluator.h"
#include "QsLog.h"
#include <QApplication>



using namespace qglviewer;

namespace IQmol {

ComplexOrbitalEvaluator::ComplexOrbitalEvaluator(
   Data::GridDataList& realGrids, 
   Data::GridDataList& imaginaryGrids, 
   Data::ShellList const& shellList, 
   Matrix const& realCoefficients, 
   Matrix const& imaginaryCoefficients, 
   QList<int> indices) 
    : m_realGrids(realGrids), 
      m_imaginaryGrids(imaginaryGrids), 
      m_reEvaluator(0),
      m_imEvaluator(0)
{
   m_reEvaluator = new OrbitalEvaluator(m_realGrids, shellList, realCoefficients, indices);
   m_imEvaluator = new OrbitalEvaluator(m_imaginaryGrids, shellList, imaginaryCoefficients, indices);

   connect(m_reEvaluator, SIGNAL(progress(int)), this, SIGNAL(progress(int)));
   connect(m_imEvaluator, SIGNAL(progress(int)), this, SIGNAL(progress(int)));

   m_totalProgress = m_reEvaluator->totalProgress() + m_imEvaluator->totalProgress();
}


ComplexOrbitalEvaluator::~ComplexOrbitalEvaluator()
{
   if (m_reEvaluator) delete m_reEvaluator;
   if (m_imEvaluator) delete m_imEvaluator;
}


void ComplexOrbitalEvaluator::run()
{
   m_reEvaluator->start();
   while (m_reEvaluator->isRunning()) {
      msleep(100); 
      QApplication::processEvents();
      if (m_terminate) {
         m_reEvaluator->stopWhatYouAreDoing();
         m_reEvaluator->wait();
      }
   }

   m_imEvaluator->start();
   while (m_imEvaluator->isRunning()) {
      msleep(100); 
      QApplication::processEvents();
      if (m_terminate) {
         m_imEvaluator->stopWhatYouAreDoing();
         m_imEvaluator->wait();
      }
   }

   convertToPolar();
}


void ComplexOrbitalEvaluator::convertToPolar()
{
   Data::GridDataList::iterator iter;

   if (m_realGrids.size() != m_imaginaryGrids.size()) {
      QLOG_ERROR() << "Grid mismatch in ComplexOrbitalEvaluator";
      return;
   }

   size_t nGrids = m_realGrids.size();
   unsigned nx, ny, nz; 

   QLOG_INFO() << "Converting Re & Im to Mod & Arg grids";
   for (size_t i = 0; i < nGrids; ++i) {
       Data::GridData& reGrid = *m_realGrids[i];
       Data::GridData& imGrid = *m_imaginaryGrids[i];

       reGrid.getNumberOfPoints(nx, ny, nz);
       for (size_t i = 0; i < nx; ++i) {
           for (size_t j = 0; j < ny; ++j) {
               for (size_t k = 0; k < nz; ++k) {
                   double re = reGrid(i,j,k);
                   double im = imGrid(i,j,k);
                   reGrid(i,j,k) = std::sqrt(re*re+im*im);
                   imGrid(i,j,k) = std::atan2(im,re);
               }
           }
       }
   }

   qDebug() << "Printing grids from ComplexOrbitalEvaluator";
   for (auto iter = m_realGrids.begin(); iter != m_realGrids.end(); ++iter) {
       (*iter)->dump();
   }
   for (auto iter = m_imaginaryGrids.begin(); iter != m_imaginaryGrids.end(); ++iter) {
       (*iter)->dump();
   }

   finished();
}

} // end namespace IQmol
