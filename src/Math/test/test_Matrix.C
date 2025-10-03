#include <cassert>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <type_traits>
#include <vector>

#include "Matrix.h"

using IQmol::Array;

#define CHECK(cond) do {                                                     \
    if (!(cond)) {                                                           \
        std::cerr << "CHECK failed: " #cond "  at "                          \
                  << __FILE__ << ":" << __LINE__ << std::endl;               \
        std::abort();                                                        \
    }                                                                        \
} while (0)


void test_matrix_multiply()
{
    using IQmol::Array;
    using IQmol::product;

    Array<2,double> A({2,3});
    Array<2,double> B({3,2});

    // A = [ [1,2,3],
    //       [4,5,6] ]
    A(0,0)=1; A(0,1)=2; A(0,2)=3;
    A(1,0)=4; A(1,1)=5; A(1,2)=6;

    // B = [ [7, 8],
    //       [9,10],
    //       [11,12] ]
    B(0,0)=7;  B(0,1)=8;
    B(1,0)=9;  B(1,1)=10;
    B(2,0)=11; B(2,1)=12;

    auto C = product(A,B);
    // Expected C = A*B = [ [58, 64],
    //                      [139,154] ]
    CHECK(C.shape()[0] == 2 && C.shape()[1] == 2);
    CHECK(C(0,0) == 58.0);
    CHECK(C(0,1) == 64.0);
    CHECK(C(1,0) == 139.0);
    CHECK(C(1,1) == 154.0);

    // address sanity: row-major (last index contiguous)
    CHECK(&C(0,0) + 1 == &C(0,1));
    CHECK(&C(0,1) + 1 == &C(1,0));
}


void test_print_matrix()
{
    size_t const m(6);
    size_t const n(12);

    IQmol::Matrix X({m,n});

    for (size_t i = 0; i < m; ++i) {
        for (size_t j = 0; j < n; ++j) {
            X(i,j) = 1.000000001+ i + 0.01*(j+1);
        }
    }

    std::cout << "Printing product matrix (default):\n";
    std::cout << X;

    std::cout << "Printing product matrix ():\n";
    std::cout << IQmol::setcols(12) << std::setprecision(4) << std::setw(8) ;
    std::cout << X;
}


int main()
{
   test_matrix_multiply();
   test_print_matrix();
   return 0;
}
