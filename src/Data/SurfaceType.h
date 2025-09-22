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

#include "Data.h"


namespace IQmol {
namespace Data {

   class SurfaceType : public Base {

      public:
         enum Kind { Custom = -1, AlphaOrbital, BetaOrbital, TotalDensity, 
            SpinDensity, AlphaDensity, BetaDensity, DensityCombo, CubeData, 
            VanDerWaals, Promolecule, SolventExcluded, SID, ElectrostaticPotential,
            Geminal, Correlation, CustomDensity, BasisFunction, DysonLeft, DysonRight,
            MullikenAtomic, MullikenDiatomic, GenericOrbital, Ribbon

// TODO
//            AlphaHole Density, BetaHole Density,
//            AlphaExcitationDensity, BetaExcitationDensity,
//            AlphaAttachmentDensity, BetaAttachmentDensity,
//            AlphaDetachmentDensity, BetaDetachmentDensity
         };

		 // Not really units, but indicates how any unit conversion should be
		 // handled.
         enum Units { Orbital, Volume };

         SurfaceType(int const kind);

         SurfaceType(Kind const kind = Custom, unsigned index = 0) 
          : m_kind(kind), m_index(index) { }

         QString toString() const;

         Kind const& kind() const { return m_kind; }
         void setKind(Kind const kind) { m_kind = kind; }

         Units units() const;

         // used for custom surfaces
         QString const& label() const { return m_label; }
         void setLabel(QString const& label) { m_label = label; }

         unsigned const& index() const { return m_index; }
         void setIndex(unsigned const index) { m_index = index; }

         bool isDensity() const;
         bool isRegularDensity() const;
         bool isOrbital() const;
         bool isBasis() const;
         bool isSigned() const;
         bool isIndexed() const;

         bool operator==(SurfaceType const& that) const;

         bool operator!=(SurfaceType const& that) const
         {
            return !(*this == that);
         }

         bool operator<(SurfaceType const& that) const
         {
            return (m_kind == that.m_kind) ? (m_index < that.m_index)
                                           : (m_kind  < that.m_kind); 
         }

         void dump() const;

      private:
         Kind m_kind;
         unsigned m_index;
         QString m_label;
   };

} } // end namespace IQmol::Data
