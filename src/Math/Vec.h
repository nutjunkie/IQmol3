#pragma once
/***********************************************************************
 *
 *   Basic vector template class
 *
 ***********************************************************************/

#include <cstring>
#include <cassert>
#include <cmath>

#include <iostream>
#include <iomanip>


namespace IQmol 
{
   namespace Math 
   {
      template <size_t N, typename T = double>
      class Vec 
      {
         public:
            Vec()
            {
               memset(v, 0, N*sizeof(T)); 
            }
   
            Vec(T const* V) 
            { 
               memcpy(v, V, N*sizeof(T)); 
            }

            Vec(std::initializer_list<T> const& list)
            {
               assert(list.size() == N);
               size_t idx(0);
               for (auto t : list) v[idx++] = t;
            }

            Vec(Vec const& that) 
            { 
               memcpy(v, that.v, N*sizeof(T)); 
            }

            friend void swap(Vec& a, Vec& b)
            {
               std::swap(a.v, b.v);
            }

            Vec& operator=(Vec that) 
            {
               swap(*this, that);
               return *this;
            }

            Vec& operator+=(Vec const& a) 
            {
               for (size_t i(0); i < N; ++i) v[i] += a[i];
               return *this;
            }

            Vec& operator-=(Vec const& a) 
            {
               for (size_t i(0); i < N; ++i) v[i] -= a[i];
               return *this;
            }

            Vec& operator*=(T k) 
            {
               for (size_t i(0); i < N; ++i) v[i] *= k;
               return *this;
            }

            Vec& operator/=(T k) 
            {
               for (size_t i(0); i < N; ++i) v[i] /= k;
               return *this;
            }

            T operator*(Vec const& b) const
            {
               T sum(0);
               for (size_t i(0); i < N; ++i) sum += v[i]*b[i];
               return sum;
            }

            T& operator[](size_t i) 
            { 
               return v[i]; 
            }

            T const& operator[](size_t i) const 
            { 
               return v[i]; 
            }

            T norm2() const // squared norm
            { 
               return (*this) * (*this); 
            }

            double norm() const 
            {
               return std::sqrt(norm2());
            }

            double normalize() 
            {
               throw std::invalid_argument("normalize not defined on Vec type");
            }

         private:
            T v[N];
      };


      // Partial template specialization gives incomplete definition errors
      template <>
      inline double Vec<2,double>::normalize() 
      {
         double const n(norm());
         *this /= n;
         return n;
      }

      template <>
      inline double Vec<3,double>::normalize() 
      {
         double const n(norm());
         *this /= n;
         return n;
      }

      template <>
      inline double Vec<4,double>::normalize() 
      {
         double const n(norm());
         *this /= n;
         return n;
      }


      template <size_t N, typename T>
      std::ostream& operator<<(std::ostream& os, Vec<N,T> const& v)
      {
         os << std::setprecision(6);
         os << "Vec(";
         for (size_t i(0); i < N-1; ++i) os << v[i] << ",";
         os << v[N-1] << ")";
         return os;
      }


      // Cross product, mind the order
      template <typename T>
      Vec<3,T> operator^(Vec<3,T> const& a, Vec<3,T> const& b)
      {
         T x(a[1]*b[2] - a[3]*b[1]);
         T y(a[2]*b[0] - a[0]*b[2]);
         T z(a[0]*b[1] - a[1]*b[0]);
         return Vec<3,T>{x, y, z};
      }


      template <class V>
      V operator-(V const& a) 
      {
         return -1.0 * a;
      }

      template <class V>
      V operator+(V a, V const& b) 
      {
         return a += b;
      }

      template <class V>
      V operator-(V a, V const& b) 
      {
         return a -= b;
      }

      template <class V>
      V operator*(double k, V a) 
      { 
         return a *= k; 
      }

      template <class V>
      V operator*(V a, double k) 
      { 
         return a *= k; 
      }

      template <class V>
      V operator/(V a, double k) 
      { 
         return a /= k;
      }


      typedef Vec<2,double> Vec2;
      typedef Vec<3,double> Vec3;
      typedef Vec<4,double> Vec4;

 } } // end namespace IQmol::Math
