#include <cassert>
#include <cstddef>
#include <iostream>
#include <type_traits>
#include <vector>

#include "../Array.h"

using IQmol::Array;

#define CHECK(cond) do {                                                     \
    if (!(cond)) {                                                           \
        std::cerr << "CHECK failed: " #cond "  at "                          \
                  << __FILE__ << ":" << __LINE__ << std::endl;               \
        std::abort();                                                        \
    }                                                                        \
} while (0)

// Reference row-major (last index contiguous) offset
template <size_t D>
static size_t row_major_offset(const std::array<size_t, D>& shape,
                               const std::array<size_t, D>& idx) {
    size_t stride = 1;
    size_t off = 0;
    for (size_t i = D; i-- > 0; ) { // D-1 .. 0
        off += idx[i] * stride;
        stride *= shape[i];
    }
    return off;
}

// Increment an Index lexicographically (last index fastest)
// returns false when rolled over past end
template <size_t D>
static bool next_index(const std::array<size_t, D>& shape,
                       std::array<size_t, D>& idx) {
    for (size_t i = D; i-- > 0; ) {
        if (++idx[i] < shape[i]) return true;
        idx[i] = 0;
    }
    return false;
}


int test_index_and_variadic_accesss() {
    // Return type sanity (variadic)
    static_assert(std::is_same<decltype(std::declval<Array<2,int>&>()(0,0)), int&>::value,
                  "non-const variadic operator() must return T&");
    static_assert(std::is_same<decltype(std::declval<const Array<2,int>&>()(0,0)), const int&>::value,
                  "const variadic operator() must return const T&");

    // ----- D = 1 -----
    {
        Array<1,int> a({5});
        for (size_t i=0; i<5; ++i) a(i) = int(i+1);

        for (size_t i=0; i<5; ++i) {
            typename Array<1,int>::Index idx{i};
            CHECK(a(idx) == a(i));
            CHECK(&a(idx) == &a(i));
        }

        // Last index contiguous (degenerate)
        CHECK(&a(0) + 1 == &a(1));
    }

    // ----- D = 2 -----
    {
        Array<2,int> a({2,3});

        // Fill via variadic operator()
        int v = 0;
        for (size_t i=0; i<2; ++i)
            for (size_t j=0; j<3; ++j)
                a(i,j) = ++v;

        // Variadic vs Index: values and addresses match
        for (size_t i=0; i<2; ++i)
            for (size_t j=0; j<3; ++j) {
                typename Array<2,int>::Index idx{i,j};
                CHECK(a(idx) == a(i,j));
                CHECK(&a(idx) == &a(i,j));
            }

        // Row-major / last-index contiguous
        CHECK(&a(0,0) + 1 == &a(0,1));
        CHECK(&a(0,1) + 1 == &a(0,2));
        CHECK(&a(0,2) + 1 == &a(1,0)); // wrap to next outer index

        // --- Index-only write/read path ---
        a.zero();
        // Write using Index (no variadics)
        {
            typename Array<2,int>::Index idx{0,0};
            int k = 1;
            do {
                a(idx) = k++;
            } while (next_index<2>(a.shape(), idx));
        }
        // Verify using Index only, with reference offset math
        {
            typename Array<2,int>::Index idx{0,0};
            int expected = 1;
            do {
                // value check
                CHECK(a(idx) == expected++);
                // address check vs row_major_offset
                auto off = row_major_offset<2>(a.shape(), idx);
                CHECK(&a(idx) == a.data() + off);
            } while (next_index<2>(a.shape(), idx));
        }

        // Const access
        const Array<2,int>& ca = a;
        CHECK(ca(1,2) == a(1,2));
        CHECK(&ca(1,2) == &a(1,2));
    }

    // ----- D = 3 -----
    {
        Array<3,int> a({3,2,4});
        // Fill with a recognizable pattern using Index-only access
        {
            typename Array<3,int>::Index idx{0,0,0};
            do {
                a(idx) = int(idx[0]*100 + idx[1]*10 + idx[2]);
            } while (next_index<3>(a.shape(), idx));
        }

        // Cross-check: Index vs variadic + address vs row_major_offset
        for (size_t i=0; i<3; ++i)
            for (size_t j=0; j<2; ++j)
                for (size_t k=0; k<4; ++k) {
                    typename Array<3,int>::Index idx{i,j,k};
                    int via_index = a(idx);
                    int via_var   = a(i,j,k);
                    CHECK(via_index == via_var);
                    auto off = row_major_offset<3>(a.shape(), idx);
                    CHECK(&a(idx) == a.data() + off);
                }

        // Last index contiguous along k
        CHECK(&a(0,0,0) + 1 == &a(0,0,1));
        CHECK(&a(0,0,1) + 1 == &a(0,0,2));
        CHECK(&a(0,0,2) + 1 == &a(0,0,3));
        CHECK(&a(0,0,3) + 1 == &a(0,1,0)); // then next j
        CHECK(&a(0,1,3) + 1 == &a(1,0,0)); // then next i

        // Copy/assign semantics preserve values (exercise both paths)
        Array<3,int> b = a;   // copy-ctor
        CHECK(b(2,1,3) == a(2,1,3));
        typename Array<3,int>::Index idx{2,1,3};
        CHECK(b(idx) == a(idx));

        Array<3,int> c;       // copy-assign
        c = b;
        CHECK(c(2,1,3) == b(2,1,3));
        CHECK(c(idx) == b(idx));
    }

    // ----- Non-owning view writes via Index -----
    {
        const size_t n0 = 2, n1 = 3;
        std::vector<double> buf(n0*n1, -1.0);

        Array<2,double> view({n0,n1}, buf.data()); // non-owning
        typename Array<2,double>::Index idx{1,2};
        view(idx) = 42.5;

        // Row-major with last index contiguous => offset = i*n1 + j
        CHECK(buf[1*n1 + 2] == 42.5);
        CHECK(view.data() == buf.data());
        CHECK(&view(idx) == &buf[1*n1 + 2]);
    }

    std::cout << "All variadic & Index operator() tests passed.\n";
    return 0;
}


static void test_row_major_outer_index_contiguous() 
{
    IQmol::Array<2,int> a({2,3});
    int val = 0;
    for (size_t i0=0;i0<2;++i0)
      for (size_t i1=0;i1<3;++i1)
        a(IQmol::Array<2,int>::Index{i0,i1}) = ++val;

    // last index contiguous: (0,0),(0,1),(0,2),(1,0) are adjacent in memory
    auto* p00 = &a(IQmol::Array<2,int>::Index{0,0});
    auto* p01 = &a(IQmol::Array<2,int>::Index{0,1});
    auto* p02 = &a(IQmol::Array<2,int>::Index{0,2});
    auto* p10 = &a(IQmol::Array<2,int>::Index{1,0});
    assert(p00+1==p01 && p01+1==p02 && p02+1==p10);
}


int test_array_slicing()
{
    using Matrix = Array<2,double>;
    using Cube   = Array<3,double>;

    // ---------- Matrix -> Vector (row slice) ----------
    {
        const std::size_t R = 3, C = 5;
        Matrix M({R, C});

        // Fill with a recognizable pattern: M(i,j) = 100*i + j
        for (std::size_t i = 0; i < R; ++i)
            for (std::size_t j = 0; j < C; ++j)
                M(i,j) = 100.0*i + j;

        // Non-const parent: writable row view
        auto row1 = M.slice(1);  // Array<1,double>
        CHECK(row1.shape()[0] == C);
        CHECK(row1.strides()[0] == 1);          // last axis contiguous → unit stride
        // Values and addresses match parent
        for (std::size_t j = 0; j < C; ++j) {
            CHECK(row1(j) == M(1,j));
            CHECK(&row1(j) == &M(1,j));
        }
        // Contiguity along the vector
        CHECK(&row1(0) + 1 == &row1(1));
        CHECK(&row1(1) + 1 == &row1(2));

        // Const parent: read-only row view
        const Matrix& Mc = M;
        auto row2 = Mc.slice(2); // Array<1,const double>
        static_assert(std::is_const<
            std::remove_reference_t<decltype(row2(0))>
        >::value, "slice(const) must return const element reference");
        // Still correct addresses/values
        for (std::size_t j = 0; j < C; ++j) {
            CHECK(row2(j) == Mc(2,j));
            CHECK(&row2(j) == &Mc(2,j));
        }
        // NOTE: attempting "row2(0) = ..." should not compile (const).
    }

    // ---------- Cube -> Matrix (fix x=i → Y×Z slice) ----------
    {
        const std::size_t NX = 2, NY = 3, NZ = 4;
        Cube A({NX, NY, NZ});

        // Fill: A(i,j,k) = 100*i + 10*j + k
        for (std::size_t i = 0; i < NX; ++i)
            for (std::size_t j = 0; j < NY; ++j)
                for (std::size_t k = 0; k < NZ; ++k)
                    A(i,j,k) = 100.0*i + 10.0*j + k;

        // Writable slice at i = 1 (drop axis 0) → shape {NY, NZ}
        auto yz = A.slice(1);     // Array<2,double>
        CHECK(yz.shape()[0] == NY);
        CHECK(yz.shape()[1] == NZ);

        // Strides are inherited from parent: for row-major (last index fastest)
        // A.strides() should be {NY*NZ, NZ, 1}; the slice keeps {NZ, 1}.
        CHECK(yz.strides()[1] == 1);
        CHECK(yz.strides()[0] == A.strides()[1]);

        // Values and addresses match
        for (std::size_t j = 0; j < NY; ++j)
            for (std::size_t k = 0; k < NZ; ++k) {
                CHECK(yz(j,k) == A(1,j,k));
                CHECK(&yz(j,k) == &A(1,j,k));
            }

        // Contiguity along last axis of the slice
        CHECK(&yz(0,0) + 1 == &yz(0,1));
        CHECK(&yz(0,1) + 1 == &yz(0,2));

/*      // Disable for read-only
        // Write-through
        yz(2,3) = -7.0;
        CHECK(A(1,2,3) == -7.0);
*/

        // Const parent slice
        const Cube& Ac = A;
        auto yz_c = Ac.slice(0);  // Array<2,const double>
        static_assert(std::is_const<
            std::remove_reference_t<decltype(yz_c(0,0))>
        >::value, "slice(const) must return const element reference");

        // Shape/stride sanity again
        CHECK(yz_c.shape()[0] == NY);
        CHECK(yz_c.shape()[1] == NZ);
        CHECK(yz_c.strides()[1] == 1);
        CHECK(yz_c.strides()[0] == Ac.strides()[1]);
    }

    std::cout << "All slice tests passed.\n";
    return 0;
}



int main()
{
   test_row_major_outer_index_contiguous();
   test_index_and_variadic_accesss();
   return 0;
}
