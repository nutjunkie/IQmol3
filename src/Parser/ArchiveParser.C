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
#include "Data/GeometryList.h"
#include "Data/OrbitalFactory.h"

#include <QFile>
#include <QDebug>

template<typename T>
QList<T> toQList(std::vector<T> vec)
{
   return QList<T>::fromVector(QVector<T>(vec.begin(), vec.end()));
}


QList<unsigned> toQList(std::vector<unsigned long> vec)
{
   return QList<unsigned>::fromVector(QVector<unsigned>(vec.begin(), vec.end()));
}


template<typename T>
QList<T> toQList(T const* start, size_t const n)
{
   return QList<T>(start, start+n);
}




namespace IQmol {
namespace Parser {

Data::Geometry* Archive::readGeometry(schema::job::sp& sp)
{
   Data::Geometry* geometry(0);
   typedef schema::job::sp::structure geom;
   auto structure = sp.add_layer<geom>();

   size_t natoms, nalpha, nbeta;
   int charge;
   structure.read(geom::natoms, natoms);
   structure.read(geom::nalpha, nalpha);
   structure.read(geom::nbeta,  nbeta);
   structure.read(geom::charge, charge);

   unsigned multiplicity = nalpha >= nbeta ? nalpha - nbeta + 1 : nbeta - nalpha + 1;

   std::vector<int> nuclei(natoms);
   std::vector<double> coords(3*natoms);

   libaview::array_view<double> av_coords(&coords[0],3*natoms);
   libaview::tens2<double> tens_coords(av_coords, 3, natoms);

   structure.read(geom::nuclei, nuclei);
   structure.read(geom::coordinates, tens_coords);

   qDebug() << "number of atoms in structure" << natoms;

   std::vector<unsigned> nuclei_std(natoms);
   for (size_t i = 0; i < natoms; ++i) {
       qDebug() << "Coordinates" << nuclei[i] << coords[3*i+0] << coords[3*i+1] << coords[3*i+2];
       nuclei_std[i] = nuclei[i];
   }

   geometry = new Data::Geometry(nuclei_std, coords);
   geometry->scaleCoordinates(Constants::BohrToAngstrom);
   geometry->setChargeAndMultiplicity(charge, multiplicity);
   geometry->setNAlpha(nalpha);
   geometry->setNBeta(nbeta);

   return geometry;
}


void Archive::readShellData(schema::job::sp& sp, Data::ShellData& shellData)
{
   typedef schema::job::sp::aobasis basis;
   auto aobasis = sp.add_layer<basis>();

   std::vector<int> shell_types;
   aobasis.read(basis::shell_types, shell_types);
   shellData.shellTypes = toQList(shell_types);

   std::vector<size_t> shell_to_atom_map;
   aobasis.read(basis::shell_to_atom_map, shell_to_atom_map);
   shellData.shellToAtom = toQList(shell_to_atom_map);
   
   std::vector<size_t> primitives_per_shell;
   aobasis.read(basis::primitives_per_shell, primitives_per_shell);
   shellData.shellPrimitives = toQList(primitives_per_shell);

   std::vector<double> primitive_exponents;
   aobasis.read(basis::primitive_exponents, primitive_exponents);
   shellData.exponents = toQList(primitive_exponents);

   std::vector<double> contraction_coefficients;
   aobasis.read(basis::contraction_coefficients, contraction_coefficients);
   shellData.contractionCoefficients = toQList(contraction_coefficients);

   std::vector<double> sp_contraction_coefficients;
   aobasis.read(basis::sp_contraction_coefficients, sp_contraction_coefficients);
   shellData.contractionCoefficientsSP = toQList(sp_contraction_coefficients);

   size_t nbasis;
   aobasis.read(basis::nbasis, nbasis);
   shellData.nBasis = nbasis;
}


void Archive::readDensityMatrix(schema::job::sp& sp, Data::DensityList& densityList)
{
   typedef schema::job::sp::energy_function::density_matrix dms;
   auto densities = sp.add_layer<dms>();

   size_t nbasis;
   auto aobasis = sp.add_layer<schema::job::sp::aobasis>();
   aobasis.read(schema::job::sp::aobasis::nbasis, nbasis);

   size_t nsets(2);
   densities.read(dms::nsets, nsets);

   std::vector<double> data(nsets*nbasis*nbasis);
   libaview::array_view<double> av_data(&data[0],nsets*nbasis*nbasis);
   libaview::tens3<double> tens_data(av_data, nsets, nbasis, nbasis);
   densities.read(dms::opdm, tens_data);

   QList<double> alphaData = toQList(&tens_data(0,0,0),nbasis*nbasis);
   QList<double> betaData  = toQList(&tens_data(nsets-1,0,0), nbasis*nbasis);

   bool square(true);
   Data::Density* alphaDensity = 
      new Data::Density(Data::SurfaceType::AlphaDensity, alphaData, "Alpha Density", square);
   Data::Density* betaDensity = 
      new Data::Density(Data::SurfaceType::BetaDensity, betaData, "Beta Density", square);
   densityList.append(alphaDensity);
   densityList.append(betaDensity);
 
}


void Archive::readOrbitalData(schema::job::sp& sp, Data::OrbitalData& orbitalData)
{
   typedef schema::job::sp::energy_function::method::scf::molecular_orbitals mos;
   auto orbs = sp.add_layer<mos>();

   schema::molecular_orbital_type orbitalType;

   orbs.read(mos::kind, orbitalType);
   // This should be taken from the HDF5 file directly
   switch (orbitalType) {
      case schema::molecular_orbital_type::scf: 
         orbitalData.orbitalType = Data::Orbitals::Canonical; 
         orbitalData.label = "Canonical Orbitals"; 
         break;
      case schema::molecular_orbital_type::edmiston_ruedenberg: 
         orbitalData.orbitalType = Data::Orbitals::Localized; 
         orbitalData.label = "Localized MOs (ER)";
         break; 
      case schema::molecular_orbital_type::boys: 
         orbitalData.orbitalType = Data::Orbitals::Localized; 
         orbitalData.label = "Localized MOs (Boys)";
         break;
      case schema::molecular_orbital_type::oslo: 
         orbitalData.orbitalType = Data::Orbitals::Localized; 
         orbitalData.label = "Localized MOs (OSLO)";
         break;
      case schema::molecular_orbital_type::dyson: 
         orbitalData.orbitalType = Data::Orbitals::Dyson;
         orbitalData.label = "Dyson Orbitals";
         break;
      case schema::molecular_orbital_type::sdhg2005: 
         orbitalData.orbitalType = Data::Orbitals::Generic;   
         orbitalData.label = "SDGH (2005) Orbitals";
         break; 
      case schema::molecular_orbital_type::ibo: 
         orbitalData.orbitalType = Data::Orbitals::Generic;
         orbitalData.label = "IBO Orbitals";
         break; 
   }

   orbitalData.stateIndex = 0;

   size_t nsets, norb, nbasis;
   orbs.read(mos::nsets, nsets);
   orbs.read(mos::norb,  norb);
   
   auto aobasis = sp.add_layer<schema::job::sp::aobasis>();
   aobasis.read(schema::job::sp::aobasis::nbasis, nbasis);
   
   // Coefficients
   std::vector<double> coeffs(nsets*nbasis*norb);
   libaview::array_view<double> av_coeffs(&coeffs[0],nsets*nbasis*norb);
   libaview::tens3<double> tens_coeffs(av_coeffs, nsets, nbasis, norb);
   orbs.read(mos::mo_coefficients, tens_coeffs);

   orbitalData.alphaCoefficients = toQList(&tens_coeffs(0,0,0),nbasis*norb);
   orbitalData.betaCoefficients  = toQList(&tens_coeffs(nsets-1,0,0), nbasis*norb);
   
   // Energies
   std::vector<double> energies(nsets*norb);
   libaview::array_view<double> av_energies(&energies[0],nsets*norb);
   libaview::tens2<double> tens_energies(av_energies, nsets, norb);
   orbs.read(mos::mo_energies, tens_energies);

   orbitalData.alphaEnergies = toQList(&tens_energies(0,0), norb);
   orbitalData.betaEnergies = toQList(&tens_energies(nsets-1,0), norb);
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

              Data::ShellData shellData;
              readShellData(*sp, shellData);

              Data::OrbitalData orbitalData;
              readOrbitalData(*sp, orbitalData);

              Data::DensityList densityList;
              readDensityMatrix(*sp, densityList);

              size_t nalpha(geometry->getNAlpha());
              size_t nbeta(geometry->getNBeta());

              Data::Orbitals* orbitals = Data::OrbitalFactory(nalpha, nbeta, 
                 orbitalData, shellData, *geometry, densityList);
              if (! orbitals) continue;

              // This needs proper data hierarchy organization
              m_dataBank.append(orbitals);
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
