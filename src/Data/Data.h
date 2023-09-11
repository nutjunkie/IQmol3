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

#include "Serialization.h"
#include <boost/serialization/version.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>


// The Data namespace contains POD classes that are read in from files and
// forms the interface between the Parsers and Layers.

namespace IQmol {

namespace Data {

   typedef boost::archive::text_iarchive InputArchive;
   typedef boost::archive::text_oarchive OutputArchive;

   namespace Type {

      enum ID { Undefined = 0, 
               /*---------------------  *---------------------  *--------------------- */
                Atom,                   AtomList,               Bank, 
                QmAtom,                 QmAtomList,             MacroMolecule,
                MmAtom,                 MmAtomList,             Group,
                Pdb,                    ProteinChain,           Solvent,
                PointCharge,            PointChargeList,        EfpFragment, 
                EfpFragmentList,        EfpFragmentLibrary,     GridData, 
                GridDataList,           File,                   FileList, 
                Geometry,               GeometryList,           RemSection,
                ChargeMultiplicity,     CubeData,
                Energy,                 ConformerEnergy,        Hessian,
                ScfEnergy,              TotalEnergy,            ForceFieldEnergy,
                DipoleMoment,           VibrationalMode,        VibrationalModeList, 
                PointGroup,             Frequencies,            
                Orbitals,               OrbitalsList,           LocalizedOrbitals, 
                CanonicalOrbitals,      NaturalBondOrbitals,    NaturalTransitionOrbitals,
                DysonOrbitals,
                Density,                DensityList,
                Shell,                  ShellList,              Mesh,
                MeshList,               Surface,                SurfaceList,
                SurfaceInfo,            SurfaceInfoList,        SurfaceType, 
               /*---------------------  *---------------------  *--------------------- */
                Constraint,             PositionConstraint,     DistanceConstraint,
                AngleConstraint,        TorsionConstraint,      FrozenAtomsConstraint,
               /*---------------------  *---------------------  *--------------------- */
                AtomicProperty,         AtomicSymbol,           AtomColor, 
                AtomicNumber,
                NmrShielding,           NmrShift,               Mass, 
                MultipoleExpansion,     MullikenCharge,         MultipoleDerivedCharge, 
                AtomicCharge,           GasteigerCharge,        ChelpgCharge,
                HirshfeldCharge,        LowdinCharge,           NaturalCharge,
                MerzKollmanRespCharge,  MerzKollmanEspCharge,   Cm5Charge,
                SpinDensity,            VdwRadius,              MultipoleExpansionList,
                NmrReference,           NmrReferenceList,       Nmr,
                ExcitedStates,          ElectronicTransition,   ElectronicTransitionList, 
                OrbitalSymmetries,      Vibronic,               VibronicSpectrum,
               /*---------------------  *---------------------  *--------------------- */
                YamlNode,               PovRay,                 GeminalOrbitals,
               /*---------------------  *---------------------  *--------------------- */
                AminoAcid,              AminoAcidList,          Residue
      };

      QString toString(ID const);
   }

   /// Base class for all data classes that can be serialized.
   class Base 
   {
      public:
         virtual ~Base() {  }

		 // Returns a portable type identification for the Data::Factory.  New
		 // Data classes must have their TypeID added to the Factory::create()
		 // method before they can be used by the Factory.
		 virtual Type::ID typeID() const { return Type::Undefined; }

		 // These can't be template functions as templates and virtual
		 // functions don't play nicely together.
         virtual void serialize(InputArchive&,  unsigned int const version = 0) { };
         virtual void serialize(OutputArchive&, unsigned int const version = 0) { };

         /// This is only meant for debugging;
		 virtual void dump() const { };
   };

} } // end namespace IQmol::Data
