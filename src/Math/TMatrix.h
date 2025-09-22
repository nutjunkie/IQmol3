#pragma once
/*******************************************************************************

  Copyright (C) 2023 Andrew Gilbert

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


#include <vector>

namespace IQmol {

template <typename T> 
class TMatrix {

   public:
      TMatrix(unsigned _rows, unsigned _cols, const T& _initial)
      {
         mat.resize(_rows);
         for (unsigned i(0); i<mat.size(); i++) {
             mat[i].resize(_cols, _initial);
         }
         rows = _rows;
         cols = _cols;
      }

      TMatrix(const TMatrix<T>& rhs);
      virtual ~TMatrix();

      TMatrix<T>& operator=(const TMatrix<T>& rhs);
      TMatrix<T> operator+(const TMatrix<T>& rhs);
      TMatrix<T>& operator+=(const TMatrix<T>& rhs);
      TMatrix<T> operator-(const TMatrix<T>& rhs);
      TMatrix<T>& operator-=(const TMatrix<T>& rhs);
      TMatrix<T> operator*(const TMatrix<T>& rhs);
      TMatrix<T>& operator*=(const TMatrix<T>& rhs);
      TMatrix<T> transpose();

      TMatrix<T> operator+(const T& rhs);
      TMatrix<T> operator-(const T& rhs);
      TMatrix<T> operator*(const T& rhs);
      TMatrix<T> operator/(const T& rhs);

      std::vector<T> operator*(const std::vector<T>& rhs);
      std::vector<T> diag_vec();

      T& operator()(const unsigned& row, const unsigned& col);
      const T& operator()(const unsigned& row, const unsigned& col) const;

      unsigned get_rows() const;
      unsigned get_cols() const;

   private:
      std::vector<std::vector<T> > mat;
      unsigned rows;
      unsigned cols;
};


// Copy Constructor
template<typename T>
TMatrix<T>::TMatrix(const TMatrix<T>& rhs) {
  mat = rhs.mat;
  rows = rhs.get_rows();
  cols = rhs.get_cols();
}

// (Virtual) Destructor
template<typename T>
TMatrix<T>::~TMatrix() {}

// Assignment Operator
template<typename T>
TMatrix<T>& TMatrix<T>::operator=(const TMatrix<T>& rhs) {
  if (&rhs == this)
    return *this;

  unsigned new_rows = rhs.get_rows();
  unsigned new_cols = rhs.get_cols();

  mat.resize(new_rows);
  for (unsigned i=0; i<mat.size(); i++) {
    mat[i].resize(new_cols);
  }

  for (unsigned i=0; i<new_rows; i++) {
    for (unsigned j=0; j<new_cols; j++) {
      mat[i][j] = rhs(i, j);
    }
  }
  rows = new_rows;
  cols = new_cols;

  return *this;
}

// Addition of two matrices
template<typename T>
TMatrix<T> TMatrix<T>::operator+(const TMatrix<T>& rhs) {
  TMatrix result(rows, cols, 0.0);

  for (unsigned i=0; i<rows; i++) {
    for (unsigned j=0; j<cols; j++) {
      result(i,j) = this->mat[i][j] + rhs(i,j);
    }
  }

  return result;
}

// Cumulative addition of this matrix and another
template<typename T>
TMatrix<T>& TMatrix<T>::operator+=(const TMatrix<T>& rhs) {
  unsigned rows = rhs.get_rows();
  unsigned cols = rhs.get_cols();

  for (unsigned i=0; i<rows; i++) {
    for (unsigned j=0; j<cols; j++) {
      this->mat[i][j] += rhs(i,j);
    }
  }

  return *this;
}

// Subtraction of this matrix and another
template<typename T>
TMatrix<T> TMatrix<T>::operator-(const TMatrix<T>& rhs) {
  unsigned rows = rhs.get_rows();
  unsigned cols = rhs.get_cols();
  TMatrix result(rows, cols, 0.0);

  for (unsigned i=0; i<rows; i++) {
    for (unsigned j=0; j<cols; j++) {
      result(i,j) = this->mat[i][j] - rhs(i,j);
    }
  }

  return result;
}

// Cumulative subtraction of this matrix and another
template<typename T>
TMatrix<T>& TMatrix<T>::operator-=(const TMatrix<T>& rhs) {
  unsigned rows = rhs.get_rows();
  unsigned cols = rhs.get_cols();

  for (unsigned i=0; i<rows; i++) {
    for (unsigned j=0; j<cols; j++) {
      this->mat[i][j] -= rhs(i,j);
    }
  }

  return *this;
}

// Left multiplication of this matrix and another
template<typename T>
TMatrix<T> TMatrix<T>::operator*(const TMatrix<T>& rhs) {
  unsigned rows = rhs.get_rows();
  unsigned cols = rhs.get_cols();
  TMatrix result(rows, cols, 0.0);

  for (unsigned i=0; i<rows; i++) {
    for (unsigned j=0; j<cols; j++) {
      for (unsigned k=0; k<rows; k++) {
        result(i,j) += this->mat[i][k] * rhs(k,j);
      }
    }
  }

  return result;
}

// Cumulative left multiplication of this matrix and another
template<typename T>
TMatrix<T>& TMatrix<T>::operator*=(const TMatrix<T>& rhs) {
  TMatrix result = (*this) * rhs;
  (*this) = result;
  return *this;
}

// Calculate a transpose of this matrix
template<typename T>
TMatrix<T> TMatrix<T>::transpose() {
  TMatrix result(rows, cols, 0.0);

  for (unsigned i=0; i<rows; i++) {
    for (unsigned j=0; j<cols; j++) {
      result(i,j) = this->mat[j][i];
    }
  }

  return result;
}

// Matrix/scalar addition
template<typename T>
TMatrix<T> TMatrix<T>::operator+(const T& rhs) {
  TMatrix result(rows, cols, 0.0);

  for (unsigned i=0; i<rows; i++) {
    for (unsigned j=0; j<cols; j++) {
      result(i,j) = this->mat[i][j] + rhs;
    }
  }

  return result;
}

// Matrix/scalar subtraction
template<typename T>
TMatrix<T> TMatrix<T>::operator-(const T& rhs) {
  TMatrix result(rows, cols, 0.0);

  for (unsigned i=0; i<rows; i++) {
    for (unsigned j=0; j<cols; j++) {
      result(i,j) = this->mat[i][j] - rhs;
    }
  }

  return result;
}

// Matrix/scalar multiplication
template<typename T>
TMatrix<T> TMatrix<T>::operator*(const T& rhs) {
  TMatrix result(rows, cols, 0.0);

  for (unsigned i=0; i<rows; i++) {
    for (unsigned j=0; j<cols; j++) {
      result(i,j) = this->mat[i][j] * rhs;
    }
  }

  return result;
}

// Matrix/scalar division
template<typename T>
TMatrix<T> TMatrix<T>::operator/(const T& rhs) {
  TMatrix result(rows, cols, 0.0);

  for (unsigned i=0; i<rows; i++) {
    for (unsigned j=0; j<cols; j++) {
      result(i,j) = this->mat[i][j] / rhs;
    }
  }

  return result;
}

// Multiply a matrix with a vector 
template<typename T>
std::vector<T> TMatrix<T>::operator*(const std::vector<T>& rhs) {
  std::vector<T> result(rhs.size(), 0.0);

  for (unsigned i=0; i<rows; i++) {
    for (unsigned j=0; j<cols; j++) {
      result[i] = this->mat[i][j] * rhs[j];
    }
  }

  return result;
}

// Obtain a vector of the diagonal elements
template<typename T>
std::vector<T> TMatrix<T>::diag_vec() {
  std::vector<T> result(rows, 0.0);

  for (unsigned i=0; i<rows; i++) {
    result[i] = this->mat[i][i];
  }

  return result;
}

// Access the individual elements
template<typename T>
T& TMatrix<T>::operator()(const unsigned& row, const unsigned& col) {
  return this->mat[row][col];
}

// Access the individual elements (const)
template<typename T>
const T& TMatrix<T>::operator()(const unsigned& row, const unsigned& col) const {
  return this->mat[row][col];
}

// Get the number of rows of the matrix
template<typename T>
unsigned TMatrix<T>::get_rows() const {
  return this->rows;
}

// Get the number of columns of the matrix
template<typename T>
unsigned TMatrix<T>::get_cols() const {
  return this->cols;
}

} // end namespace IQmol
