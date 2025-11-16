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

#include <array>
#include <cassert>
#include <cstring>


namespace IQmol {

/** \brief Provides a mutable view of array data.  Note that if a pointer is passed
           to the ctor, the data are not owned and are not deleted on destruction.
        
    \param D the rank of the Array
    \param T the type of data stored in the Array

    \usage Use the Shape typedef to give the size of each dimension:

           Array<2>::Shape shape = { 5, 6 };
           Array<2> array(shape);

           Use the Index typedef for indexing elements of the array, (note data
           is stored row-major):

           Array<2>::Index idx;

           for (idx[0] = 0; idx[0] < shape[0]; ++idx[0]) {
               for (idx[1] = 0; idx[1] < shape[1]; ++idx[1]) {
                   array(idx) = 0.0;
               }
           }

           array(3,4) = 3.1415;
 **/


// Array operations factored into a base.  
// Requirements on class A: size(), operator[](i) const/non-const
template<class A>
class ArrayOps 
{
   public:
      // Downcast helpers
      A&       self()       { return static_cast<A&>(*this); }
      const A& self() const { return static_cast<const A&>(*this); }

      // Compound assignments (single source of truth)
      A& operator+=(const A& rhs) {
          auto& a = self();
          assert(a.size() == rhs.size());
          for (size_t i = 0; i < a.size(); ++i) a[i] += rhs[i];
          return a;
      }
      A& operator-=(const A& rhs) {
          auto& a = self();
          assert(a.size() == rhs.size());
          for (size_t i = 0; i < a.size(); ++i) a[i] -= rhs[i];
          return a;
      }

      template<class T>
      A& operator*=(T t) {
          auto& a = self();
          for (size_t i = 0; i < a.size(); ++i) a[i] *= t;
          return a;
      }

      template<class T>
      A& operator/=(T t) {
          auto& a = self();
          for (size_t i = 0; i < a.size(); ++i) a[i] /= t;
          return a;
      }

      // Unary and binary operators, built from the compound ops and implicit copy
      friend A operator-(A arr)               { arr *= -1.0; return arr; }
      friend A operator+(A lhs, const A& rhs) { lhs += rhs;  return lhs; }
      friend A operator-(A lhs, const A& rhs) { lhs -= rhs;  return lhs; }

      template<class T> 
      friend A operator*(A lhs, T t) { lhs *= t; return lhs; }

      template<class T> 
      friend A operator*(T t, A rhs) { rhs *= t; return rhs; }

      template<class T> 
      friend A operator/(A lhs, T t) { lhs /= t; return lhs; }
};


template < size_t D, typename T = double >
class Array : public ArrayOps<Array<D,T>>
{
   public:
      using Shape = std::array<size_t, D>;
      using Index = std::array<size_t, D>;

      static Shape strides(Shape const& shape)
      {
        Shape s{}; 
        size_t stride = 1;
        for (size_t i = D; i-- > 0;) {   // i = D-1, D-2, ..., 0
            s[i] = stride;
            stride *= shape[i];
        }
        return s;
      }

      Array(Array const& array) : 
         m_data(0), 
         m_shape(array.m_shape), 
         m_strides(array.m_strides),
         m_ownData(true)
      {
         reallocate(size());
         copyData(array);
      }

      Array(Shape const& shape = Shape(), T* data = nullptr) : 
         m_data(data), 
         m_shape(shape), 
         m_strides(strides(shape)), 
         m_ownData(m_data == nullptr)
      { 
         if (m_ownData) reallocate(size());
      }

      ~Array() 
      { 
         if (m_ownData && m_data) delete[] m_data;
      }

      // Initializes the Array buffer to zero
      void zero() 
      { 
         if (m_data) memset(m_data, 0, size()*sizeof(T)); // Assumes T is trivially copyable
      }

      Array& operator=(Array const& that) 
      {
          if (this != &that) {
             if (size() != that.size()) reallocate(that.size());
             m_shape = that.m_shape;
             m_strides = that.m_strides;
             copyData(that);
          }
          return *this;
      }

      T& operator[](size_t i) 
      { 
         assert(i < size());
         return m_data[i]; 
      }

      T const& operator[](size_t i) const 
      { 
         assert(i < size());
         return m_data[i]; 
      }

      // Index access
      T& operator()(Index const& d) 
      {
         return m_data[offset_from_index(d, std::make_index_sequence<D>{})];
      }

      T const& operator()(const Index& d) const 
      {
         return m_data[offset_from_index(d, std::make_index_sequence<D>{})];
      }

      // The following variadic operator allow for efficient element access
      // using  M(i,j) rather than Index types
      template<class... Is, typename = std::enable_if_t<sizeof...(Is) == D>>
      T& operator()(Is... is) {
          const size_t c[D] = { static_cast<size_t>(is)... };
          return m_data[offset_from_coords(c, std::make_index_sequence<D>{})];
      }

      template<class... Is, typename = std::enable_if_t<sizeof...(Is) == D>>
      const T& operator()(Is... is) const {
          const size_t c[D] = { static_cast<size_t>(is)... };
          return m_data[offset_from_coords(c, std::make_index_sequence<D>{})];
      }

      void resize(Shape const & shape)
      {
         size_t newSize = size(shape);
         size_t oldSize = size(m_shape);

         if (newSize != oldSize) reallocate(newSize);

         m_shape   = shape;
         m_strides = strides(shape);
      }

      size_t rank() const { return D; }
      size_t dimension(size_t const n) const { return (n < D ? m_shape[n] : 0); }
      size_t size() const { return size(m_shape); }
      Shape  const& shape() const { return m_shape; }
      Shape  const& strides() const { return m_strides; }

      T*  data() { return m_data; }
      T const* data() const { return m_data; }

/*
      // Creates a read-only view of an array slice
      Array<D-1, const T> slice(size_t i) const&
      {
         static_assert(D >= 2, "slice() requires D >= 2");
         assert(i < m_shape[0]);

         using Child = Array<D-1, const T>;
         typename Child::Shape cshape{}, cstrides{};

         // Keep axes 1..D-1 (same order), copy shapes/strides.
         for (size_t k = 1, o = 0; k < D; ++k, ++o) {
             cshape[o]   = m_shape[k];
             cstrides[o] = m_strides[k];
         }

         const T* ptr = m_data + i * m_strides[0];
         return Child::make_view(cshape, ptr, cstrides);
      }
*/

      // Read/Write version
      Array<D-1, T> const slice(size_t i) const& 
      {
         static_assert(D >= 2, "slice() requires D >= 2");
         assert(i < m_shape[0]);

         using Child = Array<D-1, T>;
         typename Child::Shape cshape{}, cstrides{};

         for (size_t k = 1, o = 0; k < D; ++k, ++o) { 
             cshape[o]=m_shape[k]; 
             cstrides[o]=m_strides[k]; 
         }

         T* ptr = m_data + i * m_strides[0];
         return Child::make_view(cshape, ptr, cstrides);
      }

      // prevent dangling view from temporaries
      Array<D-1, T> slice(size_t) && = delete;

      // Non-owning view for slicing
      static Array const make_view(const Shape& shape, T* data, const Shape& custom_strides) 
      {
         Array v;
         v.m_data    = data;
         v.m_shape   = shape;
         v.m_strides = custom_strides;
         v.m_ownData = false;
         return v;
      }

   protected:
      T*     m_data = nullptr;  
      Shape  m_shape{};
      Shape  m_strides{}; // used for computing offsets into m_data
      bool   m_ownData;

   private:
      static size_t size(Shape const& shape)
      {
         size_t n(1);
         for (size_t d : shape) n *= d;
         return n;
      }

      size_t offset(Index const& d) const
      {
         size_t idx(0);
         for (size_t i = 0; i < D; ++i) {
             assert(d[i] < m_shape[i]);
             idx += d[i]*m_strides[i];
         }
         return idx;
      }

      void reallocate(size_t const length)
      {
         assert(m_ownData && "Array::reallocate called on non-owning data");  //only allow resize if we own the data
         if (m_data) delete[] m_data;
         m_data = (length == 0) ? nullptr : new T[length];
      }

      void copyData(Array const& that) 
      {
        // Assumes T is trivially copyable
         memcpy(m_data, that.data(), size()*sizeof(T));
      }

      // The following helper functions allow for efficient indexing
      template<size_t... I>
      size_t offset_from_index(const Index& d, std::index_sequence<I...>) const 
      {
         size_t idx = 0;
         [[maybe_unused]] int tmp[] = { 0, (assert(d[I] < m_shape[I]), idx += d[I] * m_strides[I], 0)... };
         return idx;
      }

      template<size_t... I>
      size_t offset_from_coords(const size_t (&c)[D], std::index_sequence<I...>) const 
      {
         size_t idx = 0;
         [[maybe_unused]] int tmp[] = { 0, (assert(c[I] < m_shape[I]), idx += c[I] * m_strides[I], 0)... };
         return idx;
      }
};

} // end namespace
