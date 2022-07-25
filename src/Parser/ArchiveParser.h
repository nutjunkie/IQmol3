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

using namespace libarchive::impl;


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
   typedef std::vector<libarchive::impl::schema::is_jobtype> JobList;
   typedef libarchive::impl::schema::job::sp::energy_function EnergyFunction;


   typedef EnergyFunction::observables Observables;
   typedef EnergyFunction::analysis    Analysis;

   typedef Observables::multipole_moments MultipoleMoments;
   typedef Observables::nmr_shieldings NmrShieldings;

   typedef Analysis::vibrational Vibrational;
   typedef std::vector<Vibrational> FrequenciesList;
   typedef std::vector<Analysis::atomic_charges> AtomicChargesList;
}

namespace Parser {

   /// Parser for YAML files.
   class Archive : public Base {

      public:
         bool parseFile(QString const& filePath);
         // This should inherit the base-class definition.
         bool parse(TextStream&) { return false; } 

      private:
         Data::Geometry* readStructure(schema::job::sp&);
         Data::PointChargeList* readExternalCharges(schema::job::sp&);

         Data::Frequencies* readVibrationalData(Schema::Vibrational&);

         void readShellData(schema::job::sp&, Data::ShellData&);
         void readOrbitalData(schema::job::sp::energy_function&, size_t nbasis, 
            Data::OrbitalData&);
         void readDensityMatrix(schema::job::sp::energy_function&, size_t nbasis,
            Data::DensityList&);
         void readAtomicCharges(schema::job::sp::energy_function::analysis::atomic_charges&, 
            Data::Geometry&);

         void readObservables(Schema::Observables&, Data::Geometry&);
         void readAnalysis(Schema::Analysis&, Data::Geometry&);
   };

} } // end namespace IQmol::Parser

#endif
