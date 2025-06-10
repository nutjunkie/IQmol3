#include <iostream>

using namespace std;

int main()
{
    for (unsigned L = 0; L <=5; ++L) {
        unsigned offset(0);
        cout << "L-Ordering for L = " << L << endl;

        for (int lx = L; lx >= 0; lx--) {
            for (int ly = L-lx; ly >= 0; ly--) {
                for (int lz = L-lx-ly; lz >= 0; lz--) {
                    if (lx+ly+lz == L) {
                       cout << "basis[" << offset << "] = {" ;
                       cout <<  lx << "," << ly << "," << lz << "};\n";
                       ++offset;
                   }  
              }   
            }   
        }
        cout << endl;

        cout << "K-Ordering for L = " << L << endl;
        offset = 0;
        //  This nested loop generates Cartesian components in K-order
        for(unsigned lz = 0, i = 0; lz <= L; lz++)
        for(unsigned ly = 0; ly <= L-lz; ly++, i++) {
            unsigned lx = L-lz-ly;
            cout << "basis[" << offset << "] = {" ;
            cout <<  lx << "," << ly << "," << lz << "};\n";
            ++offset;
        }
        cout << endl;


        // This is the loop structure claimed by:
        //    https://hpc.wm.edu/software/docs/libint/progman-2.0.3-stable.pdf
        // which is inconsistent with what is reported by:
        //    https://github.com/MolSSI/QCSchema/issues/45
        // See also https://github.com/evaleev/libint/wiki/using-modern-CPlusPlus-API
        cout << "LIBINT ordering for L = " << L << endl;

        offset = 0;
        for (int i = 0; i <= L; i++) {
            int lx = L - i; /* exponent of x */
            for (int j = 0; j <= i; j++) {
                int ly = i-j; /* exponent of y */
                int lz = j; /* exponent of z */
                cout << "basis[" << offset << "] = {" ;
                cout <<  lx << "," << ly << "," << lz << "};\n";
                ++offset;
            }
        }
        cout << endl;
    }

    return 0;
}
