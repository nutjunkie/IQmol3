#ifndef IQMOL_PARSER_ARCHIVE_H
#define IQMOL_PARSER_ARCHIVE_H
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

#include "Parser.h"
#include <libarchive/api/impl/qarchive.h>


namespace IQmol {

namespace Data {
   class Geometry;
   class ShellData;
   class OrbitalData;
   class OrbitalsList;
   class DensityList;
   class Frequencies;
   class PointChargeList;
}



namespace Schema {
   typedef libarchive::impl::schema::is_jobtype Job;
   typedef libarchive::impl::schema::job::sp SinglePoint;
   typedef libarchive::impl::schema::atomic_charges_theory AtomicChargesTheory;
   typedef libarchive::impl::schema::molecular_orbital_type MolecularOrbitalType;

   typedef SinglePoint::structure Structure;
   typedef SinglePoint::aobasis   AOBasis;
   typedef SinglePoint::energy_function EnergyFunction;

   typedef Structure::external_charges ExternalCharges;

   typedef EnergyFunction::observables Observables;
   typedef EnergyFunction::analysis    Analysis;
   typedef EnergyFunction::method::scf::molecular_orbitals MolecularOrbitals;

   typedef Observables::multipole_moments MultipoleMoments;
   typedef Observables::nmr_shieldings NmrShieldings;

   typedef Analysis::vibrational Vibrational;
   typedef Analysis::atomic_charges AtomicCharges;
   typedef Analysis::localized_orbitals::molecular_orbitals  LocalizedOrbitals;

   typedef Vibrational::thermodynamics Thermodynamics;
}


namespace Parser {

   class Archive : public Base {

      public:
         bool parseFile(QString const& filePath);
         // This should inherit the base-class definition.
         bool parse(TextStream&) { return false; } 

      private:
         void readGeometry(Schema::Structure&, Data::Geometry&);
         void readExternalCharges(Schema::Structure&, Data::PointChargeList&);

         void readVibrationalData(Schema::Vibrational&, Data::Frequencies&);
         void readObservables(Schema::Observables&, Data::Geometry&);
         void readAnalysis(Schema::Analysis&, Data::Geometry&);
         void readShellData(Schema::SinglePoint&, Data::ShellData&);

         void readOrbitalData(Schema::MolecularOrbitals&, Data::ShellData const&, Data::OrbitalData&);
         void readOrbitalData(Schema::LocalizedOrbitals&, Data::ShellData const&, Data::OrbitalData&);
         void readDensityMatrix(Schema::EnergyFunction&, Data::ShellData const&, Data::DensityList&);
         void readLocalizedOrbitals(Schema::Analysis&, Data::ShellData const&, Data::Geometry const&, 
            Data::OrbitalsList&);

         void readAtomicCharges(Schema::AtomicCharges&, Data::Geometry&);
   };

} } // end namespace IQmol::Parser

#endif
