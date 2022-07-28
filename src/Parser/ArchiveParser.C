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
#include "ReorderBasis.h"
#include "Util/QsLog.h"
#include "Util/Constants.h"
#include "Data/AtomicProperty.h"
#include "Data/DipoleMoment.h"
#include "Data/GeometryList.h"
#include "Data/OrbitalFactory.h"
#include "Data/OrbitalsList.h"
#include "Data/Frequencies.h"
#include "Data/VibrationalMode.h"
#include "Data/PointCharge.h"

#include <QFile>
#include <QDebug>

template<typename T>
QList<T> toQList(std::vector<T> vec)
{
   return QList<T>::fromVector(QVector<T>::fromStdVector(vec));
}


QList<unsigned> toQList(std::vector<unsigned long long> vec)
{
   QList<unsigned> out;

   std::vector<unsigned long long>::const_iterator iter;

   for (iter = vec.begin(); iter != vec.end(); ++iter) {
       out.push_back(unsigned(*iter));
   }

   return out;
}


template<typename T>
QList<T> toQList(T const* start, size_t const n)
{
   QList<T> out;
   T const* iter(start);

   for (size_t i = 0; i  < n; ++i, ++iter) {
       out.push_back(*iter);
   }

   return out;
}


namespace IQmol {
namespace Parser {


void Archive::readGeometry(Schema::Structure& structure, Data::Geometry& geometry)
{
   size_t natoms, nalpha, nbeta;
   int charge;
   structure.read(Schema::Structure::natoms, natoms);
   structure.read(Schema::Structure::charge, charge);
   structure.read(Schema::Structure::nalpha, nalpha);
   structure.read(Schema::Structure::nbeta,  nbeta);

   // cordinates
   std::vector<double> coords(3*natoms);
   libaview::array_view<double> av_coords(&coords[0],3*natoms);
   libaview::tens2<double> tens_coords(av_coords, 3, natoms);
   structure.read(Schema::Structure::coordinates, tens_coords);

   // nuclei
   std::vector<int> nuclei(natoms);
   structure.read(Schema::Structure::nuclei, nuclei);
   std::vector<unsigned> nuclei_std(natoms);
   for (size_t i = 0; i < natoms; ++i) {
       nuclei_std[i] = nuclei[i];
   }

   // point_group_symmetry (ignored for now)

   unsigned multiplicity = nalpha >= nbeta ? nalpha - nbeta + 1 : nbeta - nalpha + 1;

   geometry.set(toQList(nuclei_std), toQList(coords));
   geometry.scaleCoordinates(Constants::BohrToAngstrom);
   geometry.setChargeAndMultiplicity(charge, multiplicity);
   geometry.setNAlpha(nalpha);
   geometry.setNBeta(nbeta);
}


void Archive::readExternalCharges(Schema::Structure& structure, Data::PointChargeList& pointChargeList)
{
   Schema::ExternalCharges charges = structure.add_layer<Schema::ExternalCharges>();

   if (! charges.contains(Schema::ExternalCharges::ncharges)) return;
   size_t ncharges(0);
   charges.read(Schema::ExternalCharges::ncharges, ncharges);
   if (ncharges == 0) return;

   std::vector<double> pcCoords(3*ncharges);
   libaview::array_view<double> av_pcCoords(&pcCoords[0],3*ncharges);
   libaview::tens2<double> tens_pcCoords(av_pcCoords, 3, ncharges);
   charges.read(Schema::ExternalCharges::coordinates, tens_pcCoords);

   std::vector<double> pcCharges(ncharges);
   charges.read(Schema::ExternalCharges::charges, pcCharges);

   for (size_t i = 0; i < ncharges; ++i) {
       qglviewer::Vec pos(tens_pcCoords(0,i), tens_pcCoords(1,i), tens_pcCoords(2,i)); 
       pos *= Constants::BohrToAngstrom;
       pointChargeList.append(new Data::PointCharge(pcCharges[i],pos));

   }
}


void Archive::readShellData(Schema::SinglePoint& sp, Data::ShellData& shellData)
{
   auto aobasis = sp.add_layer<Schema::AOBasis>(); 
   std::vector<int> shell_types;
   aobasis.read(Schema::AOBasis::shell_types, shell_types);
   // this needs to depend on the schema version number
   // if (invertedShellTypes) Negative L => cartesian for L >= 2
   bool invertedShellTypes(true); 
   if (invertedShellTypes) {
      for (size_t i(0); i < shell_types.size(); ++i) {
          if (std::abs(shell_types[i]) >= 2) shell_types[i] = -shell_types[i];
      }
   }
   shellData.shellTypes = toQList(shell_types);

   std::vector<size_t> shell_to_atom_map;
   aobasis.read(Schema::AOBasis::shell_to_atom_map, shell_to_atom_map);
   for (size_t i(0); i < shell_to_atom_map.size(); ++i) {
       ++shell_to_atom_map[i]; // Convert to 1-indexed values
   }
   shellData.shellToAtom = toQList(shell_to_atom_map);
   
   std::vector<size_t> primitives_per_shell;
   aobasis.read(Schema::AOBasis::primitives_per_shell, primitives_per_shell);
   shellData.shellPrimitives = toQList(primitives_per_shell);

   std::vector<double> primitive_exponents;
   aobasis.read(Schema::AOBasis::primitive_exponents, primitive_exponents);
   shellData.exponents = toQList(primitive_exponents);

   std::vector<double> contraction_coefficients;
   aobasis.read(Schema::AOBasis::contraction_coefficients, contraction_coefficients);
   shellData.contractionCoefficients = toQList(contraction_coefficients);

   std::vector<double> sp_contraction_coefficients;
   aobasis.read(Schema::AOBasis::sp_contraction_coefficients, sp_contraction_coefficients);
   shellData.contractionCoefficientsSP = toQList(sp_contraction_coefficients);

   size_t nbasis;
   aobasis.read(Schema::AOBasis::nbasis, nbasis);
   shellData.nBasis = nbasis;
}


void Archive::readDensityMatrix(Schema::EnergyFunction& ef, Data::ShellData const& shellData, 
   Data::DensityList& densityList)
{
   typedef Schema::EnergyFunction::density_matrix dms;
   auto densities = ef.add_layer<dms>();

   size_t nsets(2);
   densities.read(dms::nsets, nsets);

   size_t nbasis(shellData.nBasis);
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


void Archive::readObservables(Schema::Observables& observables, Data::Geometry& geometry)
{
   try {
      auto mm = observables.add_layer<Schema::MultipoleMoments>();
      std::vector<double> dipole(3);
      mm.read(Schema::MultipoleMoments::dipole, dipole);
      geometry.getProperty<Data::DipoleMoment>().setValue(dipole[0],dipole[1],dipole[2]);
   }catch (...) { }

   try {
      auto nmr = observables.add_layer<Schema::NmrShieldings>();
      auto list = nmr.get_iter_layers<Schema::NmrShieldings::centers>();
      QList<double> shieldings;

      for (auto iter = list.begin(); iter != list.end(); ++iter) {
          std::vector<double> s(9);
          iter->read(Schema::NmrShieldings::centers::total, s);
          double trace = s[0] + s[4] + s[8];
          shieldings.push_back(trace);
      }
      geometry.setAtomicProperty<Data::NmrShielding>(shieldings); 
   }catch (...) { }
}


void Archive::readAnalysis(Schema::Analysis& analysis, Data::Geometry& geometry)
{
   try {
      std::vector<Schema::Vibrational> vibList = analysis.get_iter_layers<Schema::Vibrational>(); 
      for (auto va = vibList.begin(); va != vibList.end(); ++va) {
          Data::Frequencies* frequencies = new Data::Frequencies;
          readVibrationalData(*va,*frequencies);
          if (frequencies->nModes() >0) {
             m_dataBank.append(frequencies);
          }else {
             delete frequencies;
          }
      }
   }catch (...) { }

   try {
      std::vector<Schema::AtomicCharges> list = analysis.get_iter_layers<Schema::AtomicCharges>();
      for (auto atomicCharges = list.begin(); atomicCharges != list.end(); ++atomicCharges) {
          readAtomicCharges(*atomicCharges, geometry);
      }
   }catch (...) { }
}


void Archive::readLocalizedOrbitals(Schema::Analysis& analysis, Data::ShellData const& shellData, 
   Data::Geometry const& geometry,  Data::OrbitalsList& orbitalsList)
{
   try {
      size_t nalpha(geometry.getNAlpha());
      size_t nbeta(geometry.getNBeta());

      std::vector<Schema::Analysis::localized_orbitals> 
         list = analysis.get_iter_layers<Schema::Analysis::localized_orbitals>();
      QLOG_INFO() << "Found" << list.size() << "sets of localized orbitals";
      for (auto mos = list.begin(); mos != list.end(); ++mos) {

          auto loc_mos = mos->add_layer<Schema::LocalizedOrbitals>();

          Data::OrbitalData orbitalData;
          readOrbitalData(loc_mos, shellData, orbitalData);

          Data::Orbitals* orbitals = Data::OrbitalFactory(nalpha, nbeta, 
             orbitalData, shellData, geometry);
          if (orbitals) orbitalsList.append(orbitals);
          QLOG_INFO() << "Localized orbitals made:" << orbitals;

      }
   }catch (...) { }
}


void Archive::readVibrationalData(Schema::Vibrational& vibData, Data::Frequencies& freqs)
{
   std::vector<double> frequencies;
   vibData.read(Schema::Vibrational::frequencies, frequencies);

   std::vector<double> ir_intensities;
   vibData.read(Schema::Vibrational::ir_intensities, ir_intensities);

   std::vector<double> raman_intensities;

   bool hasRaman(vibData.contains(Schema::Vibrational::raman_intensities));
   if (hasRaman) vibData.read(Schema::Vibrational::raman_intensities, raman_intensities);
   
   size_t natoms,nmodes;
   vibData.read(Schema::Vibrational::natoms, natoms);
   vibData.read(Schema::Vibrational::nmodes, nmodes);

   std::vector<double> modes(nmodes*natoms*3);
   libaview::array_view<double> av_modes(&modes[0],nmodes*natoms*3);
   libaview::tens3<double> tens_modes(av_modes, nmodes, natoms, 3);
   vibData.read(Schema::Vibrational::modes, tens_modes);

   for (size_t i = 0; i < frequencies.size(); ++i) {
       Data::VibrationalMode* mode = new Data::VibrationalMode(frequencies[i]);
       mode->setIntensity(ir_intensities[i]);
       if (hasRaman) mode->setRamanIntensity(raman_intensities[i]);
       for (size_t j = 0; j < natoms; ++j) {
           mode->appendDirectionVector(
              qglviewer::Vec(tens_modes(i,j,0),tens_modes(i,j,1),tens_modes(i,j,2))
           );
       }
       freqs.append(mode); 
   }
    
   auto thermo = vibData.add_layer<Schema::Thermodynamics>();
   bool hasThermo(thermo.contains(Schema::Thermodynamics::zpve));

   QLOG_INFO() << "Do we have HDF5 thermo data: " << hasThermo;
   if (hasThermo) {
      auto thermoData = vibData.add_layer<Schema::Thermodynamics>();
      double zpve(0), temperature(0), pressure(0), entropy(0), enthalpy(0);

      thermoData.read(Schema::Thermodynamics::zpve, zpve);
      thermoData.read(Schema::Thermodynamics::temperature, temperature);
      thermoData.read(Schema::Thermodynamics::pressure, pressure);
      thermoData.read(Schema::Thermodynamics::entropy, entropy);

      freqs.setThermochemicalData(zpve, enthalpy, entropy, temperature, pressure);
   }
}


void Archive::readAtomicCharges(Schema::AtomicCharges& ac, Data::Geometry& geom)
{
   Schema::AtomicChargesTheory theory;
   ac.read(Schema::AtomicCharges::theory, theory);

   std::vector<double> charges;
   ac.read(Schema::AtomicCharges::charges, charges);

   switch (theory) {
      case Schema::AtomicChargesTheory::mulliken:
         geom.setAtomicProperty<Data::MullikenCharge>(toQList(charges));
         break;
      case Schema::AtomicChargesTheory::lowdin:
         geom.setAtomicProperty<Data::LowdinCharge>(toQList(charges));
         break;
      case Schema::AtomicChargesTheory::chelpg:
         geom.setAtomicProperty<Data::ChelpgCharge>(toQList(charges));
         break;
      case Schema::AtomicChargesTheory::hirshfeld:
         geom.setAtomicProperty<Data::HirshfeldCharge>(toQList(charges));
         break;
      case Schema::AtomicChargesTheory::cm5:
         geom.setAtomicProperty<Data::Cm5Charge>(toQList(charges));
         break;
      case Schema::AtomicChargesTheory::stewart:
         geom.setAtomicProperty<Data::MultipoleDerivedCharge>(toQList(charges));
         break;
      case Schema::AtomicChargesTheory::nbo:
         geom.setAtomicProperty<Data::NaturalCharge>(toQList(charges));
         break;
      case Schema::AtomicChargesTheory::merz_kollman_esp:
         geom.setAtomicProperty<Data::MerzKollmanEspCharge>(toQList(charges));
         break;
      case Schema::AtomicChargesTheory::merz_kollman_resp:
         geom.setAtomicProperty<Data::MerzKollmanRespCharge>(toQList(charges));
         break;
      default:
         QLOG_WARN() << "Unknown atomic_charge_theory encountered";;
         break;
   }
}


void Archive::readOrbitalData(Schema::LocalizedOrbitals& orbs, Data::ShellData const& shellData, 
   Data::OrbitalData& orbitalData)
{
   Schema::MolecularOrbitalType orbitalType;
   orbs.read(Schema::LocalizedOrbitals::molecular_orbitals::kind, orbitalType);

   // This should be taken from the HDF5 file directly
   switch (orbitalType) {
      case Schema::MolecularOrbitalType::scf: 
         orbitalData.orbitalType = Data::Orbitals::Canonical; 
         orbitalData.label = "Canonical Orbitals"; 
         break;
      case Schema::MolecularOrbitalType::edmiston_ruedenberg: 
         orbitalData.orbitalType = Data::Orbitals::Localized; 
         orbitalData.label = "Localized MOs (ER)";
         break; 
      case Schema::MolecularOrbitalType::boys: 
         orbitalData.orbitalType = Data::Orbitals::Localized; 
         orbitalData.label = "Localized MOs (Boys)";
         break;
      case Schema::MolecularOrbitalType::pipek_mezey: 
         orbitalData.orbitalType = Data::Orbitals::Localized; 
         orbitalData.label = "Localized MOs (Pipek Mezey)";
         break;
      case Schema::MolecularOrbitalType::oslo: 
         orbitalData.orbitalType = Data::Orbitals::Localized; 
         orbitalData.label = "Localized MOs (OSLO)";
         break;
      case Schema::MolecularOrbitalType::dyson: 
         orbitalData.orbitalType = Data::Orbitals::Dyson;
         orbitalData.label = "Dyson Orbitals";
         break;
      case Schema::MolecularOrbitalType::sdhg2005: 
         orbitalData.orbitalType = Data::Orbitals::Generic;   
         orbitalData.label = "SDGH (2005) Orbitals";
         break; 
      case Schema::MolecularOrbitalType::ibo: 
         orbitalData.orbitalType = Data::Orbitals::Generic;
         orbitalData.label = "IBO Orbitals";
         break; 
   }

   QLOG_INFO() << "Reading data for localized orbitals:" << orbitalData.label;

   orbitalData.stateIndex = 0;

   size_t nbasis(shellData.nBasis);
   size_t nsets, norb;
   orbs.read(Schema::LocalizedOrbitals::nsets, nsets);
   orbs.read(Schema::LocalizedOrbitals::norb,  norb);
   

   // Coefficients
   std::vector<double> coeffs(nsets*nbasis*norb);
   libaview::array_view<double> av_coeffs(&coeffs[0],nsets*nbasis*norb);
   libaview::tens3<double> tens_coeffs(av_coeffs, nsets, nbasis, norb);
   orbs.read(Schema::LocalizedOrbitals::mo_coefficients, tens_coeffs);

   orbitalData.alphaCoefficients.clear();
   orbitalData.betaCoefficients.clear();

   for (size_t i = 0; i < norb; ++i) {
       for (size_t mu = 0; mu < nbasis; ++mu) {
           orbitalData.alphaCoefficients.append(tens_coeffs(0,      mu,i));
           orbitalData.betaCoefficients.append( tens_coeffs(nsets-1,mu,i));
       }
   }

   QList<unsigned> map(ReorderQChemToFchk(shellData.shellTypes));
   ReorderMOs(&orbitalData.alphaCoefficients[0], map, nbasis, norb);
   ReorderMOs(&orbitalData.betaCoefficients[0], map, nbasis, norb);
}


void Archive::readOrbitalData(Schema::MolecularOrbitals& orbs, Data::ShellData const& shellData, 
   Data::OrbitalData& orbitalData)
{
   Schema::MolecularOrbitalType orbitalType;
   orbs.read(Schema::MolecularOrbitals::kind, orbitalType);

   // This should be taken from the HDF5 file directly
   switch (orbitalType) {
      case Schema::MolecularOrbitalType::scf: 
         orbitalData.orbitalType = Data::Orbitals::Canonical; 
         orbitalData.label = "Canonical Orbitals"; 
         break;
      case Schema::MolecularOrbitalType::edmiston_ruedenberg: 
         orbitalData.orbitalType = Data::Orbitals::Localized; 
         orbitalData.label = "Localized MOs (ER)";
         break; 
      case Schema::MolecularOrbitalType::boys: 
         orbitalData.orbitalType = Data::Orbitals::Localized; 
         orbitalData.label = "Localized MOs (Boys)";
         break;
      case Schema::MolecularOrbitalType::pipek_mezey: 
         orbitalData.orbitalType = Data::Orbitals::Localized; 
         orbitalData.label = "Localized MOs (Pipek Mezey)";
         break;
      case Schema::MolecularOrbitalType::oslo: 
         orbitalData.orbitalType = Data::Orbitals::Localized; 
         orbitalData.label = "Localized MOs (OSLO)";
         break;
      case Schema::MolecularOrbitalType::dyson: 
         orbitalData.orbitalType = Data::Orbitals::Dyson;
         orbitalData.label = "Dyson Orbitals";
         break;
      case Schema::MolecularOrbitalType::sdhg2005: 
         orbitalData.orbitalType = Data::Orbitals::Generic;   
         orbitalData.label = "SDGH (2005) Orbitals";
         break; 
      case Schema::MolecularOrbitalType::ibo: 
         orbitalData.orbitalType = Data::Orbitals::Generic;
         orbitalData.label = "IBO Orbitals";
         break; 
   }

   orbitalData.stateIndex = 0;

   size_t nbasis(shellData.nBasis);
   size_t nsets, norb;
   orbs.read(Schema::MolecularOrbitals::nsets, nsets);
   orbs.read(Schema::MolecularOrbitals::norb,  norb);
   
   // Coefficients
   std::vector<double> coeffs(nsets*nbasis*norb);
   libaview::array_view<double> av_coeffs(&coeffs[0],nsets*nbasis*norb);
   libaview::tens3<double> tens_coeffs(av_coeffs, nsets, nbasis, norb);
   orbs.read(Schema::MolecularOrbitals::mo_coefficients, tens_coeffs);

   orbitalData.alphaCoefficients.clear();
   orbitalData.betaCoefficients.clear();

   for (size_t i = 0; i < norb; ++i) {
       for (size_t mu = 0; mu < nbasis; ++mu) {
           orbitalData.alphaCoefficients.append(tens_coeffs(0,      mu,i));
           orbitalData.betaCoefficients.append( tens_coeffs(nsets-1,mu,i));
       }
   }

   QList<unsigned> map(ReorderQChemToFchk(shellData.shellTypes));
   ReorderMOs(&orbitalData.alphaCoefficients[0], map, nbasis, norb);
   ReorderMOs(&orbitalData.betaCoefficients[0], map, nbasis, norb);

   // Energies - checked
   std::vector<double> energies(nsets*norb);
   libaview::array_view<double> av_energies(&energies[0],nsets*norb);
   libaview::tens2<double> tens_energies(av_energies, norb, nsets);
   orbs.read(Schema::MolecularOrbitals::mo_energies, tens_energies);

   orbitalData.alphaEnergies.clear();
   orbitalData.betaEnergies.clear();

   for (size_t i = 0; i < norb; ++i){
       orbitalData.alphaEnergies.append(tens_energies(i,0));
       orbitalData.betaEnergies.append(tens_energies(i,nsets-1));
   }
}


bool Archive::parseFile(QString const& filePath)
{
   bool ok(true);

   try {
      libarchive::impl::QArchive archive(filePath.toStdString(), libstore::FileMode::ReadOnly);
      std::vector<Schema::Job>  jobList = archive.get_jobs();
      QLOG_INFO() << "Found" << jobList.size() << "jobs in archive file" << filePath;

      for (size_t j(0); j < jobList.size(); ++j) {

          QString label("Job " + QString::number(j));
          Data::GeometryList* geometryList(new Data::GeometryList(label));
          Data::OrbitalsList* orbitalsList(new Data::OrbitalsList());

          std::vector<Schema::SinglePoint> spList = archive.get_single_points(jobList[j]);
          qDebug() << "Number of single points:" << spList.size();

          for (auto sp = spList.begin(); sp != spList.end(); ++sp) {

              Schema::Structure structure = sp->add_layer<Schema::Structure>();
   
              Data::Geometry* geometry = new Data::Geometry;
              readGeometry(structure, *geometry);

              if (geometry->nAtoms() == 0) {
                 QLOG_WARN() << "Unable to get geometry";
                 delete geometry;
                 continue;
              }
              geometryList->append(geometry);

              Data::PointChargeList* pointCharges = new Data::PointChargeList;
              readExternalCharges(structure, *pointCharges);
              if (pointCharges->size() > 0) {
                 m_dataBank.append(pointCharges);
              }else {
                 delete pointCharges;
              }

              Data::ShellData shellData;
              readShellData(*sp, shellData);

              size_t nalpha(geometry->getNAlpha());
              size_t nbeta(geometry->getNBeta());

              // need to loop over energy functions
              std::vector<Schema::EnergyFunction> efList = sp->get_iter_layers<Schema::EnergyFunction>();

              for (auto ef = efList.begin(); ef != efList.end(); ++ef) {

                  auto meth = ef->add_layer<Schema::EnergyFunction::method>();
                  auto scf  = meth.add_layer<Schema::EnergyFunction::method::scf>();
                  auto orbs = scf.add_layer<Schema::MolecularOrbitals>();

                  Data::OrbitalData orbitalData;
                  readOrbitalData(orbs, shellData, orbitalData);

                  Data::DensityList densityList;
                  readDensityMatrix(*ef, shellData, densityList);

                  Data::Orbitals* orbitals = Data::OrbitalFactory(nalpha, nbeta, 
                     orbitalData, shellData, *geometry, densityList);
                  if (orbitals) orbitalsList->append(orbitals);

                  auto analysis = ef->add_layer<Schema::Analysis>();
                  readAnalysis(analysis, *geometry);

                  readLocalizedOrbitals(analysis, shellData, *geometry, *orbitalsList);
                  
                  auto observables = ef->add_layer<Schema::Observables>();
                  readObservables(observables, *geometry);
              }
          }

          if (!geometryList->isEmpty()) m_dataBank.append(geometryList);
          if (!orbitalsList->isEmpty()) m_dataBank.append(orbitalsList);
      }

   } catch (std::exception& e) {
      m_errors.append(e.what());
      return false;
   }

   return ok;
}

} } // end namespace IQmol::Parser
