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

#include "GridEvaluator.h"
#include "GridData.h"
#include "QsLog.h"
#include <QApplication>
#include <QElapsedTimer>
#include <algorithm>

#ifdef IQMOL_USE_OPENMP
#include <omp.h>
#endif


namespace IQmol {

void GridEvaluator::evaluateAndStore(double x, double y, double z, unsigned i,
   unsigned j, unsigned k, Vector& values)
{
   m_function(x, y, z, values);

   for (unsigned f = 0; f < m_grids.size(); ++f) {
      (*m_grids[f])(i, j, k) = values(f);
   }
}


void GridEvaluator::checkGrids()
{
   if (m_grids.isEmpty()) return;

   unsigned nx, ny, nz;
   Data::GridData* g0(m_grids.first());
   g0->getNumberOfPoints(nx, ny, nz);
   m_totalProgress = m_coarseGrain ? 4*nx : nx;

   Data::GridDataList::iterator iter;
   for (iter = m_grids.begin(); iter != m_grids.end(); ++iter) {
       if ( ((*iter)->size() != g0->size()) ) {
          QLOG_ERROR() << "Different sized grids found in GridEvaluator";
       }
    }

}


void GridEvaluator::run()
{
   if (m_coarseGrain) return runCoarseGrain();

   QElapsedTimer timer;
   timer.start();

   unsigned nGrids(m_grids.size());
   unsigned nx, ny, nz;

   Data::GridData* g0(m_grids.first());
   g0->getNumberOfPoints(nx, ny, nz);

   qglviewer::Vec origin(g0->origin());
   qglviewer::Vec delta(g0->delta());

#ifdef IQMOL_USE_OPENMP
   unsigned const chunkSize(std::max(1u, nx / 32u));

   for (unsigned chunkBegin = 0; chunkBegin < nx; chunkBegin += chunkSize) {
      if (m_terminate) break;

      unsigned const chunkEnd(std::min(nx, chunkBegin + chunkSize));

#pragma omp parallel
      {
         Vector values({nGrids});

#pragma omp for schedule(static)
         for (int ii = static_cast<int>(chunkBegin); ii < static_cast<int>(chunkEnd); ++ii) {
            double const x(origin.x + ii*delta.x);
            for (unsigned j = 0; j < ny; ++j) {
               double const y(origin.y + j*delta.y);
               for (unsigned k = 0; k < nz; ++k) {
                  double const z(origin.z + k*delta.z);
                  evaluateAndStore(x, y, z, ii, j, k, values);
               }
            }
         }
      }

      progress(chunkEnd);
   }
#else
   Vector values({nGrids});

   double x(origin.x);
   for (unsigned i = 0; i < nx; ++i, x += delta.x) {
       double y(origin.y);
       for (unsigned j = 0; j < ny; ++j, y += delta.y) {
           double z(origin.z);
           for (unsigned k = 0; k < nz; ++k, z += delta.z) {
               evaluateAndStore(x, y, z, i, j, k, values);
           }
       }
       progress(i); 
       if (m_terminate) break;
   }
#endif

   progress(m_totalProgress); 
   QLOG_INFO() << "Grid generation:" << (timer.elapsed() / 1000.0) << "seconds";
}



void GridEvaluator::runCoarseGrain()
{
   QElapsedTimer timer;
   timer.start();

   unsigned nGrids(m_grids.size());
   unsigned nx, ny, nz;
   Data::GridData* g0(m_grids.first());
   g0->getNumberOfPoints(nx, ny, nz);

   qglviewer::Vec origin(g0->origin());
   qglviewer::Vec delta(g0->delta());

   // We take a two-pass approach, the first computes data on a grid with half
   // the number of points for each dimension (so a factor of 8 fewer points
   // than the target grid).  The second pass fills in the remainder of the grid
   // either using interpolation (where the values are insignificant) or explicit
   // evaluation

   int prog(0);
   double x, y, z;

   // Just use the maximum function value at each grid point for screenting
   Cube screen;
   Cube::Shape extents{1+nx/2,1+ny/2,1+nz/2};
   screen.resize(extents);
   Vector values({nGrids});
   Vector v0({nGrids}), v1({nGrids}), v2({nGrids}), v3({nGrids});
   Vector v4({nGrids}), v5({nGrids}), v6({nGrids});
   unsigned const sparseSlices((nx + 1) / 2);

   // First Pass (sparse)
   x = origin.x;
#ifdef IQMOL_USE_OPENMP
   unsigned const sparseChunkSize(std::max(1u, sparseSlices / 32u));

   for (unsigned chunkBegin = 0; chunkBegin < sparseSlices; chunkBegin += sparseChunkSize) {
      if (m_terminate) return;

      unsigned const chunkEnd(std::min(sparseSlices, chunkBegin + sparseChunkSize));

#pragma omp parallel
      {
         Vector localValues({nGrids});

#pragma omp for schedule(static)
         for (int slice = static_cast<int>(chunkBegin); slice < static_cast<int>(chunkEnd); ++slice) {
            unsigned const i = 2*slice;
            double const x0(origin.x + i*delta.x);
            for (unsigned j = 0; j < ny; j += 2) {
               double const y0(origin.y + j*delta.y);
               for (unsigned k = 0; k < nz; k += 2) {
                  double const z0(origin.z + k*delta.z);
                  m_function(x0, y0, z0, localValues);
                  double max(0.0);
                  for (unsigned f = 0; f < nGrids; ++f) {
                     (*m_grids[f])(i, j, k) = localValues(f);
                     max = std::max(max, std::abs(localValues(f)));
                  }
                  screen(i/2,j/2,k/2) = max;
               }
            }
         }
      }

      prog += chunkEnd - chunkBegin;
      progress(prog);
   }
#else
   for (unsigned i = 0; i < nx; i += 2, x += 2.0*delta.x) {
       y = origin.y;
       for (unsigned j = 0; j < ny; j += 2, y += 2.0*delta.y) {
           z = origin.z;
           for (unsigned k = 0; k < nz; k += 2, z += 2.0*delta.z) {
               m_function(x, y, z, values);
               double max(0.0);
               for (unsigned f = 0; f < nGrids; ++f) {
                   (*m_grids[f])(i, j, k) = values(f);
                   max = std::max(max, std::abs(values(f)));
               }
               screen(i/2,j/2,k/2) = max;
           }
       }
       progress(prog++); 
       if (m_terminate) return;
   }
#endif

   double g000, g001, g010, g011, g100, g101, g110, g111;
   origin += delta;

   x = origin.x;
   for (unsigned i = 1;  i < nx-1;  i += 2, x += 2.0*delta.x) {
       y = origin.y;
       for (unsigned j = 1;  j < ny-1;  j += 2, y += 2.0*delta.y) {
           z = origin.z;
           for (unsigned k = 1;  k < nz-1;  k += 2, z += 2.0*delta.z) {

               // Compute exact values
               if (screen((i-1)/2,(j-1)/2,(k-1)/2) > 0.125*m_thresh) {

                  m_function(x, y, z, v0);
                  for (unsigned f = 0; f < nGrids; ++f) (*m_grids[f])(i,  j,  k  ) = v0(f);

                  m_function(x, y, z-delta.z, v1);
                  for (unsigned f = 0; f < nGrids; ++f) (*m_grids[f])(i,  j,  k-1) = v1(f);

                  m_function(x, y-delta.y, z, v2);
                  for (unsigned f = 0; f < nGrids; ++f) (*m_grids[f])(i,  j-1,k  ) = v2(f);

                  m_function(x, y-delta.y, z-delta.z, v3);
                  for (unsigned f = 0; f < nGrids; ++f) (*m_grids[f])(i,  j-1,k-1) = v3(f);

                  m_function(x-delta.x, y, z, v4);
                  for (unsigned f = 0; f < nGrids; ++f) (*m_grids[f])(i-1,j,  k  ) = v4(f);

                  m_function(x-delta.x, y, z-delta.z, v5);
                  for (unsigned f = 0; f < nGrids; ++f) (*m_grids[f])(i-1,j,  k-1) = v5(f);

                  m_function(x-delta.x, y-delta.y, z, v6);
                  for (unsigned f = 0; f < nGrids; ++f) (*m_grids[f])(i-1,j-1,k  ) = v6(f);
 
               }else {
                  // Use interpolation
                  for (unsigned f = 0; f < nGrids; ++f) {
                      g000 = (*m_grids[f])(i-1, j-1, k-1);
                      g001 = (*m_grids[f])(i-1, j-1, k+1);
                      g010 = (*m_grids[f])(i-1, j+1, k-1);
                      g011 = (*m_grids[f])(i-1, j+1, k+1);
                      g100 = (*m_grids[f])(i+1, j-1, k-1);
                      g101 = (*m_grids[f])(i+1, j-1, k+1);
                      g110 = (*m_grids[f])(i+1, j+1, k-1);
                      g111 = (*m_grids[f])(i+1, j+1, k+1);

                      (*m_grids[f])(i,  j,  k  ) = 0.125*(g000+g001+g010+g011+
                                                          g100+g101+g110+g111);
                      (*m_grids[f])(i,  j,  k-1) = 0.250*(g000+g010+g100+g110);
                      (*m_grids[f])(i,  j-1,k  ) = 0.250*(g000+g001+g100+g101);
                      (*m_grids[f])(i,  j-1,k-1) = 0.500*(g000+g100);
                      (*m_grids[f])(i-1,j,  k  ) = 0.250*(g000+g001+g010+g011);
                      (*m_grids[f])(i-1,j,  k-1) = 0.500*(g000+g010);
                      (*m_grids[f])(i-1,j-1,k  ) = 0.500*(g000+g001);
                  }

               }

            }
       }

       prog += 7;
       progress(prog); 
       if (m_terminate) return;
   }
   progress(m_totalProgress); 
   QLOG_INFO() << "Coarse grid generation:" << (timer.elapsed() / 1000.0) << "seconds";
}

} // end namespace IQmol
