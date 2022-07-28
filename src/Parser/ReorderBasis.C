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

#include <string.h>
#include "Util/QsLog.h"


namespace IQmol {

// In-place reordering of
void ReorderMOs(double *C, QList<unsigned> const& map, unsigned nBasis, unsigned nOrb)
{
   double *tmp = new double[nBasis*nOrb];

   memcpy(tmp,C,nBasis*nOrb*sizeof(double))

   for (size_t mu=0; mu < nBasis; ++mu) {
       for (size_t i=0; i < nOrb; ++i) {
           C[i*nBasis+mu] = tmp[i*nBasis+map[mu]];
       }
   }

   delete [] tmp;
}


void ReorderMatrix(double *M, QList<unsigned> const& map)
{
   size_t n(map.size());
   double *tmp = new double[n*n];

   memcpy(tmp,M,n*n*sizeof(double))

   for (size_t i = 0; i < n; ++i) {
       for (size_t j = 0; j < n; ++j) {
           M[i*n+j] = tmp[ map[i]*n + map[j] ];
       }
   }

   delete [] tmp;
}


QList<unsigned> ReorderQChemToFchk(QList<int> const& shellTypes)
{
   QList<unsigned> index;
   unsigned nShells(shellTypes.size());

   unsigned iBasis(0);

   for (auto iter = shellTypes.begin(); iter != shellTypes.end(); ++iter) {

       int  L(*iter);
       bool pure(L < -1);

       if (pure) {

          L = std::abs(L);
          index.append(iBasis+L); // m = 0
          for (unsigned m = 1; m <= L; m++) {
              index.append(iBasis + L+m);
              index.append(iBasis + L-m);
          }
          iBasis += 2*L+1;

       }else {

          switch (L) {

             case -1: {
	            index.append(iBasis  );
	            index.append(iBasis+1);
	            index.append(iBasis+2);
	            index.append(iBasis+3);
	            iBasis += 4;
             } break;

             case 0: {
                index.append(iBasis);
                iBasis += 1;
             } break;

             case 1: {
	            index.append(iBasis  );
	            index.append(iBasis+1);
	            index.append(iBasis+2);
	            iBasis += 3;
             } break;

             case 2: {
	            index.append(iBasis  ); // dxx
	            index.append(iBasis+2); // dyy
	            index.append(iBasis+5); // dzz
	            index.append(iBasis+1); // dxy
	            index.append(iBasis+3); // dxz
	            index.append(iBasis+4); // dyz
	            iBasis += 6;
             } break;
   
             case 3: {
                index.append(iBasis  ); // fxxx 
                index.append(iBasis+3); // fyyy
                index.append(iBasis+9); // fzzz
                index.append(iBasis+2); // fxyy
                index.append(iBasis+1); // fxxy
                index.append(iBasis+4); // fxxz
                index.append(iBasis+7); // fxzz
                index.append(iBasis+8); // fyzz
                index.append(iBasis+6); // fyyz
                index.append(iBasis+5); // fxyz
                iBasis += 10;
             } break;

             case 4: {
                index.append(iBasis   ); // gxxxx
                index.append(iBasis+ 4); // gyyyy
                index.append(iBasis+14); // gzzzz
                index.append(iBasis+ 1); // gxxxy
                index.append(iBasis+ 5); // gxxxz
                index.append(iBasis+ 3); // gxyyy
                index.append(iBasis+ 8); // gyyyz
                index.append(iBasis+12); // gxzzz
                index.append(iBasis+13); // gyzzz
                index.append(iBasis+ 2); // gxxyy
                index.append(iBasis+ 9); // gxxzz
                index.append(iBasis+11); // gyyzz
                index.append(iBasis+ 6); // gxxyz
                index.append(iBasis+ 7); // gxyyz
                index.append(iBasis+10); // gxyzz
                iBasis += 15;
             } break;
   
             case -5: {  // This ordering is suspicious, so skip it
                index.append(iBasis+20); // hzzzzz
                index.append(iBasis+19); // hyzzzz
                index.append(iBasis+17); // hyyzzz
                index.append(iBasis+14); // hyyyzz
                index.append(iBasis+10); // hyyyyz
                index.append(iBasis+ 5); // hyyyyy
                index.append(iBasis+18); // hxzzzz
                index.append(iBasis+16); // hxyzzz
                index.append(iBasis+13); // hxyyzz
                index.append(iBasis+ 9); // hxyyyz
                index.append(iBasis+ 4); // hxyyyy
                index.append(iBasis+15); // hxxzzz
                index.append(iBasis+12); // hxxyzz
                index.append(iBasis+ 8); // hxxyyz
                index.append(iBasis+ 3); // hxxyyy
                index.append(iBasis+11); // hxxxzz
                index.append(iBasis+ 7); // hxxxyz
                index.append(iBasis+ 2); // hxxxyy
                index.append(iBasis+ 6); // hxxxxz
                index.append(iBasis+ 1); // hxxxxy
                index.append(iBasis   ); // hxxxxx
                iBasis += 21;
             } break;

             default: {
                // If we can't do the reordering, just leave the basis functions in the 
                // same order and issue a warning.
                QLOG_WARN() << "Unable to reorder cartesian basis functions for L =" << L;
                unsigned n( ((L+1)*(L+2))/2);
                for (unsigned i = 0; i < n; ++i) {
                    index.append(iBasis+i);
                }
                iBasis += n;       
             } break;

          }
       }
   }

   return index;
}

} // namespace IQmol
