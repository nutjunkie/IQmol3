#include <cassert>
#include <cstddef>
#include <iostream>
#include <type_traits>
#include <vector>

#include "Vector.h"

using IQmol::Vec3;

#define CHECK(cond) do {                                                     \
    if (!(cond)) {                                                           \
        std::cerr << "CHECK failed: " #cond "  at "                          \
                  << __FILE__ << ":" << __LINE__ << std::endl;               \
        std::abort();                                                        \
    }                                                                        \
} while (0)

void test_vec3() 
{
   Vec3 a(1,2,3);
   assert(a[0]==1 && a[1]==2 && a[2]==3);
   a[2] = 4; assert(a.z==4);
   assert(a.norm2()==1+4+16);
   double* p = a.data();
   p[0]=7; p[1]=8; p[2]=9;
   assert(a.x==7 && a.y==8 && a.z==9);
}


void test_vector_dot()
{
   IQmol::Array<1,double> a({3}); a(0)=1; a(1)=2; a(2)=3;
   IQmol::Array<1,double> b({3}); b(0)=4; b(1)=5; b(2)=6;
   assert(std::abs(IQmol::dot(a,b)-32.0) < 1e-14); 
}


void test_vec3_ops() 
{
   Vec3 a{1,0,0}, b{0,1,0}, c{0,0,1};
   CHECK(dot(a,a) == 1.0);
   CHECK(dot(a,b) == 0.0);
   CHECK(cross(a,b).x == 0 && cross(a,b).y == 0 && cross(a,b).z == 1);
   auto ab = cross(a,b);
   CHECK(dot(ab, a) == 0.0 && dot(ab, b) == 0.0);            // orthogonal
   auto bac = cross(b,a);
   CHECK(bac.x == -ab.x && bac.y == -ab.y && bac.z == -ab.z); // anti-commutative
   // scalar triple product == determinant
   CHECK(dot(a, cross(b, c)) == 1.0);
}


int main()
{
   test_vec3();
   test_vec3_ops(); 
   test_vector_dot(); 
   return 0;
}
