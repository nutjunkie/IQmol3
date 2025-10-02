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

#include "Grid/MolecularGridEvaluator.h"
#include "Grid/DensityEvaluator.h"
#include "Grid/OrbitalEvaluator.h"
#include "Grid/ComplexOrbitalEvaluator.h"
#include "Grid/BasisEvaluator.h"
#include "Data/ShellList.h"
#include "Data/Density.h"
#include "Util/QsLog.h"
#include <set>
#include <QApplication>


using namespace qglviewer;

namespace IQmol {


MolecularGridEvaluator::MolecularGridEvaluator(
   Data::GridDataList& grids, 
   Data::ShellList& shellList, 
   Matrix const& alphaCoefficients, 
   Matrix const& betaCoefficients, 
   QList<Data::Density*> const& densities,
   Matrix const& alphaImaginaryCoefficients, 
   Matrix const& betaImaginaryCoefficients) 
    : m_grids(grids),
      m_shellList(shellList),
      m_alphaCoefficients(alphaCoefficients),
      m_betaCoefficients(betaCoefficients),
      m_densities(densities),
      m_alphaImaginaryCoefficients(alphaImaginaryCoefficients),
      m_betaImaginaryCoefficients(betaImaginaryCoefficients)
{
}


void MolecularGridEvaluator::run()
{
   Data::GridDataList::iterator iter;
   for (iter = m_grids.begin(); iter != m_grids.end(); ++iter) {
       (*iter)->surfaceType().dump();
   }

   // First obtain a list of the unique grid sizes.  This avoids calculating
   // shell(pair) data on the same grid points more than once.
   std::set<Data::GridSize> sizes;
   for (iter = m_grids.begin(); iter != m_grids.end(); ++iter) {
       sizes.insert((*iter)->size());    
   }

   // Second, determine what data are required for each grid size.  We do 
   // this loop so that we don't have to recompute shell and/or shell-pair
   // values over the same grid points.
   QLOG_TRACE() << "Computing data for" << m_grids.size() << "grids";
   QLOG_TRACE() << "There are" << sizes.size() << "different grid sizes";
   std::set<Data::GridSize>::iterator size;

   unsigned sizeCount(1);

   for (size = sizes.begin(); size != sizes.end(); ++size, ++sizeCount) {
       Data::GridDataList densityGrids;
       Data::GridDataList alphaGrids;
       Data::GridDataList betaGrids;
       Data::GridDataList basisGrids;

       Data::GridDataList alphaRealGrids;
       Data::GridDataList betaRealGrids;
       Data::GridDataList alphaImaginaryGrids;
       Data::GridDataList betaImaginaryGrids;


       QList<Vector const*> densityVectors;

       QList<int>  alphaOrbitals;
       QList<int>  betaOrbitals;
       QList<int>  basisFunctions;
       QList<int>  alphaComplexOrbitals;
       QList<int>  betaComplexOrbitals;

       for (iter = m_grids.begin(); iter != m_grids.end(); ++iter) {
           if ((*iter)->size() == *size) {

              Data::SurfaceType type((*iter)->surfaceType());
              bool found(false);
qDebug() << "Surface type requested:" << type.toString();

              if (type.isDensity()) {
                 // Find the corresponding density matrix
                 for (int i = 0; i < m_densities.size(); ++i) {
                     if (m_densities[i]->surfaceType() == (*iter)->surfaceType()) {
                        densityGrids.append(*iter);
                        densityVectors.append(m_densities[i]->vector());
                        found = true;
                        break;
                     }
                 }
         
              }else if (type.kind() == Data::SurfaceType::BasisFunction) {
                 found = true;
                 basisGrids.append(*iter);
                 basisFunctions.append(type.index());

              }else if (type.kind() == Data::SurfaceType::GenericOrbital ||
                        type.kind() == Data::SurfaceType::AlphaOrbital) {
                 found = true;
                 alphaGrids.append(*iter);
                 alphaOrbitals.append(type.index());
                 
              }else if (type.kind() == Data::SurfaceType::BetaOrbital) {
                 found = true;
                 betaGrids.append(*iter);
                 betaOrbitals.append(type.index());

              // Complex orbital handling
              }else if (type.kind() == Data::SurfaceType::AlphaRealOrbital) {
                 found = true;
                 alphaRealGrids.append(*iter);
                 alphaComplexOrbitals.append(type.index());

              }else if (type.kind() == Data::SurfaceType::AlphaImaginaryOrbital) {
                 found = true;
                 alphaImaginaryGrids.append(*iter);

              }else if (type.kind() == Data::SurfaceType::BetaRealOrbital) {
                 found = true;
                 betaRealGrids.append(*iter);
                 betaComplexOrbitals.append(type.index());

              }else if (type.kind() == Data::SurfaceType::BetaImaginaryOrbital) {
                 found = true;
                 betaImaginaryGrids.append(*iter);
              // ------------------------

              }else if (type.kind() == Data::SurfaceType::DysonLeft) {
                 found = true;
                 alphaGrids.append(*iter);
                 alphaOrbitals.append(type.index());
                 
              }else if (type.kind() == Data::SurfaceType::DysonRight) {
                 found = true;
                 betaGrids.append(*iter);
                 betaOrbitals.append(type.index());

              }else if (type.kind() == Data::SurfaceType::Custom) {
                 for (int i = 0; i < m_densities.size(); ++i) {
                     if (type.label() == m_densities[i]->label()) {
                        QLOG_DEBUG() << "Pairing successful" << type.label() << m_densities[i]->label(); 
                        densityGrids.append(*iter);
                        densityVectors.append(m_densities[i]->vector());
                        found  = true;
                        break;
                     }
                 }

              }else  {
                 QLOG_WARN() << "Unknown Grid type found in processQueue";
              }

              if (!found) {
                 QLOG_WARN() << "Failed to pair data with grid";
                 type.dump();
              }
           }
       }

       // Run Evaluators

       if (!basisFunctions.isEmpty() && !m_terminate) {
          QString s("Computing basis functions on grid ");
          s += QString::number(sizeCount);
          progressLabelText(s);

          QLOG_TRACE() << "MGE: Computing" << basisFunctions.size() << "basis function grids";
          BasisEvaluator evaluator(basisGrids, m_shellList, basisFunctions);

          runTask(evaluator);

          QLOG_TRACE() << "Time taken to compute basis function grids:" << evaluator.timeTaken();
       }

       if (!alphaOrbitals.isEmpty() && !m_terminate) {
          QString s("Computing alpha orbitals on grid ");
          s += QString::number(sizeCount);
          progressLabelText(s);

          QLOG_TRACE() << "MGE: Computing" << alphaOrbitals.size() << "alpha orbital grids";
          OrbitalEvaluator evaluator(alphaGrids, m_shellList, m_alphaCoefficients, 
             alphaOrbitals);

          runTask(evaluator);
          QLOG_TRACE() << "Time taken to compute orbital grids:" << evaluator.timeTaken();
       }

       if (!betaOrbitals.isEmpty() && !m_terminate) {
          QString s("Computing beta orbitals on grid ");
          s += QString::number(sizeCount);
          progressLabelText(s);

          OrbitalEvaluator evaluator(betaGrids, m_shellList, m_betaCoefficients, 
             betaOrbitals);

          runTask(evaluator);
          QLOG_TRACE() << "Time taken to compute orbital grids:" << evaluator.timeTaken();
       }

       if (!densityGrids.isEmpty() && !m_terminate) {
          QString s("Computing density on grid ");
          s += QString::number(sizeCount);
          progressLabelText(s);

          QLOG_TRACE() << "MGE: Computing" << densityVectors.size() << "density grids";
          DensityEvaluator evaluator(densityGrids, m_shellList, densityVectors);

          runTask(evaluator);
          QLOG_TRACE() << "Time taken to compute density grids:" << evaluator.timeTaken();
       }


       // This is wasteful as it is recomputing shell data. 
       // TODO: re-engineer to allow imaginary coefficients in the above evaluators
       if (!alphaComplexOrbitals.isEmpty() && !m_terminate) {
          QString s("Computing imaginary alpha orbitals on grid ");
          s += QString::number(sizeCount);
          progressLabelText(s);

          ComplexOrbitalEvaluator evaluator(alphaRealGrids, alphaImaginaryGrids, m_shellList, 
             m_alphaCoefficients, m_alphaImaginaryCoefficients, alphaComplexOrbitals);

          runTask(evaluator);
          QLOG_TRACE() << "Time taken to compute imaginary orbital grids:" << evaluator.timeTaken();
       }

       if (!betaComplexOrbitals.isEmpty() && !m_terminate) {
          QString s("Computing imaginary beta orbitals on grid ");
          s += QString::number(sizeCount);
          progressLabelText(s);

          ComplexOrbitalEvaluator evaluator(betaRealGrids, betaImaginaryGrids, m_shellList, 
             m_betaCoefficients, m_betaImaginaryCoefficients, betaComplexOrbitals);

          runTask(evaluator);
          QLOG_TRACE() << "Time taken to compute imaginary orbital grids:" << evaluator.timeTaken();
       }
   }
}


void MolecularGridEvaluator::runTask(Task& task)
{
   progressMaximum(task.totalProgress());
   progressValue(0);
   connect(&task, SIGNAL(progress(int)), this, SIGNAL(progressValue(int)));  

   task.start();
   while (task.isRunning()) {
      msleep(100);
      QApplication::processEvents();
      if (m_terminate) {
         task.stopWhatYouAreDoing();
         task.wait();
      }
   }
}

} // end namespace IQmol
