#ifndef IQMOL_UTIL_CONSTANTS_H
#define IQMOL_UTIL_CONSTANTS_H
/*******************************************************************************

  Copyright (C) 2011-2015 Andrew Gilbert

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

#include <QString>


namespace IQmol {
namespace Constants {

   // Values taken from the source of all knowledge: Wikipedia

   double const BohrRadius          = 5.2917721092e-11;
   double const BohrToAngstrom      = BohrRadius*1.0e10;
   double const AngstromToBohr      = 1.0/BohrToAngstrom;

   double const Mole                = 6.02214129e23;

   double const JoulesPerCalorie    = 4.184;
   double const HartreeToEv         = 27.21138386;
   double const HartreeToWavenumber = 219474.6314;
   double const HartreeToMHz        = 6.579683921e9;
   double const HartreeToKJmol      = 2625.49962;
   double const HartreeToKCalmol    = HartreeToKJmol/JoulesPerCalorie;
   double const HartreeToNanometer  = 45.56337117; // Need to divide this by energy
   double const NanometerToHartree  = 33737.82860; // Need to divide this by energy

   enum Units { 
      Hartree = 0, 
      ElectronVolt, 
      Wavenumber, 
      KJoulePerMol, 
      KCalPerMol, 
      MegaHertz,
      Nanometer
   };

   const Units constexpr AllUnits[] = {
      Hartree, 
      ElectronVolt, 
      Wavenumber, 
      KJoulePerMol, 
      KCalPerMol, 
      MegaHertz,
      Nanometer
   };


   inline QString toString(Units const units)
   {
      QString s;
      switch (units) {
         case Hartree:       s = "Hartree";    break;
         case ElectronVolt:  s = "eV";         break;
         case Wavenumber:    s = "cm⁻¹";       break;
         case KJoulePerMol:  s = "KJ mol⁻¹";   break;
         case KCalPerMol:    s = "KCal mol⁻¹"; break;
         case MegaHertz:     s = "MHz";        break;
         case Nanometer:     s = "nm";         break;
      }
      return s;
   }


/*
   inline double convertFromHartree(Units const units, double const energy = 1.0)
   {
      double conv(1.0);
      switch (units) {
         case Hartree:       conv = 1.0;                  break;
         case ElectronVolt:  conv = HartreeToEv;          break;
         case Wavenumber:    conv = HartreeToWavenumber;  break;
         case KJoulePerMol:  conv = HartreeToKJmol;       break;
         case KCalPerMol:    conv = HartreeToKCalmol;     break;
         case MegaHertz:     conv = HartreeToMHz;         break;
         case NanoMeters:
      }
      return energy*conv;
   }
*/


   inline double convert(double const value, Units const from, Units const to)
   {
      if (from == to) return value;
      // First convert to Hartree
      double conv(value);

      switch (from) {
         case Hartree:       conv /= 1.0;                        break;
         case ElectronVolt:  conv /= HartreeToEv;                break;
         case Wavenumber:    conv /= HartreeToWavenumber;        break;
         case KJoulePerMol:  conv /= HartreeToKJmol;             break;
         case KCalPerMol:    conv /= HartreeToKCalmol;           break;
         case MegaHertz:     conv /= HartreeToMHz;               break;
         case Nanometer:     conv  = NanometerToHartree/value;  break;
      }

      switch (to) {
         case Hartree:       conv *= 1.0;                      break;
         case ElectronVolt:  conv *= HartreeToEv;              break;
         case Wavenumber:    conv *= HartreeToWavenumber;      break;
         case KJoulePerMol:  conv *= HartreeToKJmol;           break;
         case KCalPerMol:    conv *= HartreeToKCalmol;         break;
         case MegaHertz:     conv *= HartreeToMHz;             break;
         case Nanometer:     conv  = HartreeToNanometer/conv;  break;
      }

      return conv; 
   }

} } // end namespace IQmol::Constants

#endif
