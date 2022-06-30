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

#include "ArchiveParser.h"
#include "Util/QsLog.h"
#include "Util/Constants.h"
#include <libarchive/api/impl/qarchive.h>

#include <QFile>
#include <QDebug>

#include "Data/GeometryList.h"


namespace IQmol {
namespace Parser {


Data::Geometry* Archive::readGeometry(schema::job::sp& sp)
{
   Data::Geometry* geometry(0);
   auto structure = sp.add_layer<schema::job::sp::structure>();

   size_t natoms, nalpha, nbeta;
   int charge;
   structure.read(schema::job::sp::structure::natoms, natoms);
   structure.read(schema::job::sp::structure::nalpha, nalpha);
   structure.read(schema::job::sp::structure::nbeta,  nbeta);
   structure.read(schema::job::sp::structure::charge, charge);

   unsigned multiplicity = nalpha >= nbeta ? nalpha - nbeta + 1 : nbeta - nalpha + 1;

   std::vector<int> nuclei(natoms);
   std::vector<double> coords(3*natoms);

   libaview::array_view<double> av_coords(&coords[0],3*natoms);
   libaview::tens2<double> tens_coords(av_coords, 3, natoms);

   structure.read(schema::job::sp::structure::nuclei, nuclei);
   structure.read(schema::job::sp::structure::coordinates, tens_coords);

   qDebug() << "number of atoms in structure" << natoms;

   std::vector<unsigned> nuclei_std(natoms);
   for (size_t i = 0; i < natoms; ++i) {
       qDebug() << "Coordinates" << nuclei[i] << coords[3*i+0] << coords[3*i+1] << coords[3*i+2];
       nuclei_std[i] = nuclei[i];
   }

   geometry = new Data::Geometry(nuclei_std, coords);
   geometry->scaleCoordinates(Constants::BohrToAngstrom);
   geometry->setChargeAndMultiplicity(charge, multiplicity);

   return geometry;
}


bool Archive::readShellData(schema::job::sp& sp)
{
   //Data::ShellData shellData;
   auto aobasis = sp.add_layer<schema::job::sp::aobasis>();

   std::vector<int> shell_types;
   aobasis.read(schema::job::sp::aobasis::shell_types, shell_types);

   std::vector<size_t> shell_to_atom_map;
   aobasis.read(schema::job::sp::aobasis::shell_to_atom_map, shell_to_atom_map);
   
   std::vector<size_t> primitives_per_shell;
   aobasis.read(schema::job::sp::aobasis::primitives_per_shell, primitives_per_shell);

   std::vector<double> primitive_exponents;
   aobasis.read(schema::job::sp::aobasis::primitive_exponents, primitive_exponents);

   std::vector<double> contraction_coefficients;
   aobasis.read(schema::job::sp::aobasis::contraction_coefficients, contraction_coefficients);

   std::vector<double> sp_ontraction_coefficients;
   aobasis.read(schema::job::sp::aobasis::sp_contraction_coefficients, sp_ontraction_coefficients);

   
/*
   struct ShellData {
      QList<int>      shellTypes;
      QList<unsigned> shellToAtom;
      QList<unsigned> shellPrimitives;
      QList<double>   exponents;
      QList<double>   contractionCoefficients;
      QList<double>   contractionCoefficientsSP;
      QList<double>   overlapMatrix;
      unsigned        nBasis;   // recomputed once the ShellList has been formed
   };
*/
}



bool Archive::parseFile(QString const& filePath)
{
   bool ok(true);

   try {

      libarchive::impl::QArchive archive(filePath.toStdString(), libstore::FileMode::ReadOnly);

      typedef std::vector<schema::is_jobtype> JobList;
      typedef std::vector<schema::job::sp> SPList;

      JobList jobList = archive.get_jobs();
      JobList::iterator jobIter;

      for (size_t j(0); j < jobList.size(); ++j) {

          QString label("Job " + QString::number(j));
          Data::GeometryList* geometryList(new Data::GeometryList(label));

          SPList spList = archive.get_single_points(jobList[j]);

          SPList::iterator sp;
          for (sp = spList.begin(); sp != spList.end(); ++sp) {
              Data::Geometry* geometry = readGeometry(*sp);
              if (!geometry) continue;
              geometryList->append(geometry);
              readShellData(*sp);
          }

          if (!geometryList->isEmpty()) m_dataBank.append(geometryList);

         
      }

   } catch (std::exception& e) {
      m_errors.append(e.what());
      return false;
   }

   return ok;

}


} } // end namespace IQmol::Parser
