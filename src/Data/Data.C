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

namespace Type {

QString chargeToString(ID const id)
{
    QString s("Unspecified");

    switch (id) {
       case GasteigerCharge:         s = "ESP (Gasteiger)";         break;
       case MullikenCharge:          s = "ESP (Mulliken)";          break;
       case MultipoleDerivedCharge:  s = "ESP (MDC)";               break;
       case ChelpgCharge:            s = "ESP (GHELPG)";            break;
       case HirshfeldCharge:         s = "ESP (Hershfeld)";         break;
       case LowdinCharge:            s = "ESP (Lowdin)";            break;
       case NaturalCharge:           s = "ESP (Natural)";           break;
       case MerzKollmanEspCharge:    s = "ESP (Merz-Kollma/ESP)";   break;
       case MerzKollmanRespCharge:   s = "ESP (Merz-Kollma/RESP)";  break;
       default:                                                     break;
    } 

    return s;
}



QString toString(ID const id)
{
   QString s("Invalid Type");

   switch (id) {
      case Undefined:                  s = "Data::Undefined";                 break;

      case Atom:                       s = "Data::Atom";                      break;
      case AtomList:                   s = "Data::List<Atom>";                break;
      case QmAtom:                     s = "Data::QmAtom";                    break;
      case QmAtomList:                 s = "Data::List<QmAtom>";              break;
      case MmAtom:                     s = "Data::MmAtom";                    break;
      case MmAtomList:                 s = "Data::List<MmAtom>";              break;
      case MacroMolecule:              s = "Data::MacroMolecule";             break;
      case Group:                      s = "Data::Group";                     break;
      case Pdb:                        s = "Data::Pdb";                       break;
      case ProteinChain:               s = "Data::ProteinChain";              break;
      case Solvent:                    s = "Data::Solvent";                   break;

      case Bank:                       s = "Data::Bank";                      break;
      case PointCharge:                s = "Data::PointCharge";               break;
      case PointChargeList:            s = "Data::List<PointCharge>";         break;
      case EfpFragment:                s = "Data::EfpFragment";               break;
      case EfpFragmentLibrary:         s = "Data::EfpFragmentLibrary";        break;
      case EfpFragmentList:            s = "Data::List<EfpFragment>";         break;
      case CubeData:                   s = "Data::CubeData";                  break;
      case GridData:                   s = "Data::GridData";                  break;
      case GridDataList:               s = "Data::GridDataList";              break;
      case File:                       s = "Data::File";                      break;
      case FileList:                   s = "Data::List<File>";                break;
      case Geometry:                   s = "Data::Geometry";                  break;
      case GeometryList:               s = "Data::List<Geometry>";            break;
      case RemSection:                 s = "Data::RemSection";                break;
      case Frequencies:                s = "Data::Frequencies";               break;
      case VibrationalMode:            s = "Data::VibrationalMode";           break;
      case VibrationalModeList:        s = "Data::VibrationalModeList";       break;
      case Vibronic:                   s = "Data::Vibronic";                  break;
      case VibronicSpectrum:           s = "Data::VibronicSpectrum";          break;

      case Orbitals:                   s = "Data::Orbitals";                  break;
      case OrbitalsList:               s = "Data::OrbitalsList";              break;
      case LocalizedOrbitals:          s = "Data::LocalizedOrbitals";         break;
      case CanonicalOrbitals:          s = "Data::CanonicalOrbitals";         break;
      case NaturalTransitionOrbitals:  s = "Data::NaturalTransitionOrbitals"; break;
      case NaturalBondOrbitals:        s = "Data::NaturalBondlOrbitals";      break;
      case DysonOrbitals:              s = "Data::DysonOrbitals";             break;

      case Density:                    s = "Data::Density";                   break;
      case DensityList:                s = "Data::DensityList";               break;

      case Shell:                      s = "Data::Shell";                     break;
      case ShellList:                  s = "Data::ShellList";                 break;
      case Energy:                     s = "Data::Energy";                    break;
      case Hessian:                    s = "Data::Hessian";                   break;
      case ConformerEnergy:            s = "Data::ConformerEnergy";           break;

      case AtomicProperty:             s = "Data::AtomicProperty";            break;
      case AtomicSymbol:               s = "Data::AtomicSymbol";              break;
      case AtomicNumber:               s = "Data::AtomicNumber";              break;
      case AtomColor:                  s = "Data::AtomColor";                 break;
      case NmrShielding:               s = "Data::NmrShielding";              break;
      case NmrShift:                   s = "Data::NmrShift";                  break;
      case Mass:                       s = "Data::Mass";                      break;

      case AtomicCharge:               s = "Data::AtomicCharge";              break;
      case MullikenCharge:             s = "Data::MullikenCharge";            break;
      case GasteigerCharge:            s = "Data::GasteigerCharge";           break;
      case MultipoleDerivedCharge:     s = "Data::MultipoleDerivedCharge";    break;
      case LowdinCharge:               s = "Data::LowdinCharge";              break;
      case ChelpgCharge:               s = "Data::ChelpgCharge";              break;
      case HirshfeldCharge:            s = "Data::HirshfeldCharge";           break;
      case NaturalCharge:              s = "Data::NaturalCharge";             break;
      case MerzKollmanEspCharge:       s = "Data::MerzKollmanEspCharge";      break;
      case MerzKollmanRespCharge:      s = "Data::MerzKollmanRespCharge";     break;
      case Cm5Charge:                  s = "Data::Cm5Charge";                 break;

      case SpinDensity:                s = "Data::SpinDensity";               break;
      case VdwRadius:                  s = "Data::VdwRadius";                 break;
      case DipoleMoment:               s = "Data::DipoleMoment";              break;
      case TotalEnergy:                s = "Data::TotalEnergy";               break;
      case ScfEnergy:                  s = "Data::ScfEnergy";                 break;
      case ForceFieldEnergy:           s = "Data::ForceFieldEnergy";          break;
      case PointGroup:                 s = "Data::PointGroup";                break;
      case ResidueName:                s = "Data::ResidueName";               break;

      case Constraint:                 s = "Data::Constraint:";               break;
      case PositionConstraint:         s = "Data::PositionConstraint:";       break;
      case DistanceConstraint:         s = "Data::DistanceConstraint:";       break;
      case AngleConstraint:            s = "Data::AngleConstraint:";          break;
      case TorsionConstraint:          s = "Data::TorsionConstraint:";        break;
      case FrozenAtomsConstraint:      s = "Data::FrozenAtomsConstraint:";    break;

      case ChargeMultiplicity:         s = "Data::ChargeMultiplicity";        break;
      case MultipoleExpansion:         s = "Data::MultipoleExpansion";        break;
      case MultipoleExpansionList:     s = "Data::MultipoleExpansionList";    break;

      case Mesh:                       s = "Data::Mesh";                      break;
      case MeshList:                   s = "Data::MeshList";                  break;
      case Surface:                    s = "Data::Surface";                   break;
      case SurfaceType:                s = "Data::SurfaceType";               break;
      case SurfaceList:                s = "Data::SurfaceList";               break;
      case SurfaceInfo:                s = "Data::SurfaceInfo";               break;
      case SurfaceInfoList:            s = "Data::SurfaceInfoList";           break;

      case ElectronicTransition:       s = "Data::ElectronicTransition";      break;
      case ElectronicTransitionList:   s = "Data::ElectronicTransitionList";  break;
      case ExcitedStates:              s = "Data::ExcitedStates";             break;
      case NmrReference:               s = "Data::NmrReference";              break;
      case NmrReferenceList:           s = "Data::NmrReferenceList";          break;
      case Nmr:                        s = "Data::Nmr";                       break;

      case OrbitalSymmetries:          s = "Data::OrbitalSymmetries";         break;
      case YamlNode:                   s = "Data::YamlNode";                  break;
      case PovRay:                     s = "Data::PovRay";                    break;
      case GeminalOrbitals:            s = "Data::GeminalOrbitals";           break;

      case AminoAcid:                  s = "Data::AminoAcid";                 break;
      case AminoAcidList:              s = "Data::AminoAcidList";             break;
      case Residue:                    s = "Data::Residue";                   break;
   }

   return s;
}

} // end namespace Type


} } // end namespace IQmol::Data
