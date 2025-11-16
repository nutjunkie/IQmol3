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

#include "VibrationalMode.h"


namespace IQmol {
namespace Data {

   class Frequencies : public Base {

      public:
         Type::ID typeID() const { return Type::Frequencies; }

         Frequencies() : m_zpve(0.0), m_entropy(0.0), m_enthalpy(0.0), m_haveRaman(false) { }

         void append(VibrationalMode* mode) { m_modes.append(mode); }
         void setThermochemicalData(double const zpve, double const enthalpy, 
            double const entropy, double const temperature = 298.15, double const pressure = 1.0);

         void haveRaman(bool tf) { m_haveRaman = tf; }
         bool haveRaman() const { return m_haveRaman; }
         double zpve() const { return m_zpve; }

         double maxFrequency() const;
         double maxIntensity() const;
         double maxRamanIntensity() const;
         QList<double> frequencies() const;

         unsigned nModes() const { return m_modes.size(); }

         void dump() const;
         VibrationalModeList const& modes() const { return m_modes; }

         VibrationalMode const& mode(unsigned i) const { return *m_modes[i]; }

      private:
         double m_zpve;
         double m_entropy;
         double m_enthalpy;
         double m_temperature;
         double m_pressure;
         bool   m_haveRaman; 
         VibrationalModeList m_modes;
   };

} } // end namespace IQmol::Data
