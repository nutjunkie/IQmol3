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

//using namespace libarchive::impl;


namespace IQmol {

namespace Data {
   class Geometry;
   class ShellData;
   class OrbitalData;
   class DensityList;
   class Frequencies;
   class PointChargeList;
}


namespace Schema {
   typedef libarchive::impl::schema::is_jobtype Job;
   typedef std::vector<Job> JobList;

   typedef libarchive::impl::schema::job::sp SinglePoint;
   typedef std::vector<SinglePoint> SinglePointList;

   typedef libarchive::impl::schema::atomic_charges_theory AtomicChargesTheory;
   typedef libarchive::impl::schema::molecular_orbital_type MolecularOrbitalType;

   typedef SinglePoint::structure Structure;
   typedef SinglePoint::aobasis   AOBasis;

   typedef SinglePoint::energy_function EnergyFunction;
   typedef std::vector<EnergyFunction> EnergyFunctionList;

   typedef EnergyFunction::observables Observables;
   typedef EnergyFunction::analysis    Analysis;
   typedef EnergyFunction::method::scf::molecular_orbitals MolecularOrbitals;

   typedef Observables::multipole_moments MultipoleMoments;
   typedef Observables::nmr_shieldings NmrShieldings;

   typedef Analysis::vibrational Vibrational;
   typedef std::vector<Vibrational> FrequenciesList;
   typedef Vibrational::thermodynamics Thermodynamics;

   typedef Analysis::atomic_charges AtomicCharges;
   typedef std::vector<AtomicCharges> AtomicChargesList;
}


namespace Parser {

   /// Parser for YAML files.
   class Archive : public Base {

      public:
         bool parseFile(QString const& filePath);
         // This should inherit the base-class definition.
         bool parse(TextStream&) { return false; } 

      private:
         Data::Geometry* readStructure(Schema::SinglePoint&);
         Data::PointChargeList* readExternalCharges(Schema::SinglePoint&);
         Data::Frequencies* readVibrationalData(Schema::Vibrational&);

         void readObservables(Schema::Observables&, Data::Geometry&);
         void readAnalysis(Schema::Analysis&, Data::Geometry&);

         void readShellData(Schema::SinglePoint&, Data::ShellData&);
         void readOrbitalData(Schema::EnergyFunction&, size_t nbasis, Data::OrbitalData&);
         void readDensityMatrix(Schema::EnergyFunction&, size_t nbasis, Data::DensityList&);

         void readAtomicCharges(Schema::AtomicCharges&, Data::Geometry&);
   };

} } // end namespace IQmol::Parser

#endif
