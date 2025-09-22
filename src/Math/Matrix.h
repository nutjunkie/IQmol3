#pragma once
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

#include <cassert>
#include <ostream>

#include "Array.h"


namespace IQmol {

typedef Array<2,double> Matrix;

template<typename T>
Array<2,T> product(Array<2,T> const& A, Array<2,T> const& B, size_t blockSize = 64)
{
   const size_t m = A.shape()[0];
   const size_t k = A.shape()[1];
   const size_t n = B.shape()[1];

   assert(k == B.shape()[0] && "Matrix product shape mismatch: (m x k) * (k x n)");

   Array<2,T> C({m, n});

   for (size_t i0 = 0; i0 < m; i0 += blockSize) {
       for (size_t k0 = 0; k0 < k; k0 += blockSize) {
           for (size_t j0 = 0; j0 < n; j0 += blockSize) {

               const size_t iMax = std::min(i0 + blockSize, m);
               const size_t kMax = std::min(k0 + blockSize, k);
               const size_t jMax = std::min(j0 + blockSize, n);

               for (size_t i = i0; i < iMax; ++i) {
                   for (size_t k = k0; k < kMax; ++k) {
                       const T aik = A(i,k);
                       for (size_t j = j0; j < jMax; ++j) {
                           C(i,j) += aik * B(k,j);
                       }
                   }
               }
           }
       }
    }
   return C;
}


template<typename T>
Array<2,T> transpose(Array<2,T> const& A, size_t const blockSize = 64)
{
   const size_t m = A.shape()[0];
   const size_t n = A.shape()[1];

   Array<2,T> B({n,m});

   for (size_t i0 = 0; i0 < m; i0 += blockSize) {
       for (size_t j0 = 0; j0 < n; j0 += blockSize) {
           const size_t iMax = std::min(i0 + blockSize, m);
           const size_t jMax = std::min(j0 + blockSize, n);
           for (size_t i = i0; i < iMax; ++i) {
               for (size_t j = j0; j < jMax; ++j) {
                   B(j,i) = A(i,j);
               }
           }
       }
   }
   return B;
}


namespace detail 
{
   inline int mat_cols_xalloc() 
   { 
      static int idx = std::ios_base::xalloc(); return idx; 
   }
}


// Manipulator: std::cout << matrix_cols(8) << A;
struct matrix_cols_t 
{ 
   size_t n; 
};

inline matrix_cols_t setcols(size_t n) 

{ 
   return {n}; 
}


inline std::ostream& operator<<(std::ostream& os,  matrix_cols_t m) 
{
    os.iword(detail::mat_cols_xalloc()) = static_cast<long>(m.n);
    return os;
}


// Printer for Array<2,T>
template<typename T>
std::ostream& operator<<(std::ostream& os, const Array<2,T>& A) 
{
    const auto rows = A.shape()[0];
    const auto cols = A.shape()[1];

    // get per-stream setting; default 6 if unset/nonpositive
    long ncols_l = os.iword(detail::mat_cols_xalloc());
    size_t ncols = (ncols_l > 0) ? static_cast<size_t>(ncols_l) : 6u;

    // Respect any width the caller set (need to reapply per element)
    const std::streamsize cellw = os.width(); // 0 means "no fixed width"
    const bool usew = (cellw > 0);

    if (cols == 0 || rows == 0) {
        return os << "[ " << rows << " x " << cols << " ] (empty)\n";
    }

    for (size_t c0 = 0; c0 < cols; c0 += ncols) {
        const size_t c1 = std::min(cols, c0 + ncols);

        if (cols > ncols) os << "[ columns " << c0 << "…" << (c1 - 1) << " ]\n";

        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = c0; j < c1; ++j) {
                if (j > c0) os << ' ';
                if (usew) os.width(cellw);
                os << A(i, j);
            }
            os << '\n';
        }
        if (c1 < cols) os << '\n';
    }
    return os;
}


} // end namespace IQmol
